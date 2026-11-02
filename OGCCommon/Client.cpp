#include "OGCClient.h"
#include <windows.h>
#include <winhttp.h>
#include <objbase.h>
#include <xmllite.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <locale>
#include <iomanip>
#include <cmath>
#include <climits>
#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace 
{
	using Params=std::map<std::string,std::string>;
	struct Invalid:std::runtime_error
	{
		using runtime_error::runtime_error;
	};

	const char* value(const char* s,const char* def="")
	{
		return s?s:def;
	}

	void require(bool ok,const char* why)
	{
		if(!ok)
			throw Invalid(why);
	}

	std::string upper(std::string s)
	{
		for(char& c:s)
			if(c>='a'&&c<='z')
				c-=32;
		return s;
	}

	std::string encode(const std::string& s)
	{
		const char*h="0123456789ABCDEF";
		std::string o;
		for(unsigned char c:s)
		{
			if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~')
				o+=c;
			else
			{
				o+='%';
				o+=h[c>>4];
				o+=h[c&15];
			}
		}
		return o;
	}

	std::string num(double d)
	{
		require(std::isfinite(d),"Non-finite bounding box");
		std::ostringstream s;
		s.imbue(std::locale::classic());
		s<<std::setprecision(15)<<d;
		return s.str();
	}

	std::string bbox(double x1,double y1,double x2,double y2,bool swap)
	{
		require(x1<x2&&y1<y2,"Invalid BBOX: split antimeridian requests into two boxes");
		return swap? num(y1)+","+num(x1)+","+num(y2)+","+num(x2): num(x1)+","+num(y1)+","+num(x2)+","+num(y2);
	}

	void versionOK(const std::string& s,const char* service)
	{
		bool ok=false;
		if(!strcmp(service,"WMS"))
			ok=s=="1.3.0"||s=="1.1.1";
		if(!strcmp(service,"WFS"))
			ok=s=="2.0.0"||s=="1.1.0"||s=="1.0.0";
		if(!strcmp(service,"WMTS"))
			ok=s=="1.0.0";
		require(ok,"Unsupported protocol version");
	}

	std::wstring wide(const std::string& s)
	{
		if(s.empty())	return {};

		require(s.size()<=INT_MAX,"String too long");
		int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s.data(),(int)s.size(),nullptr,0);
		require(n>0,"Invalid UTF-8");
		std::wstring w(n,0);MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s.data(),(int)s.size(),&w[0],n);

		return w;
	}

	int hex(char c)
	{
		if(c>='0'&&c<='9')
			return c-'0';
		if(c>='a'&&c<='f')
			return c-'a'+10;
		if(c>='A'&&c<='F')
			return c-'A'+10;
		return -1;
	}

	std::string keyDecode(std::string s)
	{
		std::string o;
		for(size_t i=0;i<s.size();++i)
		{
			if(s[i]=='%'&&i+2<s.size()&&hex(s[i+1])>=0&&hex(s[i+2])>=0)
			{
				o+=char(hex(s[i+1])*16+hex(s[i+2]));
				i+=2;
			}
			else
			{
				o += s[i];
			}
		}
		return upper(o);
	}

	std::string url(const OGC_Connection* c, const Params& p)
	{
		 require(c && c->endpoint && *c->endpoint, "Missing endpoint");
		 std::string base = c->endpoint;
		 require(base.size()<32768 && base.find('#') == std::string::npos && 
			 base.find_first_of("\r\n") == std::string::npos,"Invalid endpoint");
		 auto w=wide(base);
		 URL_COMPONENTS u{};

		 u.dwStructSize=sizeof(u);
		 u.dwHostNameLength = u.dwUserNameLength=u.dwPasswordLength=(DWORD)-1;
		 require(WinHttpCrackUrl(w.c_str(),0,0,&u)!=0 && (u.nScheme==INTERNET_SCHEME_HTTP ||
			 u.nScheme==INTERNET_SCHEME_HTTPS) && u.dwHostNameLength>0,"Expected http or https endpoint");
		 require(!u.dwUserNameLength && !u.dwPasswordLength, "Do not embed credentials in URL");
		 size_t q = base.find('?');
		 std::string out=base.substr(0,q),query;
		 if(q!=std::string::npos)
		 {
			 std::istringstream in(base.substr(q+1));
			 std::string part;
			 while(std::getline(in,part,'&'))
			 {
				 auto k=keyDecode(part.substr(0,part.find('=')));
				 if(!part.empty()&& p.find(k)== p.end())
				 {
					 if(!query.empty())
						 query += '&';
					 query += part;
				 }
			 }
		 }
		 for(auto& kv:p)
		 {
			 if(!query.empty())
				 query+='&';query+=kv.first+"="+encode(kv.second);
		 }
		 return out+"?"+query;
	}

	std::string mapUrl(const OGC_Connection*c,const OGC_WMSMap*r)
	{
		require(r && r->layers && *r->layers && r->crs && *r->crs, "Missing WMS layers or CRS");
		require(r->width && r->height && r->width <= 65535 && r->height <= 65535,"Invalid image dimensions");
		require(r->axis_order>=0 && r->axis_order<=2,"Invalid axis order");
		std::string v=value(r->version,"1.3.0");
		versionOK(v,"WMS");
		bool swap = r->axis_order==OGC_AXIS_YX || (r->axis_order==OGC_AXIS_AUTO && v=="1.3.0" && upper(r->crs)=="EPSG:4326");
		Params p={
			{"SERVICE","WMS"},
			{"VERSION",v},
			{"REQUEST","GetMap"},
			{"LAYERS",r->layers},
			{"STYLES",value(r->styles)},
			{"FORMAT",value(r->format,"image/png")},
			{"WIDTH",std::to_string(r->width)},
			{"HEIGHT",std::to_string(r->height)},
			{"TRANSPARENT",r->transparent?"TRUE":"FALSE"},
			{"BBOX",bbox(r->min_x,r->min_y,r->max_x,r->max_y,swap)}
		};

		p[v=="1.3.0"?"CRS":"SRS"]=r->crs;
		return url(c,p);
	}

	std::string featureUrl(const OGC_Connection*c,const OGC_WFSQuery*r)
	{
		require(r && r->type_names&&*r->type_names,"Missing WFS type names");
		std::string v = value(r->version,"2.0.0");
		versionOK(v,"WFS");require(v=="2.0.0" || r->start_index==0,"STARTINDEX requires WFS 2.0.0");
		Params p={
			{"SERVICE","WFS"},
			{"VERSION",v},
			{"REQUEST","GetFeature"},
			{v=="2.0.0"?"TYPENAMES":"TYPENAME",r->type_names}
		};

		if(r->srs_name&&*r->srs_name)
			p["SRSNAME"]=r->srs_name;
		if(r->output_format&&*r->output_format)
			p["OUTPUTFORMAT"]=r->output_format;
		if(r->count)
			p[v=="2.0.0"?"COUNT":"MAXFEATURES"]=std::to_string(r->count);
		if(v=="2.0.0")
			p["STARTINDEX"]=std::to_string(r->start_index);
		if(r->use_bbox)
		{
			require(r->axis_order==OGC_AXIS_XY || r->axis_order == OGC_AXIS_YX,"WFS BBOX requires explicit XY or YX axis order");
			p["BBOX"] = bbox(r->min_x,r->min_y,r->max_x,r->max_y,r->axis_order == OGC_AXIS_YX);
			if(r->srs_name && *r->srs_name) 
				p["BBOX"] += ","+std::string(r->srs_name);
		}
		return url(c,p);
	}

	std::string tileUrl(const OGC_Connection*c,const OGC_WMTSTile*r)
	{
		 require(r&&r->layer&&*r->layer&&r->style&&*r->style&&r->matrix_set&&*r->matrix_set&&r->matrix&&*r->matrix,"Missing WMTS identifiers");
		 return url(c, {
			 {"SERVICE","WMTS"},
			 {"VERSION","1.0.0"},
			 {"REQUEST","GetTile"},
			 {"LAYER",r->layer},
			 {"STYLE",r->style},
			 {"FORMAT",value(r->format,"image/png")},
			 {"TILEMATRIXSET",r->matrix_set},
			 {"TILEMATRIX",r->matrix},
			 {"TILEROW",std::to_string(r->row)},
			 {"TILECOL",std::to_string(r->column)}
		 });
	}

	int fail(OGC_Result*r,int code,const char* message)
	{
		strncpy_s(r->error, message, _TRUNCATE);
		return code;
	}

	void assign(OGC_Result*r,const unsigned char*d,size_t n)
	{
		auto p=std::make_unique<unsigned char[]>(n+1);
		if(n)
			memcpy(p.get(),d,n);
		p[n]=0;
		r->data=p.release();
		r->size=n;
	}

	void freeResult(OGC_Result*r)
	{
		if(r)
		{
			delete[] r->data;*r={};
		}
	}

	struct Handle 
	{
		HINTERNET h;
		explicit Handle(HINTERNET p):h(p){}
		~Handle()
		{
			if(h)
				WinHttpCloseHandle(h);
		}
		operator HINTERNET()const
		{
			return h;
		}
	};

	int networkError(OGC_Result*r)
	{
		r->native_error=GetLastError();
		return fail(r,OGC_NETWORK_ERROR,"WinHTTP request failed; see native_error");
	}

	bool exceptionXml(const std::vector<unsigned char>& data)
	{
		 if(data.empty())	return false;
		 IStream* stream=SHCreateMemStream(data.data(),static_cast<UINT>(data.size()));
		 if(!stream)	return false;

		 IXmlReader* reader=nullptr;
		 bool result=false;
		 if(SUCCEEDED(CreateXmlReader(__uuidof(IXmlReader),(void**)&reader,nullptr)))
		 {
			 reader->SetProperty(XmlReaderProperty_DtdProcessing,DtdProcessing_Prohibit);
			 reader->SetInput(stream);
			 XmlNodeType type;
		  while(
			  reader->Read(&type)==S_OK)
			  if(type==XmlNodeType_Element)
			  {
				  const wchar_t*name=nullptr;
				  if(SUCCEEDED(reader->GetLocalName(&name,nullptr)))
					  result=wcscmp(name,L"ExceptionReport")==0||wcscmp(name,L"ServiceExceptionReport")==0;
				  break;
			  }
		  reader->Release();
		 }
		 stream->Release();
		 
		 return result;

	}
	struct AsyncState 
	{
		 HANDLE done=CreateEvent(nullptr,FALSE,FALSE,nullptr);
		 HANDLE closed=CreateEvent(nullptr,TRUE,FALSE,nullptr);
		 DWORD error=0,received=0;
		 unsigned char buffer[16384]; // Remains alive until the request-close callback completes.
		 ~AsyncState()
		 {
			 if(done)
				 CloseHandle(done);
			 if(closed)
				 CloseHandle(closed);
		 }
	};

	void CALLBACK completed(HINTERNET,DWORD_PTR context,DWORD status,void* info,DWORD size)
	{
		 auto* state = reinterpret_cast<AsyncState*>(context);
		 if(!state)	return;

		 switch(status)
		 {
		 case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
			 SetEvent(state->closed);
			 break;
		 case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		  state->error=static_cast<WINHTTP_ASYNC_RESULT*>(info)->dwError;
		  SetEvent(state->done);
		  break;
		 case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
			 state->received=size;
			 SetEvent(state->done);
			 break;
		 case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
		 case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			 SetEvent(state->done);
			 break;
		 }
	}
	struct AsyncRequest 
	{
		 HINTERNET h;
		 AsyncState& state;
		 bool callback=false;
		 AsyncRequest(HINTERNET handle,AsyncState& s):h(handle),state(s){}
		 ~AsyncRequest()
		 {
			 if(h)
			 {
				 WinHttpCloseHandle(h);
				 if(callback)
					 WaitForSingleObject(state.closed,INFINITE);
				 }
			 }
			 operator HINTERNET()const{return h;
		 }
	};

	bool awaitRequest(AsyncState& state,DWORD timeout)
	{
		 DWORD rc=WaitForSingleObject(state.done,timeout);
		 if(rc!=WAIT_OBJECT_0)
		 {
			 SetLastError(rc==WAIT_TIMEOUT?ERROR_WINHTTP_TIMEOUT:ERROR_GEN_FAILURE);
			 return false;
		 }
		 if(state.error)
		 {
			 SetLastError(state.error);return false;
		 }
		 return true;
	}

	int fetch(const OGC_Connection*c, const std::string& address, OGC_Result*r)
	{
		 require(c->timeout_ms<=INT_MAX,"Timeout too large");
		 auto u=wide(address);
		 URL_COMPONENTS parts{};
		 parts.dwStructSize=sizeof(parts);
		 parts.dwHostNameLength=parts.dwUrlPathLength=parts.dwExtraInfoLength=(DWORD)-1;
		 require(WinHttpCrackUrl(u.c_str(),0,0,&parts)!=0,"Invalid URL");
		 std::wstring host(parts.lpszHostName,parts.dwHostNameLength),path(parts.lpszUrlPath,parts.dwUrlPathLength);

		 if(parts.dwExtraInfoLength)
			 path.append(parts.lpszExtraInfo,parts.dwExtraInfoLength);
		 if(path.empty())
			 path=L"/";
		 Handle session(WinHttpOpen(L"LibSrc-OGC/1.0",c->no_proxy?WINHTTP_ACCESS_TYPE_NO_PROXY:WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,WINHTTP_FLAG_ASYNC));if(!session.h)return networkError(r);
		 int timeout=c->timeout_ms?c->timeout_ms:30000;
		 if(!WinHttpSetTimeouts(session,timeout,timeout,timeout,timeout))
			 return networkError(r);
		 Handle conn(WinHttpConnect(session,host.c_str(),parts.nPort,0));
		 if(!conn.h)return networkError(r);
		 AsyncState async;require(async.done&&async.closed,"Cannot create request events");
		 AsyncRequest request(WinHttpOpenRequest(conn,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,parts.nScheme==INTERNET_SCHEME_HTTPS?WINHTTP_FLAG_SECURE:0),async);if(!request.h)return networkError(r);
		 // Apply timeouts to the request too: bound response headers as well as body reads.
		 if(!WinHttpSetTimeouts(request,timeout,timeout,timeout,timeout))
			 return networkError(r);
		 DWORD responseTimeout=static_cast<DWORD>(timeout);
		 if(!WinHttpSetOption(request,WINHTTP_OPTION_RECEIVE_RESPONSE_TIMEOUT,&responseTimeout,sizeof(responseTimeout)))
			 return networkError(r);
		 DWORD policy=WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
		 if(!WinHttpSetOption(request,WINHTTP_OPTION_REDIRECT_POLICY,&policy,sizeof(policy)))
			 return networkError(r);
		 DWORD decompression=WINHTTP_DECOMPRESSION_FLAG_GZIP|WINHTTP_DECOMPRESSION_FLAG_DEFLATE;
		 WinHttpSetOption(request,WINHTTP_OPTION_DECOMPRESSION,&decompression,sizeof(decompression));
		 DWORD_PTR context=reinterpret_cast<DWORD_PTR>(&async);
		 if(!WinHttpSetOption(request,WINHTTP_OPTION_CONTEXT_VALUE,&context,sizeof(context)))
			 return networkError(r);
		 if(WinHttpSetStatusCallback(request,completed,WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS|WINHTTP_CALLBACK_FLAG_HANDLES,0)==WINHTTP_INVALID_STATUS_CALLBACK)
			 return networkError(r);

		 request.callback=true;
		 std::wstring headers;
		 if(c->bearer_token&&*c->bearer_token)
		 {
			 std::string token=c->bearer_token;
			 require(token.find_first_of("\r\n") == std::string::npos, "Invalid bearer token");
			 headers=L"Authorization: Bearer "+wide(token)+L"\r\n";
		 }
		 if(!WinHttpSendRequest(request,headers.empty()?WINHTTP_NO_ADDITIONAL_HEADERS:headers.c_str(),(DWORD)headers.size(),
			 WINHTTP_NO_REQUEST_DATA,0,0,context))
			 return networkError(r);
		 if(!awaitRequest(async,timeout))
			 return networkError(r);
		 if(!WinHttpReceiveResponse(request,nullptr))
			 return networkError(r);
		 if(!awaitRequest(async,timeout))
			 return networkError(r);
		 DWORD status=0,bytes=sizeof(status);
		 if(!WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,
			 &status,&bytes,WINHTTP_NO_HEADER_INDEX))
			 return networkError(r);

		 r->http_status=status;
		 wchar_t mime[128]={};bytes=sizeof(mime);
		 if(WinHttpQueryHeaders(request,WINHTTP_QUERY_CONTENT_TYPE,WINHTTP_HEADER_NAME_BY_INDEX,mime,&bytes,WINHTTP_NO_HEADER_INDEX))
			 WideCharToMultiByte(CP_UTF8,0,mime,-1,r->content_type,sizeof(r->content_type),nullptr,nullptr);

		 std::vector<unsigned char> data;uint32_t limit = c->max_bytes ? c->max_bytes: 64U*1024*1024;
		 unsigned char* chunk=async.buffer;

		 for(;;)
		 {
			 if(!WinHttpReadData(request,chunk,sizeof(async.buffer),nullptr))
				return networkError(r);
			 if(!awaitRequest(async,timeout))
				 return networkError(r);DWORD got=async.received;
			 if(!got)	break;
			 if(data.size()+got>limit)
				 return fail(r,OGC_RESPONSE_TOO_LARGE,"Response exceeds max_bytes");

			 data.insert(data.end(),chunk,chunk+got);
		 }
		 assign(r,data.data(),data.size());
		 if(status<200 || status>=300)
			 return fail(r,OGC_HTTP_ERROR,"HTTP status is not successful; see http_status and response data");
		 if(exceptionXml(data))
			 return fail(r,OGC_SERVICE_ERROR,"OGC service exception; see response data");

		 return OGC_OK;
	}

	template<class F>int guard(OGC_Result*r,F fn)
	{
		if(!r)
			return OGC_INVALID_ARGUMENT;
		if(r->data)
			return fail(r,OGC_INVALID_ARGUMENT,"Free result before reusing it");*r={};
		try{
			return fn();
		}
		catch(const Invalid& e)
		{
			return fail(r,OGC_INVALID_ARGUMENT, e.what());
		}
		catch(...)
		{
			return fail(r,OGC_INTERNAL_ERROR,"Internal allocation or processing failure");
		}
	}

	int built(OGC_Result*r, const std::string& s)
	{
		assign(r,reinterpret_cast<const unsigned char*>(s.data()),s.size());
		strcpy_s(r->content_type,"text/plain; charset=utf-8");

		return OGC_OK;
	}

	int capabilities(const OGC_Connection*c,const char*version,OGC_Result*r,const char*service,const char*def)
	{
		return guard(r,[&]
			{
				std::string v = value(version,def);

				versionOK(v,service);
				Params p={
					{"SERVICE",service},
					{"REQUEST","GetCapabilities"}
				};

				p[(!strcmp(service,"WMS") || (!strcmp(service,"WFS")&&v=="1.0.0")) ? "VERSION":"ACCEPTVERSIONS"] = v;
				return fetch(c,url(c,p), r);
			});
	}
}

#if defined(OGC_BUILD_WMS)
extern "C" {
void __cdecl WMS_FreeResult(OGC_Result*r){freeResult(r);}
int __cdecl WMS_GetCapabilities(const OGC_Connection*c,const char*v,OGC_Result*r){return capabilities(c,v,r,"WMS","1.3.0");}
int __cdecl WMS_BuildMapUrl(const OGC_Connection*c,const OGC_WMSMap*q,OGC_Result*r){return guard(r,[&]{return built(r,mapUrl(c,q));});}
int __cdecl WMS_GetMap(const OGC_Connection*c,const OGC_WMSMap*q,OGC_Result*r){return guard(r,[&]{return fetch(c,mapUrl(c,q),r);});}
}
#elif defined(OGC_BUILD_WFS)
extern "C" {
void __cdecl WFS_FreeResult(OGC_Result*r){freeResult(r);}
int __cdecl WFS_GetCapabilities(const OGC_Connection*c,const char*v,OGC_Result*r){return capabilities(c,v,r,"WFS","2.0.0");}
int __cdecl WFS_BuildFeatureUrl(const OGC_Connection*c,const OGC_WFSQuery*q,OGC_Result*r){return guard(r,[&]{return built(r,featureUrl(c,q));});}
int __cdecl WFS_GetFeature(const OGC_Connection*c,const OGC_WFSQuery*q,OGC_Result*r){return guard(r,[&]{return fetch(c,featureUrl(c,q),r);});}
}
#elif defined(OGC_BUILD_WMTS)
extern "C" {
void __cdecl WMTS_FreeResult(OGC_Result*r){freeResult(r);}
int __cdecl WMTS_GetCapabilities(const OGC_Connection*c,const char*v,OGC_Result*r){return capabilities(c,v,r,"WMTS","1.0.0");}
int __cdecl WMTS_BuildTileUrl(const OGC_Connection*c,const OGC_WMTSTile*q,OGC_Result*r){return guard(r,[&]{return built(r,tileUrl(c,q));});}
int __cdecl WMTS_GetTile(const OGC_Connection*c,const OGC_WMTSTile*q,OGC_Result*r){return guard(r,[&]{return fetch(c,tileUrl(c,q),r);});}
}
#endif

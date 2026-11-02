#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* UTF-8 strings. Initialize all structs to zero. Calls are synchronous.
   Result must be zero initialized and freed by the SAME service's FreeResult.
   Do not reuse a live result without freeing it first. No STL/MFC across ABI. */
typedef struct OGC_Result 
{
    unsigned char* data;
    uint64_t size;
    uint32_t http_status;
    uint32_t native_error;
    char content_type[128];
    char error[512];
} OGC_Result;

enum OGC_Status 
{ 
    OGC_OK=0, 
    OGC_INVALID_ARGUMENT=1, 
    OGC_NETWORK_ERROR=2,
    OGC_HTTP_ERROR=3, 
    OGC_SERVICE_ERROR=4, 
    OGC_RESPONSE_TOO_LARGE=5, 
    OGC_INTERNAL_ERROR=6 
};

enum OGC_AxisOrder 
{ 
    OGC_AXIS_AUTO=0, 
    OGC_AXIS_XY=1, 
    OGC_AXIS_YX=2 
};

typedef struct OGC_Connection {
    const char* endpoint;
    const char* bearer_token; /* optional; never include CR/LF */
    uint32_t timeout_ms;     /* zero => 30000; per WinHTTP operation */
    uint32_t max_bytes;      /* zero => 64 MiB */
    int no_proxy;            /* zero uses Windows default proxy */
} OGC_Connection;

typedef struct OGC_WMSMap 
{
    const char* version;     /* NULL => 1.3.0; also supports 1.1.1 */
    const char* layers;
    const char* styles;      /* NULL => empty (default style) */
    const char* crs;         /* e.g. EPSG:4326, CRS:84, EPSG:3857 */
    const char* format;      /* NULL => image/png */
    double min_x,min_y,max_x,max_y; /* x/y order before axis conversion */
    uint32_t width,height;
    int transparent;
    int axis_order;          /* AUTO swaps EPSG:4326 for WMS 1.3.0 only */
} OGC_WMSMap;

typedef struct OGC_WFSQuery
{
    const char* version;     /* NULL => 2.0.0; also 1.0.0 and 1.1.0 */
    const char* type_names;
    const char* srs_name;    /* optional; server default if NULL */
    const char* output_format; /* optional: server default GML, or application/json */
    uint32_t count;          /* zero => server default */
    uint32_t start_index;    /* paging for WFS 2.0.0 only */
    int use_bbox;
    double min_x,min_y,max_x,max_y;
    int axis_order;          /* specify XY/YX explicitly when use_bbox=1 */
} OGC_WFSQuery;

typedef struct OGC_WMTSTile 
{
    const char* layer;
    const char* style;       /* required; read identifier from capabilities */
    const char* format;
    const char* matrix_set;
    const char* matrix;      /* identifier, not necessarily an integer zoom */
    uint64_t row,column;    /* server's TileMatrix row/column, not TMS indices */
} OGC_WMTSTile;


#ifdef OGC_BUILD_WMS
#define WMS_API __declspec(dllexport)
#else
#define WMS_API __declspec(dllimport)
#endif
#ifdef OGC_BUILD_WFS
#define WFS_API __declspec(dllexport)
#else
#define WFS_API __declspec(dllimport)
#endif
#ifdef OGC_BUILD_WMTS
#define WMTS_API __declspec(dllexport)
#else
#define WMTS_API __declspec(dllimport)
#endif
#define OGC_DECLARE_COMMON(P) \
 P##_API int __cdecl P##_GetCapabilities(const OGC_Connection*,const char* version,OGC_Result*); \
 P##_API void __cdecl P##_FreeResult(OGC_Result*);
OGC_DECLARE_COMMON(WMS)
OGC_DECLARE_COMMON(WFS)
OGC_DECLARE_COMMON(WMTS)
WMS_API int __cdecl WMS_BuildMapUrl(const OGC_Connection*,const OGC_WMSMap*,OGC_Result*);
WMS_API int __cdecl WMS_GetMap(const OGC_Connection*,const OGC_WMSMap*,OGC_Result*);
WFS_API int __cdecl WFS_BuildFeatureUrl(const OGC_Connection*,const OGC_WFSQuery*,OGC_Result*);
WFS_API int __cdecl WFS_GetFeature(const OGC_Connection*,const OGC_WFSQuery*,OGC_Result*);
WMTS_API int __cdecl WMTS_BuildTileUrl(const OGC_Connection*,const OGC_WMTSTile*,OGC_Result*);
WMTS_API int __cdecl WMTS_GetTile(const OGC_Connection*,const OGC_WMTSTile*,OGC_Result*);
#undef OGC_DECLARE_COMMON
#ifdef __cplusplus
}
#endif

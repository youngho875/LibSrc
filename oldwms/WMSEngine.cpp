#include "pch.h"
#include "WMSEngine.h"


// Curl Callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) 
{
	//std::vector<unsigned char>* buffer = (std::vector<unsigned char>*) userp;
	//size_t totalSize = size * nmemb;
	//buffer->insert(buffer->end(), (unsigned char*)contents, (unsigned char*)contents + totalSize);
	//return totalSize;

	size_t realsize = size * nmemb;
	MemBuffer* mem = (MemBuffer*)userp;
	size_t oldSize = mem->data.size();
	mem->data.resize(oldSize + realsize);
	memcpy(&(mem->data[oldSize]), contents, realsize);

	return realsize;
}

CWMSEngine::CWMSEngine(HWND hWnd) 
	: m_hWnd(hWnd), 
	m_Level(5), 
	m_bDragging(false), 
	m_centerLon(127.0), m_centerLat(37.0),
	m_startX(0), m_endX(0),
	m_startY(0), m_endY(0)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	mCoordMgr = CCoordMgr::GetInst();
	mCoord.initialize_adaptation_data();

	//UpdateResolution();

	if (::IsWindow(m_hWnd)) 
	{
		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		//CCoordMgr::GetInst()->InitDisplay(rect.right, rect.bottom);
		UpdateResolution();

		CalculateVisibleRange();
	}
}

CWMSEngine::~CWMSEngine() 
{ 
	ClearCache(); 
}


void CWMSEngine::UpdateResolution() 
{
	// EPSG:4326 Resolution
	m_resolution = 360.0 / (pow(2.0, m_Level + 1) * 256.0);
}


void CWMSEngine::MoveMap(int dx, int dy)
{
	m_centerLon -= (dx * m_resolution);
	m_centerLat += (dy * m_resolution);

	CalculateVisibleRange();
}

void CWMSEngine::CalculateVisibleRange()
{
	if (!::IsWindow(m_hWnd)) 
		return;

	int w = CCoordMgr::GetInst()->m_nScreenWidth;
	int h = CCoordMgr::GetInst()->m_nScreenHeight;


	double wmX, wmY, topLat, leftLon;
	double wMX, wMY, bottomLat, rightLon;

	mCoordMgr->ScreenXY2WorldXY(0, 0, &wmX, &wmY);
	mCoordMgr->ScreenXY2WorldXY(w, h, &wMX, &wMY);
	mCoord.x_y_to_degrees_lat_long(wmX, wmY, &topLat, &leftLon);
	mCoord.x_y_to_degrees_lat_long(wMX, wMY, &bottomLat, &rightLon);

	double nX = pow(2.0, m_Level + 1);
	double nY = pow(2.0, m_Level);

	double degPerTileX = 360.0 / nX;
	double degPerTileY = 180.0 / nY;

	// 2. 인덱스 계산 (정확한 위치를 위해 floor 사용)
	// 48도 이상에서 안 나오는 문제를 해결하기 위해 부동소수점 오차(0.000001) 반영
	m_startX = (int)floor((leftLon + 180.0) / degPerTileX);
	m_endX = (int)floor((rightLon + 180.0) / degPerTileX);
	m_startY = (int)floor((90.0 - topLat) / degPerTileY);
	m_endY = (int)floor((90.0 - bottomLat) / degPerTileY);

	// 3. 마진 추가 (화면 경계 타일 잘림 방지)
	m_startX -= 1; m_endX += 1;
	m_startY -= 1; m_endY += 1;

	// 4. [핵심] 상하 반전 및 유효 범위 강제 제한
	// 위도가 바뀌어 startY > endY가 된 경우 보정
	if (m_startY > m_endY) 
		std::swap(m_startY, m_endY);
	if (m_startX > m_endX) 
		std::swap(m_startX, m_endX);

	// 전체 타일 범위를 벗어나지 않도록 클램핑(Clamping)
	m_startX = max(0, m_startX);
	m_startY = max(0, m_startY);
	m_endX = min((int)nX - 1, m_endX);
	m_endY = min((int)nY - 1, m_endY);
	
	/*
	double unitX = 360.0 / nX;
	double unitY = 180.0 / nY;




	m_startX = (int)floor((leftLon + 180.0) / unitX) - 1;
	m_endX = (int)floor((rightLon + 180.0) / unitX) + 1;

	m_startY = (int)floor((90.0 - topLat) / unitY) - 1;
	m_endY = (int)floor((90.0 - bottomLat) / unitY) + 1;

/*
	// 경도 타일 시작/끝 (180 더해서 0~360 범위로 만든 뒤 비율 계산)
	m_startX = (int)((leftLon + 180.0) / 360.0 * nX) - 1;
	m_endX = (int)((rightLon + 180.0) / 360.0 * nX) + 1;

	// 위도 타일 시작/끝 (90에서 빼서 북극 0 기준 아래로 얼마나 내려왔는지 계산)
	// topLat이 43도라면 (90-43)/180 = 47/180 지점의 타일을 가져옴
	m_startY = (int)((90.0 - topLat) / 180.0 * nY) - 1;
	m_endY = (int)((90.0 - bottomLat) / 180.0 * nY) + 1;
*/

/*
	// 인덱스 범위 제한 (유효한 타일 번호 유지)
	// 인덱스 역전 방지
	if (m_startY > m_endY) 
		std::swap(m_startY, m_endY);

	// 범위 제한 (nX, nY를 명확히 정수로 변환 후 비교)
	int maxIdxX = (int)nX - 1;
	int maxIdxY = (int)nY - 1;

	m_startX = max(0, m_startX);
	m_startY = max(0, m_startY);
	m_endX = min(maxIdxX, m_endX);
	m_endY = min(maxIdxY, m_endY);
	m_startX = max(0, m_startX);
	m_startY = max(0, m_startY);
	m_endX = min((int)nX - 1, m_endX);
	m_endY = min((int)nY - 1, m_endY);
*/
}



void CWMSEngine::WMSDraw(CDC* pDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_hWnd)
		return;

	//픽셀 배율 설정(1도당 픽셀 수)
	double fS = 1.0 / m_resolution;


	CPen pen(PS_SOLID, 1, RGB(180, 180, 180));	// 테두리: 연한 회색
	CBrush brush(RGB(245, 245, 245));			// 배경: 아주 연한 회색
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(120, 120, 120));

	pDC->SetStretchBltMode(COLORONCOLOR);

	for (int y = m_startY; y <= m_endY; ++y) 
	{
		for (int x = m_startX; x <= m_endX; x++)
		{
			// 타일의 BBOX 정보 가져오기 (minLon, maxLat이 좌상단 기준점)
			double minLon, minLat, maxLon, maxLat;
			GetTileBBox(x, y, m_Level, minLon, minLat, maxLon, maxLat);

			// 선형 변환 대수: 위경도 차이를 WorldXY로 대입
			double dWorldX = minLon - m_centerLon;
			double dWorldY = maxLat - m_centerLat;

			long sx, sy;
			// CoordMgr.cpp의 함수를 사용하여 화면 좌표(sx, sy) 산출
			mCoordMgr->WorldXY2ScreenXY(dWorldX, dWorldY, &sx, &sy, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, fS);

			// 타일 크기 고정 (평면 투영)
			int nTileSize = 256;
			CRect rectTile((int)sx, (int)sy, (int)sx + nTileSize, (int)sy + nTileSize);

			

			// 타일 데이터 확인 및 출력
			TileData* pTile = GetOrFetchTile(x, y, m_Level);
			if (pTile && !pTile->image.IsNull())
			{
				//pDC->Rectangle(&rectTile);
				// 이미지가 있는 경우: StretchBlt 출력
				pTile->image.StretchBlt(pDC->GetSafeHdc(), rectTile.left, rectTile.top, rectTile.Width(), rectTile.Height(), 0, 0, 256, 256, SRCCOPY);
			}
			else
			{
				// 이미지가 없는 경우: Rectangle 함수로 영역 표시
				pDC->Rectangle(&rectTile);

				// 디버깅 정보 출력 (선택 사항)
				CString strInfo;
				strInfo.Format(_T("[%d, %d]"), x, y);
				pDC->DrawText(strInfo, &rectTile, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
	}
	//DrawGrid(pDC->GetSafeHdc());

	// GDI 객체 복원
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}


/*
void CWmsEngine::OnDraw(HDC hdc) {
	RECT rect; ::GetClientRect(m_hWnd, &rect);
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	HGDIOBJ hOld = SelectObject(hMemDC, hBitmap);

	// 배경 지우기
	FillRect(hMemDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

	for (int y = m_startY; y <= m_endY; ++y) {
		for (int x = m_startX; x <= m_endX; ++x) {     
			TileData* pTile = GetOrFetchTile(x, y, m_zoom);
			if (pTile) {
				long screenX, screenY;
				LatLonToScreen(pTile->minLon, pTile->maxLat, &screenX, &screenY);
				pTile->image.Draw(hMemDC, (int)screenX, (int)screenY, 256, 256);
			}
		}
	}

	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOld);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
}
*/
/*
void CWMSEngine::LatLonToScreen(double lon, double lat, long* nXS, long* nYS)
{
	double fS = 1.0 / m_resolution;

	CCoordMgr::GetInst()->WorldXY2ScreenXY(lon - m_centerLon, lat - m_centerLat, nXS, nYS,
		CCoordMgr::GetInst()->m_fCenterX, CCoordMgr::GetInst()->m_fCenterY, fS);
}

void CWMSEngine::ScreenToLatLon(int nXS, int nYS, double* outLon, double* outLat) 
{
	double fS = 1.0 / m_resolution;
	double dLon, dLat;
	CCoordMgr::GetInst()->ScreenXY2WorldXY(nXS, nYS, &dLon, &dLat,
		CCoordMgr::GetInst()->m_fCenterX, CCoordMgr::GetInst()->m_fCenterY, fS);
	*outLon = dLon + m_centerLon;
	*outLat = dLat + m_centerLat;
}
*/

TileData* CWMSEngine::GetOrFetchTile(int x, int y, int z) 
{
	std::string key = std::to_string(z) + "_" + std::to_string(x) + "_" + std::to_string(y);
	if (m_tileCache.count(key))
	{
		auto it = m_tileCache[key];
		m_usageList.erase(it.second);
		m_usageList.push_front(key);
		m_tileCache[key].second = m_usageList.begin();
	
		return it.first;

		// 사용 순서 갱신을 위해 리스트 위치 이동
		//TileData* pData = m_tileCache[key].first;
		//m_usageList.erase(m_tileCache[key].second);
		//m_usageList.push_front(key);
		//m_tileCache[key].second = m_usageList.begin();

		//return pData;

	}
	TileData* pNew = DownloadAndLoadMFC(x, y, z);
	if (pNew) 
		AddToCache(key, pNew);

	return pNew;
}

TileData* CWMSEngine::DownloadAndLoadMFC(int x, int y, int z) 
{
	// WMS URL 생성 부분 (사용자 서버 사양에 맞춤)
		// 1. WMS URL 생성
	CString url = MakeWmsUrl(x, y, z);

	// 2. curl을 이용해 메모리로 다운로드 (WriteMemCallback 사용)
	MemBuffer chunk;
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)CT2A(url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK || chunk.data.empty())
		return nullptr;

	// 3. GDAL 대신 Windows 전역 메모리(GlobalAlloc)와 IStream 사용
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, chunk.data.size());
	if (!hGlobal)
		return nullptr;

	void* pData = GlobalLock(hGlobal);
	memcpy(pData, chunk.data.data(), chunk.data.size());
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	TileData* pNewTile = nullptr;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		pNewTile = new TileData();
		pNewTile->x = x;
		pNewTile->y = y;
		pNewTile->z = z;

		// 4. CImage::Load를 통해 스트림에서 직접 이미지 로드 (PNG, JPG 자동 인식)
		HRESULT hr = pNewTile->image.Load(pStream);

		pStream->Release();

		if (FAILED(hr)) 
		{
			delete pNewTile;
			pNewTile = nullptr;
		}
		else 
		{
			// BBOX 계산 
			GetTileBBox(x, y, z, pNewTile->minLon, pNewTile->minLat, pNewTile->maxLon, pNewTile->maxLat);
		}
	}

	return pNewTile;
}



void CWMSEngine::GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat) 
{
	double nX = pow(2.0, z + 1);
	double nY = pow(2.0, z);
	minLon = (double)x / nX * 360.0 - 180.0;
	maxLon = (double)(x + 1) / nX * 360.0 - 180.0;
	maxLat = 90.0 - (double)y / nY * 180.0;
	minLat = 90.0 - (double)(y + 1) / nY * 180.0;
}


void CWMSEngine::ClearCache() 
{
	for (auto& it : m_tileCache) 
	{ 
		it.second.first->image.Destroy(); 
		delete it.second.first; 
	}
	m_tileCache.clear(); m_usageList.clear();
}


CString CWMSEngine::MakeWmsUrl(int x, int y, int z)
{
	double minLon, minLat, maxLon, maxLat;
	GetTileBBox(x, y, z, minLon, minLat, maxLon, maxLat);

	CString baseUrl = _T("http://10.240.33.120/cgi-bin/mapserv.exe");
	CString mapFile = _T("/ms4w/apps/local-demo/ne1_hr.map"); // _T("/ms4w/apps/local-demo/height.map");
	CString url = _T("");

	// WMS 1.1.1 기준 (서버 설정에 따라 레이어 이름 등을 수정하세요)
	//url.Format(_T("%s?map=%s&SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap")
	//	_T("&STYLES&LAYERS=NE1_HR&CRS=EPSG:4326&WIDTH=256&HEIGHT=256&FORMAT=image/png")
	//	_T("&BBOX=%.8f,%.8f,%.8f,%.8f"),
	//	baseUrl, mapFile, minLon, minLat, maxLon, maxLat);

	// WMS 1.3.0 기준 (서버 설정에 따라 레이어 이름 등을 수정하세요)
	url.Format(_T("%s?map=%s&SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap")
		_T("&STYLES&LAYERS=NE1_HR&CRS=EPSG:4326&WIDTH=256&HEIGHT=256&FORMAT=image/png")
		_T("&BBOX=%.8f,%.8f,%.8f,%.8f"),
		baseUrl, mapFile, minLat, minLon, maxLat, maxLon);

	return url;
}


BOOL CWMSEngine::DownloadTile(int x, int y, int z, const CString& savePath)
{
	CURL* curl = curl_easy_init();
	if (!curl) return false;

	CString url = MakeWmsUrl(x, y, z);
	FILE* fp = _tfopen(savePath, _T("wb"));
	if (!fp) return false;

	curl_easy_setopt(curl, CURLOPT_URL, (const char*)CT2A(url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL); // 기본 쓰기 함수 사용
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

	CURLcode res = curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	return (res == CURLE_OK);
}


void CWMSEngine::AddToCache(const std::string& key, TileData* pTile) 
{
	// 1. 이미 존재하는 경우: 위치만 최신으로 갱신
	if (m_tileCache.count(key))
	{
		m_usageList.erase(m_tileCache[key].second);
	}
	// 2. 새로 추가하는 경우: 용량 확인
	else
	{
		if (m_tileCache.size() >= MAX_CACHE_SIZE)
		{
			RemoveOldestTile();
		}
	}

	// 리스트의 맨 앞에 추가 (최신 사용)
	m_usageList.push_front(key);
	// 맵에 데이터와 리스트 위치(iterator) 저장
	m_tileCache[key] = { pTile, m_usageList.begin() };
}

// 오래된 타일 삭제
void CWMSEngine::RemoveOldestTile()
{
	if (m_usageList.empty()) return;

	// 1. 가장 뒤에 있는(가장 오래된) 키 가져오기
	std::string oldestKey = m_usageList.back();

	// 2. 실제 데이터 메모리 해제
	TileData* pTile = m_tileCache[oldestKey].first;
	if (pTile)
	{
		pTile->image.Destroy(); // CImage 리소스 해제
		delete pTile;
	}

	// 3. 관리 목록에서 삭제
	m_tileCache.erase(oldestKey);
	m_usageList.pop_back();
}

BOOL CWMSEngine::LoadTileGDAL(const CString& strPath, TileData& tile)
{
	GDALDataset* poDS = (GDALDataset*)GDALOpen(CT2A(strPath), GA_ReadOnly);
	if (!poDS)
		return false;

	int w = poDS->GetRasterXSize();
	int h = poDS->GetRasterYSize();

	// 1. CImage 초기화 (32비트 BGRA)
	tile.image.Destroy();
	tile.image.Create(w, h, 32);

	// 2. GDAL 데이터를 CImage의 픽셀 버퍼로 직접 읽기
	// CImage는 위아래가 뒤집힌 DIB 구조를 가질 수 있으므로 Pitch 계산이 중요합니다.
	void* pDest = tile.image.GetBits();
	int nPitch = tile.image.GetPitch();

	poDS->RasterIO(GF_Read, 0, 0, w, h, pDest, w, h, GDT_Byte, 4, nullptr, 4, nPitch, 1);

	GDALClose(poDS);

	return true;
}


double CWMSEngine::GetGridInterval() 
{
	if (m_Level <= 5) 
		return 10.0;       // 저배율: 10도 간격
	if (m_Level <= 8)
		return 1.0;        // 중배율: 1도 간격
	if (m_Level <= 11)
		return 0.5;       // 고배율: 0.5도 간격
	if (m_Level <= 14)
		return 0.1;       // 초고배율: 0.1도 간격

	return 0.01;          // 그 이상
}

void CWMSEngine::DrawGrid(HDC hdc)
{
	RECT rect; ::GetClientRect(m_hWnd, &rect);
	double interval = GetGridInterval();

	// 1. 현재 화면에 보이는 위경도 범위 계산
	double leftLon, topLat, rightLon, bottomLat;
	double GeoLeft, GeoRight, GeoTop, GeoBottom;

	mCoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &leftLon, &topLat);
	mCoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &rightLon, &bottomLat);
	mCoord.x_y_to_degrees_lat_long(leftLon, topLat, &GeoTop, &GeoLeft);
	mCoord.x_y_to_degrees_lat_long(rightLon, bottomLat, &GeoBottom, &GeoRight);

	//ScreenToLatLon(0, 0, &leftLon, &topLat);
	//ScreenToLatLon(rect.right, rect.bottom, &rightLon, &bottomLat);

	// 2. 그리드 선 스타일 설정 (연한 회색 점선 등)
	HPEN hPen = CreatePen(PS_DOT, 1, RGB(180, 180, 180));
	HGDIOBJ hOld = SelectObject(hdc, hPen);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(100, 100, 100));

	// 3. 경도선(Vertical Lines) 그리기
	double startLon = floor(GeoLeft / interval) * interval;
	for (double lon = startLon; lon <= GeoRight; lon += interval)
	{
		//long sx, sy_dummy, sy;
		double x_coordinate, y_coordinate;
		double sx, sy;
		mCoord.degrees_lat_long_to_x_y(GeoTop, lon, &x_coordinate, &y_coordinate);
		mCoordMgr->WorldXY2ScreenXY(x_coordinate, y_coordinate, &sx, &sy);


		//LatLonToScreen(lon, m_centerLat, &sx, &sy_dummy); // Y는 무시

		MoveToEx(hdc, (int)sx, 0, NULL);
		LineTo(hdc, (int)sx, rect.bottom);

		// 경도 값 텍스트 표시
		CString strLon; strLon.Format(_T("%.2f"), lon);
		TextOut(hdc, (int)sx + 5, 5, strLon, strLon.GetLength());
	}

	// 4. 위도선(Horizontal Lines) 그리기
	double startLat = floor(GeoBottom / interval) * interval;
	for (double lat = startLat; lat <= GeoTop; lat += interval)
	{
		double x_coordinate, y_coordinate;
		double sx, sy;
		mCoord.degrees_lat_long_to_x_y(lat, GeoRight, &x_coordinate, &y_coordinate);
		mCoordMgr->WorldXY2ScreenXY(x_coordinate, y_coordinate, &sx, &sy);
		//long sx_dummy, sy;
		//LatLonToScreen(m_centerLon, lat, &sx_dummy, &sy); // X는 무시

		MoveToEx(hdc, 0, (int)sy, NULL);
		LineTo(hdc, rect.right, (int)sy);

		// 위도 값 텍스트 표시
		CString strLat; strLat.Format(_T("%.2f"), lat);
		TextOut(hdc, 5, (int)sy + 5, strLat, strLat.GetLength());
	}

/*
	// 3. 경도선(Vertical Lines) 그리기
	double startLon = floor(leftLon / interval) * interval;
	for (double lon = startLon; lon <= rightLon; lon += interval)
	{
		long sx, sy_dummy;
		LatLonToScreen(lon, m_centerLat, &sx, &sy_dummy); // Y는 무시

		MoveToEx(hdc, (int)sx, 0, NULL);
		LineTo(hdc, (int)sx, rect.bottom);

		// 경도 값 텍스트 표시
		CString strLon; strLon.Format(_T("%.2f"), lon);
		TextOut(hdc, (int)sx + 5, 5, strLon, strLon.GetLength());
	}

	// 4. 위도선(Horizontal Lines) 그리기
	double startLat = floor(bottomLat / interval) * interval;
	for (double lat = startLat; lat <= topLat; lat += interval)
	{
		long sx_dummy, sy;
		LatLonToScreen(m_centerLon, lat, &sx_dummy, &sy); // X는 무시

		MoveToEx(hdc, 0, (int)sy, NULL);
		LineTo(hdc, rect.right, (int)sy);

		// 위도 값 텍스트 표시
		CString strLat; strLat.Format(_T("%.2f"), lat);
		TextOut(hdc, 5, (int)sy + 5, strLat, strLat.GetLength());
	}
*/
}

double CWMSEngine::GetResolution() const 
{ 
	return m_resolution; 
}


double CWMSEngine::GetCenterLon() const 
{ 
	return m_centerLon; 
}


double CWMSEngine::GetCenterLat() const 
{ 
	return m_centerLat; 
}


int CWMSEngine::GetZoom() const 

{ return m_Level; 
}


void CWMSEngine::SetCenter(double lon, double lat)
{
	m_centerLon = lon; 
	m_centerLat = lat;

	CalculateVisibleRange(); // 중심 바뀌면 범위 재계산
}


void CWMSEngine::SetZoom(int zoom)
{
	m_Level = zoom;
	UpdateResolution();
	CalculateVisibleRange();
}


void CWMSEngine::OnSize(int w, int h)
{
	if (w <= 0 || h <= 0) return;

	// 1. CoordMgr의 화면 크기 및 중심점 정보 갱신
	// CoordMgr.cpp의 로직을 참고하여 중심을 절반으로 설정
	mCoordMgr->m_nScreenWidth = w;
	mCoordMgr->m_nScreenHeight = h;
	mCoordMgr->m_fCenterX = (double)w / 2.0;
	mCoordMgr->m_fCenterY = (double)h / 2.0;

	// 2. 가시 범위 타일 인덱스(m_startX ~ m_endY) 재계산
	// 창이 커지면 그려야 할 타일의 개수가 늘어나기 때문입니다.
	CalculateVisibleRange();
}


/*
void CWMSEngine::OnMouseMove(POINT pt) {
	if (m_bDragging) {
		m_centerLon -= (pt.x - m_lastMousePos.x) * m_resolution;
		m_centerLat += (pt.y - m_lastMousePos.y) * m_resolution;
		m_lastMousePos = pt;
		CalculateVisibleRange();
		::InvalidateRect(m_hWnd, NULL, FALSE);
	}
}

void CWMSEngine::OnLButtonDown(POINT pt) { m_bDragging = true; m_lastMousePos = pt; SetCapture(m_hWnd); }
void CWMSEngine::OnLButtonUp(POINT pt) { m_bDragging = false; ReleaseCapture(); }

void CWMSEngine::ClearCache() {
	for (auto& it : m_tileCache) { it.second.first->image.Destroy(); delete it.second.first; }
	m_tileCache.clear(); m_usageList.clear();
}

void CWMSEngine::AddToCache(const std::string& key, TileData* pTile) {
	if (m_tileCache.size() >= MAX_CACHE_SIZE) {
		std::string oldest = m_usageList.back();
		m_tileCache[oldest].first->image.Destroy();
		delete m_tileCache[oldest].first;
		m_tileCache.erase(oldest); m_usageList.pop_back();
	}
	m_usageList.push_front(key);
	m_tileCache[key] = { pTile, m_usageList.begin() };
}

/*
CMapEngine::CMapEngine()
	: //m_zoom(2), 
	//m_centerLon(127.0),
	//m_centerLat(37.5),
	m_startX(0), m_endX(0),
	m_startY(0), m_endY(0), m_hWnd(NULL)
{
	// 해상도 초기화 (EPSG:4326 기준 줌 10의 예시값)            
	//m_resolution = 360.0 / (256.0 * pow(2.0, m_zoom + 1));

	//m_bDragging = FALSE;      // 드래그 중인지 여부
	//m_lastMousePos = 0;   // 이전 마우스 위치 (픽셀)

	//OnMoveToTarget();
}


CMapEngine::~CMapEngine()
{
	ClearCache();
}


void CMapEngine::ClearCache()
{
	// 1. unordered_map을 순회하며 동적 할당된 TileData 메모리 해제
	for (auto& pair : m_tileCache)
	{
		// pair.second는 std::pair<TileData*, std::list<std::string>::iterator> 구조임
		TileData* pTile = pair.second.first;

		if (pTile)
		{
			// CImage 리소스 해제 (GDI 객체 반환)
			pTile->image.Destroy();

			// TileData 객체 메모리 해제
			delete pTile;
		}
	}

	// 2. 컨테이너 내부 데이터 완전히 삭제
	m_tileCache.clear();   // 해시맵 비우기
	m_usageList.clear();   // LRU 순서 리스트 비우기
}


void CMapEngine::CalculateVisibleRange(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	// 화면의 좌상단과 우하단의 위경도 계산
	double leftLon = m_centerLon - ((rect.right - rect.left) / 2.0) * m_resolution;
	double topLat = m_centerLat + ((rect.bottom -rect.top) / 2.0) * m_resolution;
	double rightLon = m_centerLon + ((rect.right - rect.left) / 2.0) * m_resolution;
	double bottomLat = m_centerLat - ((rect.bottom - rect.top) / 2.0) * m_resolution;

	// 위경도를 타일 인덱스(X, Y)로 변환
	double nX = pow(2.0, m_zoom + 1);
	double nY = pow(2.0, m_zoom);

	m_startX = static_cast<int>((leftLon + 180.0) / 360.0 * nX);
	m_endX = static_cast<int>((rightLon + 180.0) / 360.0 * nX);
	m_startY = static_cast<int>((90.0 - topLat) / 180.0 * nY);
	m_endY = static_cast<int>((90.0 - bottomLat) / 180.0 * nY);

	// 인덱스 범위 제한 (지구 범위를 벗어나지 않게)
	m_startX = max(0, m_startX);
	m_startY = max(0, m_startY);
	m_endX = min((int)nX - 1, m_endX);
	m_endY = min((int)nY - 1, m_endY);
}


BOOL CMapEngine::LoadTileGDAL(const CString& strPath, TileData& tile)
{
	GDALDataset* poDS = (GDALDataset*)GDALOpen(CT2A(strPath), GA_ReadOnly);
	if (!poDS)
		return false;

	int w = poDS->GetRasterXSize();
	int h = poDS->GetRasterYSize();

	// 1. CImage 초기화 (32비트 BGRA)
	tile.image.Destroy();
	tile.image.Create(w, h, 32);

	// 2. GDAL 데이터를 CImage의 픽셀 버퍼로 직접 읽기
	// CImage는 위아래가 뒤집힌 DIB 구조를 가질 수 있으므로 Pitch 계산이 중요합니다.
	void* pDest = tile.image.GetBits();
	int nPitch = tile.image.GetPitch();

	poDS->RasterIO(GF_Read, 0, 0, w, h, pDest, w, h, GDT_Byte, 4, nullptr, 4, nPitch, 1);

	GDALClose(poDS);

	return true;
}


// MFC 클래스 멤버 함수 또는 유틸리티 함수로 선언
void CMapEngine::GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat)
{
	// 줌 레벨 z에서의 타일 분할 수
	// 줌 0: 가로 2개, 세로 1개 (전 세계)
	// 줌 1: 가로 4개, 세로 2개
	double nX = pow(2.0, z + 1);
	double nY = pow(2.0, z);

	// 타일 한 칸의 위경도 크기
	double dLon = 360.0 / nX;
	double dLat = 180.0 / nY;

	// BBOX 계산
	minLon = -180.0 + (x * dLon);
	maxLon = minLon + dLon;

	// Y축은 북쪽(+90)에서 남쪽(-90)으로 내려가는 방향
	maxLat = 90.0 - (y * dLat);
	minLat = maxLat - dLat;
}


// X 인덱스로부터 해당 타일의 왼쪽 경도(Longitude) 계산
double CMapEngine::GetLonFromX(int x, int z)
{
	// 줌 z에서 가로 타일 개수 (보통 2^(z+1))
	double nX = pow(2.0, z + 1);

	// 전체 360도를 타일 개수로 나눈 뒤, 시작점 -180도를 더함
	return (x / nX) * 360.0 - 180.0;
}

// Y 인덱스로부터 해당 타일의 위쪽 위도(Latitude) 계산
double CMapEngine::GetLatFromY(int y, int z)
{
	// 줌 z에서 세로 타일 개수 (보통 2^z)
	double nY = pow(2.0, z);

	// 전체 180도를 타일 개수로 나눈 뒤, 시작점 +90도에서 뺌
	// (Y 인덱스는 북쪽에서 남쪽으로 증가하기 때문)
	return 90.0 - (y / nY) * 180.0;
}


void CMapEngine::GetTileBBox2(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat)
{
	minLon = GetLonFromX(x, z);
	maxLon = GetLonFromX(x + 1, z); // 다음 타일의 시작점 = 현재 타일의 끝점

	maxLat = GetLatFromY(y, z);
	minLat = GetLatFromY(y + 1, z); // 다음 타일의 위쪽 위도 = 현재 타일의 아래쪽 위도
}


CString CMapEngine::MakeWmsUrl(int x, int y, int z)
{
	double minLon, minLat, maxLon, maxLat;
	GetTileBBox2(x, y, z, minLon, minLat, maxLon, maxLat);

	CString baseUrl = _T("http://10.240.33.120/cgi-bin/mapserv.exe");
	CString mapFile = _T("/ms4w/apps/local-demo/ne1_hr.map"); // _T("/ms4w/apps/local-demo/height.map");
	CString url = _T("");

	// WMS 1.3.0 기준 (서버 설정에 따라 레이어 이름 등을 수정하세요)
	url.Format(_T("%s?map=%s&SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap")
		_T("&STYLES&LAYERS=NE1_HR&CRS=EPSG:4326&WIDTH=256&HEIGHT=256&FORMAT=image/png")
		_T("&BBOX=%.8f,%.8f,%.8f,%.8f"),
		baseUrl, mapFile, minLon, minLat, maxLon, maxLat);

	return url;
}


BOOL CMapEngine::DownloadTile(int x, int y, int z, const CString& savePath)
{
	CURL* curl = curl_easy_init();
	if (!curl) return false;

	CString url = MakeWmsUrl(x, y, z);
	FILE* fp = _tfopen(savePath, _T("wb"));
	if (!fp) return false;

	curl_easy_setopt(curl, CURLOPT_URL, (const char*)CT2A(url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL); // 기본 쓰기 함수 사용
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

	CURLcode res = curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	return (res == CURLE_OK);
}



// 통합 타일 획득 함수
//TileData* CMapEngine::GetOrFetchTile_directory(int x, int y, int z)
//{
//	CString strKey;
//	strKey.Format(_T("%d_%d_%d"), z, x, y);
//	std::string key = CT2A(strKey);
//
//	// 1단계: 메모리 캐시 확인
//	if (m_tileCache.count(key))
//		return m_tileCache[key];
//
//	// 2단계: 디스크 파일 확인
//	CString strPath;
//	strPath.Format(_T("tiles\\%d\\%d\\%d.png"), z, x, y);
//
//	if (GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES)
//	{
//		// 3단계: 디스크에 없으면 폴더 생성 후 WMS 다운로드
//		if (CreateDirectoryRecursive(strPath))
//		{
//			if (!DownloadTile(x, y, z, strPath))
//				return nullptr;
//		}
//		else
//			return nullptr;
//	}
//
//	// 4단계: 디스크에서 로드하여 메모리 캐시에 등록
//	return LoadTileFromDisk(z, x, y);
//}



TileData* CMapEngine::GetOrFetchTile(int x, int y, int z)
{
	CString strKey;
	strKey.Format(_T("%d_%d_%d"), z, x, y);
	std::string key = CT2A(strKey);

	// 1. 캐시 히트(Hit)
	if (m_tileCache.count(key))
	{
		// 사용 순서 갱신을 위해 리스트 위치 이동
		TileData* pData = m_tileCache[key].first;
		m_usageList.erase(m_tileCache[key].second);
		m_usageList.push_front(key);
		m_tileCache[key].second = m_usageList.begin();

		return pData;
	}

	// 2. 캐시 미스(Miss) -> 서버 다운로드 및 메모리 로드 gdal을 이용하는 방법
	//TileData* pNewTile = DownloadAndLoadMemGDAL(x, y, z);
	//if (pNewTile)
	//{
	//	AddToCache(key, pNewTile); // LRU 규칙에 따라 추가
	//}

	TileData* pNewTile = DownloadAndLoadMFC(x, y, z);
	if (pNewTile)
	{
		AddToCache(key, pNewTile); // LRU 규칙에 따라 추가
	}

	return pNewTile;
}



//TileData* CMapEngine::GetOrFetchTile(int x, int y, int z)
//{
//	CString strKey;
//	strKey.Format(_T("%d_%d_%d"), z, x, y);
//	std::string key = CT2A(strKey);
//
//	// 1. 메모리 캐시 확인 (이미 로드된 건 그대로 사용)
//	if (m_tileCache.count(key)) return m_tileCache[key];
//
//	// 2. 캐시에 없으면 바로 서버에서 메모리로 다운로드 후 GDAL 로드
//	TileData* pNewTile = DownloadAndLoadMemGDAL(x, y, z);
//
//	if (pNewTile) {
//		m_tileCache[key] = pNewTile;
//	}
//
//	return pNewTile;
//}
//
//
//TileData* CMapEngine::GetOrFetchTile2(int x, int y, int z)
//{
//	CString strKey;
//	strKey.Format(_T("%d_%d_%d"), z, x, y);
//	std::string key = CT2A(strKey);
//
//	// 1. 메모리 캐시 확인
//	if (m_tileCache.count(key)) return m_tileCache[key];
//
//	// 2. 디스크 확인
//	CString strPath;
//	strPath.Format(_T("tiles\\%d\\%d\\%d.png"), z, x, y);
//
//	if (GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES)
//	{
//		// 3. 디스크에 없으면 WMS 다운로드
//		CreateDirectoryRecursive(strPath); // 폴더 생성 로직 필요
//		if (!DownloadTile(x, y, z, strPath)) return nullptr;
//	}
//
//	// 4. GDAL로 로드하여 캐시에 등록
//	return LoadTileFromDisk(z, x, y);
//}



// curl write callback
size_t CMapEngine::WriteMemCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	MemBuffer* mem = (MemBuffer*)userp;
	size_t oldSize = mem->data.size();
	mem->data.resize(oldSize + realsize);
	memcpy(&(mem->data[oldSize]), contents, realsize);

	return realsize;
}


// 메모리 로드
TileData* CMapEngine::DownloadAndLoadMemGDAL(int x, int y, int z)
{
	// 1. WMS URL 생성
	CString url = MakeWmsUrl(x, y, z);

	// 2. curl을 이용해 메모리로 다운로드
	MemBuffer chunk;
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)CT2A(url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK || chunk.data.empty())
		return nullptr;

	// 3. GDAL 가상 파일 시스템(/vsimem/)에 메모리 데이터 등록
	std::string vsiPath = "/vsimem/tile_" + std::to_string(z) + "_" + std::to_string(x) + "_" + std::to_string(y) + ".png";
	//std::string vsiPath = "/vsimem/tile_temp.png";
	VSIFCloseL(VSIFileFromMemBuffer(vsiPath.c_str(), chunk.data.data(), chunk.data.size(), FALSE));

	// 4. GDAL로 가상 파일 열기
	GDALDataset* poDS = (GDALDataset*)GDALOpen(vsiPath.c_str(), GA_ReadOnly);
	if (!poDS)
		return nullptr;

	int w = poDS->GetRasterXSize();
	int h = poDS->GetRasterYSize();

	TileData* pNewTile = new TileData();
	pNewTile->image.Create(w, h, 32);

	// CImage 버퍼에 직접 픽셀 데이터 읽기
	poDS->RasterIO(GF_Read, 0, 0, w, h, pNewTile->image.GetBits(), w, h, GDT_Byte, 4, nullptr, 4, pNewTile->image.GetPitch(), 1);

	GDALClose(poDS);
	VSIUnlink(vsiPath.c_str()); // 가상 파일 해제

	// BBOX 계산
	GetTileBBox(x, y, z, pNewTile->minLon, pNewTile->minLat, pNewTile->maxLon, pNewTile->maxLat);

	return pNewTile;
}


// 디스크에 저장된 이미지 로드
TileData* CMapEngine::DownloadAndLoadMFC(int x, int y, int z)
{
	// 1. WMS URL 생성
	CString url = MakeWmsUrl(x, y, z);

	// 2. curl을 이용해 메모리로 다운로드 (WriteMemCallback 사용)
	MemBuffer chunk;
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, (const char*)CT2A(url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK || chunk.data.empty())
		return nullptr;

	// 3. GDAL 대신 Windows 전역 메모리(GlobalAlloc)와 IStream 사용
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, chunk.data.size());
	if (!hGlobal)
		return nullptr;

	void* pData = GlobalLock(hGlobal);
	memcpy(pData, chunk.data.data(), chunk.data.size());
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	TileData* pNewTile = nullptr;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		pNewTile = new TileData();
		pNewTile->x = x;
		pNewTile->y = y;
		pNewTile->z = z;

		// 4. CImage::Load를 통해 스트림에서 직접 이미지 로드 (PNG, JPG 자동 인식)
		HRESULT hr = pNewTile->image.Load(pStream);

		pStream->Release();

		if (FAILED(hr)) {
			delete pNewTile;
			pNewTile = nullptr;
		}
		else {
			// BBOX 계산 
			GetTileBBox(x, y, z, pNewTile->minLon, pNewTile->minLat, pNewTile->maxLon, pNewTile->maxLat);
		}
	}

	return pNewTile;
}


BOOL CMapEngine::CreateDirectoryRecursive(const CString& strFilePath)
{
	// 파일 경로에서 파일명을 제외한 폴더 경로만 추출
	int nPos = strFilePath.ReverseFind('\\');
	if (nPos == -1)
		return false;

	CString strDir = strFilePath.Left(nPos);

	// 하위 폴더를 포함하여 경로 생성
	// ERROR_SUCCESS(0) 또는 ERROR_ALREADY_EXISTS는 성공으로 간주
	int nRet = SHCreateDirectoryEx(NULL, strDir, NULL);

	return (nRet == ERROR_SUCCESS || nRet == ERROR_ALREADY_EXISTS || nRet == ERROR_FILE_EXISTS);
}


//TileData* CMapEngine::LoadTileFromDisk(int z, int x, int y)
//{
//	// 1. 파일 경로 구성
//	CString strPath;
//	strPath.Format(_T("tiles\\%d\\%d\\%d.png"), z, x, y);
//
//	// 2. GDAL로 파일 열기
//	// MFC 유니코드 환경일 경우 CT2A를 사용하여 ANSI 문자열로 변환
//	GDALDataset* poDS = (GDALDataset*)GDALOpen(CT2A(strPath), GA_ReadOnly);
//	if (!poDS)
//		return nullptr;
//
//	int nWidth = poDS->GetRasterXSize();
//	int nHeight = poDS->GetRasterYSize();
//
//	// 3. TileData 객체 생성 및 CImage 초기화
//	TileData* pNewTile = new TileData();
//	pNewTile->x = x;
//	pNewTile->y = y;
//	pNewTile->z = z;
//
//	// 32비트(RGBA) 이미지 생성
//	pNewTile->image.Create(nWidth, nHeight, 32);
//
//	// 4. GDAL 데이터를 CImage 버퍼로 직접 읽기
//	void* pDestBits = pNewTile->image.GetBits();
//	int nPitch = pNewTile->image.GetPitch(); // 메모리 정렬을 고려한 줄 간격
//
//	// GDAL RasterIO: BGRA 순서로 읽기 (Windows 비트맵 표준)
//	CPLErr err = poDS->RasterIO(GF_Read, 0, 0, nWidth, nHeight,
//		pDestBits, nWidth, nHeight, GDT_Byte,
//		4, nullptr, 4, nPitch, 1);
//
//	GDALClose(poDS);
//
//	if (err != CE_None) {
//		delete pNewTile;
//		return nullptr;
//	}
//
//	// 5. 타일의 지리적 영역(BBOX) 계산 및 설정
//	// 앞서 구현한 GetTileBBox4326 함수를 호출하여 좌표 설정
//	GetTileBBox(x, y, z, pNewTile->minLon, pNewTile->minLat, pNewTile->maxLon, pNewTile->maxLat);
//
//	// 6. 메모리 캐시(m_tileCache)에 등록
//	CString strKey;
//	strKey.Format(_T("%d_%d_%d"), z, x, y);
//	std::string key = CT2A(strKey);
//
//	m_tileCache[key] = pNewTile;
//
//	return pNewTile;
//}



void CMapEngine::AddToCache(const std::string& key, TileData* pTile)
{
	// 1. 이미 존재하는 경우: 위치만 최신으로 갱신
	if (m_tileCache.count(key))
	{
		m_usageList.erase(m_tileCache[key].second);
	}
	// 2. 새로 추가하는 경우: 용량 확인
	else
	{
		if (m_tileCache.size() >= MAX_CACHE_SIZE)
		{
			RemoveOldestTile();
		}
	}

	// 리스트의 맨 앞에 추가 (최신 사용)
	m_usageList.push_front(key);
	// 맵에 데이터와 리스트 위치(iterator) 저장
	m_tileCache[key] = { pTile, m_usageList.begin() };
}


// 오래된 타일 삭제
void CMapEngine::RemoveOldestTile()
{
	if (m_usageList.empty()) return;

	// 1. 가장 뒤에 있는(가장 오래된) 키 가져오기
	std::string oldestKey = m_usageList.back();

	// 2. 실제 데이터 메모리 해제
	TileData* pTile = m_tileCache[oldestKey].first;
	if (pTile)
	{
		pTile->image.Destroy(); // CImage 리소스 해제
		delete pTile;
	}

	// 3. 관리 목록에서 삭제
	m_tileCache.erase(oldestKey);
	m_usageList.pop_back();
}


void CMapEngine::UpdateResolution()
{
	// EPSG:4326 기준: 줌 0일 때 가로 2개 타일(512픽셀)이 360도 전체를 커버
	// 공식: 360 / (256 * 2^(zoom + 1))
	m_resolution = 360.0 / (256.0 * pow(2.0, m_zoom + 1));
}


// 픽셀 좌표를 위경도로 변환하는 함수
void CMapEngine::PixelToLatLon(HWND hWnd, POINT pt, double& lon, double& lat)
{
	RECT rect;
	GetClientRect(hWnd, &rect);


	// 1. 화면의 중심 픽셀 좌표 계산
	double centerX = (rect.right - rect.left) / 2.0;
	double centerY = (rect.bottom - rect.top) / 2.0;

	// 2. 중심점으로부터의 픽셀 차이(Offset) 계산
	double dx = pt.x - centerX;
	double dy = pt.y - centerY;

	// 3. 해상도를 곱하여 실제 위경도 변화량 산출
	// Lon = 중심경도 + (가로차이 * 해상도)
	// Lat = 중심위도 - (세로차이 * 해상도) -> 화면 Y는 아래로 증가, 위도는 위로 증가하기 때문
	lon = m_centerLon + (dx * m_resolution);
	lat = m_centerLat - (dy * m_resolution);
}


void CMapEngine::SetMapCenter(double lon, double lat)
{
	// 1. 중심 좌표 업데이트
	m_centerLon = lon;
	m_centerLat = lat;

	// 2. 위도 범위 제한 (EPSG:4326 규격상 -90 ~ 90도)
	if (m_centerLat > 90.0) m_centerLat = 90.0;
	if (m_centerLat < -90.0) m_centerLat = -90.0;

	// 3. 경도 범위 정규화 (-180 ~ 180도)
	while (m_centerLon > 180.0) m_centerLon -= 360.0;
	while (m_centerLon < -180.0) m_centerLon += 360.0;

	// 4. 새로운 중심에 맞춰 그려야 할 타일 인덱스(startX, endX 등) 재계산
r			CalculateVisibleRange(m_hWnd);

	// 5. 화면 재그리기 요청 (배경을 지우지 않고 덮어쓰기 위해 FALSE 사용)
	//Invalidate(FALSE);
}


// 특정 이벤트 핸들러 내부에서 호출
void CMapEngine::OnMoveToTarget()
{
	// 경도(Longitude) 37.0, 위도(Latitude) 125.0으로 설정
	SetMapCenter(37.0, 125.0);
}
*/
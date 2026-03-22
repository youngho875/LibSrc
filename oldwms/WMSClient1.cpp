#include "pch.h"
#include "WMSClient.h"

#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

/*
UINT CWMSClient::DownloadThread(LPVOID pParam)
{
    CWMSClient* pEngine = (CWMSClient*)pParam;

    while (pEngine->m_bIsRunning)
    {
        // 이벤트가 신호를 줄 때까지 대기 (CPU 점유율 0%)
        WaitForSingleObject(pEngine->m_hEvent, INFINITE);

        if (!pEngine->m_bIsRunning) break;

        while (true)
        {
            TileRequest req;

            // 큐에서 작업 하나 꺼내기 (Thread-Safe)
            {
                CSingleLock lock(&pEngine->m_csQueue, TRUE);
                if (pEngine->m_workQueue.empty())
                    break;

                req = pEngine->m_workQueue.front();
                pEngine->m_workQueue.pop_front();
            }

            // 실제 다운로드 수행 (시간이 걸리는 작업)
            CImage* pNewImg = new CImage();
            CString url = pEngine->MakeUrl(req.x, req.y, req.z);
            HRESULT hr = pNewImg->Load(url);

            if (SUCCEEDED(hr))
            {
                // 캐시에 저장 (Thread-Safe)
                pEngine->AddTileToCache(req.x, req.y, req.z, pNewImg);

                // 메인 UI 스레드에 화면 갱신 요청
                ::PostMessage(pEngine->m_hWnd, WM_USER + 100, 0, 0);
            }
            else
            {
                delete pNewImg;
            }
        }
    }
    return 0;
};
*/

UINT CWMSClient::DownloadThread(LPVOID pParam)
{
    CWMSClient* pEngine = (CWMSClient*)pParam;

    // 임시 파일 경로 설정 (예: AppData 혹은 실행 파일 경로)
    TCHAR szTempPath[MAX_PATH] = "D:/dev/Map/Bin/Release/Temp";
    GetTempPath(MAX_PATH, szTempPath);

    while (pEngine->m_bIsRunning)
    {
        WaitForSingleObject(pEngine->m_hEvent, INFINITE);
        if (!pEngine->m_bIsRunning) break;

        while (true)
        {
            TileRequest req;
            {
                CSingleLock lock(&pEngine->m_csQueue, TRUE);
                if (pEngine->m_workQueue.empty()) break;
                req = pEngine->m_workQueue.front();
                pEngine->m_workQueue.pop_front();
                pEngine->m_queuedKeys.erase(pEngine->MakeKey(req.x, req.y, req.z));
            }

            CString url = pEngine->MakeUrl(req.x, req.y, req.z);
            CString localFile;
            localFile.Format(_T("%s/tile_%d_%d_%d.png"), szTempPath, req.z, req.x, req.y);

            // URL로부터 파일을 로컬로 다운로드
            HRESULT hrDl = URLDownloadToFile(NULL, url, localFile, 0, NULL);

            if (SUCCEEDED(hrDl)) 
            {
                CImage* pNewImg = new CImage();
                // 로컬에 저장된 파일로부터 로드 (nativeImage NULL 방지)
                HRESULT hrLoad = pNewImg->Load(localFile);

                if (SUCCEEDED(hrLoad))
                {
                    pEngine->AddTileToCache(req.x, req.y, req.z, pNewImg);
                    ::PostMessage(pEngine->m_hWnd, WM_USER + 100, 0, 0);
                }
                else
                {
                    delete pNewImg;
                }
                // 사용한 임시 파일 삭제
                DeleteFile(localFile);
            }
        }
    }
    return 0;
}


CWMSClient::CWMSClient(HWND hWnd) 
    : m_hWnd(hWnd), 
    m_centerLon(127.0), m_centerLat(37.0),
    m_Level(0),
    m_startX(0), m_endX(0),
    m_startY(0), m_endY(0),
    m_bIsRunning(true) 
{
    mCoordMgr = CCoordMgr::GetInst();
    mCoord.initialize_adaptation_data(); 


    if (::IsWindow(m_hWnd))
    {
        RECT rect;
        ::GetClientRect(m_hWnd, &rect);
        //CCoordMgr::GetInst()->InitDisplay(rect.right, rect.bottom);

        DOUBLE MinWorldX = 0;
        DOUBLE MinWorldY = 0;
        DOUBLE MaxWorldX = 0;
        DOUBLE MaxWorldY = 0;

        double XYLeft, XYRight, XYTop, XYBottom;
        double GeoLeft, GeoRight, GeoTop, GeoBottom;

        mCoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &XYLeft, &XYTop);
        mCoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &XYRight, &XYBottom);
        mCoord.x_y_to_degrees_lat_long(XYLeft, XYTop, &GeoTop, &GeoLeft);
        mCoord.x_y_to_degrees_lat_long(XYRight, XYBottom, &GeoBottom, &GeoRight);

        m_Level = CalcLevel(GeoLeft, GeoTop, GeoRight, GeoBottom, rect);

        UpdateResolution();

        CalculateVisibleRange();
    }


    InitializeSRWLock(&m_srwCache);
    //InitializeCriticalSection(&m_csQueue);
    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    AfxBeginThread(DownloadThread, this);
}


CWMSClient::~CWMSClient()
{
    m_bIsRunning = false;
    SetEvent(m_hEvent);
    // 메모리 해제 로직 생략 (m_tileCache 순회 delete)
    //DeleteCriticalSection(&m_csQueue);
}


TileData* CWMSClient::GetTileFromCache(int x, int y, int z)
{
    CString key = MakeKey(x, y, z);

    // [Shared Lock] 여러 스레드가 동시에 읽기 가능
    AcquireSRWLockShared(&m_srwCache);
    auto it = m_tileCache.find(key);
    if (it == m_tileCache.end())
    {
        ReleaseSRWLockShared(&m_srwCache);
        return nullptr;
    }
    TileData* pData = it->second;
    ReleaseSRWLockShared(&m_srwCache);

    // [Exclusive Lock] LRU 순서 갱신 (리스트 수정)
    AcquireSRWLockExclusive(&m_srwCache);
    m_lruList.erase(pData->itList);
    m_lruList.push_front(key);
    pData->itList = m_lruList.begin();
    ReleaseSRWLockExclusive(&m_srwCache);

    return pData;
}


long CWMSClient::CalcLevel(double Geoleft, double Geotop, double Georight, double Geobottom, CRect ClinetRect)
{
    DOUBLE GeoWidth = (Georight - Geoleft) < 0 ? -(Georight - Geoleft) : (Georight - Geoleft);

    DOUBLE ScreenWidth = ClinetRect.right - ClinetRect.left;

    double UnitPerPixel = GeoWidth / ScreenWidth;
    double u = log10(180.0 / (256 * UnitPerPixel));
    double d = log10(2.0);

    return  (u / d);
}


CString CWMSClient::MakeUrl(int x, int y, int z)
{
    double minLon, minLat, maxLon, maxLat;
    GetTileBBox(x, y, z, minLon, minLat, maxLon, maxLat);

    CString baseUrl = _T("http://10.240.33.120/cgi-bin/mapserv.exe");
    CString mapFile = _T("/ms4w/apps/local-demo/ne1_hr.map"); // _T("/ms4w/apps/local-demo/height.map");
    CString url = _T("");

    // WMS 1.1.1 기준 (서버 설정에 따라 레이어 이름 등을 수정하세요)
    //url.Format(_T("%s?map=%s&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap")
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


void CWMSClient::GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat)
{
    double nX = pow(2.0, z + 1);
    double nY = pow(2.0, z);
    minLon = (double)x / nX * 360.0 - 180.0;
    maxLon = (double)(x + 1) / nX * 360.0 - 180.0;
    maxLat = 90.0 - (double)y / nY * 180.0;
    minLat = 90.0 - (double)(y + 1) / nY * 180.0;
}


bool CWMSClient::IsInQueue(int x, int y, int z)
{
    // 키 생성 (예: "12_10_20")
    CString key;
    key.Format(_T("%d_%d_%d"), z, x, y);

    // Set에서 즉시 검색
    return m_queuedKeys.find(key) != m_queuedKeys.end();
}


void CWMSClient::RequestTile(int x, int y, int z)
{
    CSingleLock lock( &m_csQueue, TRUE);

    if (IsInQueue(x, y, z)) 
        return;

    // 큐에 추가
    m_workQueue.push_back({ x, y, z });

    // 검색용 Set에도 추가
    CString key;
    key.Format(_T("%d_%d_%d"), z, x, y);
    m_queuedKeys.insert(key);

    SetEvent(m_hEvent);
}


void CWMSClient::CalculateVisibleRange()
{

    if (!::IsWindow(m_hWnd))
        return;

    RECT rc;
    GetClientRect(m_hWnd, &rc);

    //int w = CCoordMgr::GetInst()->m_nScreenWidth;
    //int h = CCoordMgr::GetInst()->m_nScreenHeight;
    
    double wmX, wmY, topLat, leftLon;
    double wMX, wMY, bottomLat, rightLon;

    mCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &wmX, &wmY);
    mCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &wMX, &wMY);
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

    //m_startX = (int)floor((leftLon + 180.0 + 1e-9) / degPerTileX);
    //m_endX = (int)floor((rightLon + 180.0 - 1e-9) / degPerTileX);
    //m_startY = (int)floor((90.0 - topLat + 1e-9) / degPerTileY);
    //m_endY = (int)floor((90.0 - bottomLat - 1e-9) / degPerTileY);


    // 상하 반전 및 유효 범위 강제 제한
    // 위도가 바뀌어 startY > endY가 된 경우 보정
    if (m_startY > m_endY)
        std::swap(m_startY, m_endY);
    if (m_startX > m_endX)
        std::swap(m_startX, m_endX);

    // 마진 추가 (화면 경계 타일 잘림 방지)
    m_startX -= 1; m_endX += 1;
    m_startY -= 1; m_endY += 1;


    // 전체 타일 범위를 벗어나지 않도록 클램핑(Clamping)
    m_startX = max(0, m_startX);
    m_startY = max(0, m_startY);
    m_endX = min((int)nX - 1, m_endX);
    m_endY = min((int)nY - 1, m_endY);


    CleanUpWorkQueue();
}


void CWMSClient::AddTileToCache(int x, int y, int z, CImage* pImg) 
{
    CString key = MakeKey(x, y, z);

    AcquireSRWLockExclusive(&m_srwCache);

    // 이미 존재하는지 확인
    if (m_tileCache.find(key) != m_tileCache.end())
    {
        ReleaseSRWLockExclusive(&m_srwCache);
        return;
    }

    // 용량 초과 시 가장 오래된 것 제거
    if (m_tileCache.size() >= MAX_CACHE_SIZE)
    {
        CString oldKey = m_lruList.back();
        TileData* pOld = m_tileCache[oldKey];
        delete pOld;
        m_tileCache.erase(oldKey);
        m_lruList.pop_back();
    }

    // 새 타일 추가
    TileData* pNew = new TileData{ x, y, z };
    pNew->image.Attach(pImg->Detach());
    m_lruList.push_front(key);
    pNew->itList = m_lruList.begin();
    m_tileCache[key] = pNew;

    ReleaseSRWLockExclusive(&m_srwCache);
}


void CWMSClient::CleanUpWorkQueue()
{
    // 큐 접근을 위해 Lock 설정
    CSingleLock lock(&m_csQueue, TRUE);

    if (m_workQueue.empty()) return;

    // 람다식을 사용하여 범위 밖의 타일을 찾아 제거 (C++11 이상)
    // 1. 현재 레벨과 다르거나 2. 현재 화면 인덱스 범위 밖인 경우
    m_workQueue.erase(
        std::remove_if(m_workQueue.begin(), m_workQueue.end(),
            [this](const TileRequest& req) 
            {
                // 현재 줌 레벨이 다르다면 무조건 삭제
                if (req.z != m_Level) 
                    return true;

                // 현재 가시 범위(Margin 포함)를 벗어나면 삭제
                if (req.x < m_startX || req.x > m_endX ||
                    req.y < m_startY || req.y > m_endY)
                {
                    return true;
                }
                return false;
            }), 
        m_workQueue.end()
   );
}


void CWMSClient::MoveMap(int dx, int dy)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //m_centerLon -= (dx * m_resolution);
    //m_centerLat += (dy * m_resolution);

    double mapSize = pow(2.0, m_Level) * 256.0;

    // 픽셀 이동량을 위경도 이동량으로 변환
    double dLon = (double)dx * 360.0 / mapSize;
    double dLat = (double)dy * 180.0 / mapSize;

    m_centerLon -= dLon;
    m_centerLat += dLat; // 위도는 픽셀 좌표와 위경도 방향이 반대이므로 더해줌

    SetCenter(m_centerLon, m_centerLat);


    CalculateVisibleRange();
}


double CWMSClient::GetResolution() const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_resolution;
}


double CWMSClient::GetCenterLon() const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_centerLon;
}


double CWMSClient::GetCenterLat() const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_centerLat;
}


int CWMSClient::GetZoom() const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_Level;
}


void CWMSClient::SetCenter(double lon, double lat)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_centerLon = lon;
    m_centerLat = lat;

    CalculateVisibleRange(); // 중심 바뀌면 범위 재계산
}


void CWMSClient::SetZoom(int zoom)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_Level = zoom;
    UpdateResolution();
    CalculateVisibleRange();
}


void CWMSClient::OnSize(int w, int h)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

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


void CWMSClient::OnDrawTile(CDC* pDC)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CRect rect; GetClientRect(m_hWnd, &rect);

    //픽셀 배율 설정(1도당 픽셀 수)
    double fS = 1.0 / m_resolution;
    //mCoordMgr->m_fScale = fS;
    mCoordMgr->SetDisplayCenter(rect.Width() / 2, rect.Height() / 2);

    CDC memDC; CBitmap bmp;
    memDC.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    memDC.SelectObject(&bmp);

    memDC.FillSolidRect(&rect, RGB(10, 10, 10));

    for (int y = m_startY; y <= m_endY; y++)
    {
        for (int x = m_startX; x <= m_endX; x++)
        {
            // 타일의 BBOX 정보 가져오기 (minLon, maxLat이 좌상단 기준점)
            double minLon, minLat, maxLon, maxLat;
            GetTileBBox(x, y, m_Level, minLon, minLat, maxLon, maxLat);

            // 선형 변환 대수: 위경도 차이를 WorldXY로 대입
            double dWorldX = minLon - m_centerLon;
            double dWorldY = maxLat - m_centerLat;
            double dWorldX1 = maxLon - m_centerLon;
            double dWorldY1= minLat - m_centerLat;

            long sx, sy, ex, ey;
            mCoordMgr->WorldXY2ScreenXY(dWorldX, dWorldY, &sx, &sy, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, mCoordMgr->m_fScale*fS);
            mCoordMgr->WorldXY2ScreenXY(dWorldX1, dWorldY1, &ex, &ey, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, mCoordMgr->m_fScale*fS);

            CRect rectTile((int)sx, (int)sy, (int)ex, (int)ey);

            int width = ex - sx;
            int height = ey - sy;

            TileData* pTile = GetTileFromCache(x, y, m_Level);
            if (pTile && !pTile->image.IsNull())
            {
                pTile->image.StretchBlt(memDC, sx, sy, width, height, SRCCOPY);
            }
            else 
            {
                memDC.Rectangle(sx, sy, ex, ey);
                RequestTile(x, y, m_Level);
            }
        }
    }
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
}


void CWMSClient::UpdateResolution()
{
    // EPSG:4326 Resolution
    m_resolution = 360.0 / (pow(2.0, m_Level + 1) * 256.0);
}


void CWMSClient::ClampCenter()
{
    // 현재 해상도에서 화면 절반이 차지하는 경도/위도 폭
    double halfWidthLon = (mCoordMgr->m_nScreenWidth * m_resolution) / 2.0;
    double halfHeightLat = (mCoordMgr->m_nScreenHeight * m_resolution) / 2.0;

    // 경도 제한: 화면 왼쪽이 -180도보다 작거나 오른쪽이 180도를 넘지 않게
    if (m_centerLon - halfWidthLon < -180.0)
        m_centerLon = -180.0 + halfWidthLon;
    if (m_centerLon + halfWidthLon > 180.0)
        m_centerLon = 180.0 - halfWidthLon;

    // 위도 제한: 화면 위쪽이 90도보다 크거나 아래쪽이 -90도보다 작지 않게
    if (m_centerLat + halfHeightLat > 90.0)
        m_centerLat = 90.0 - halfHeightLat;
    if (m_centerLat - halfHeightLat < -90.0)
        m_centerLat = -90.0 + halfHeightLat;
}
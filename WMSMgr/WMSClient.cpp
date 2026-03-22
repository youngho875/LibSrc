#include "pch.h"
#include "WMSClient.h"


CWMSClient::CWMSClient(HWND hWnd)
    : m_hWnd(hWnd),
    m_centerLon(127.0),
    m_centerLat(37.0),
    m_Level(0),
    m_startX(0), m_endX(0),
    m_startY(0), m_endY(0)
    //m_bIsRunning(true)
{
    mCoordMgr = CCoordMgr::GetInst();
    mCoord.initialize_adaptation_data();


    if (::IsWindow(m_hWnd))
    {
        CRect rect;
        ::GetClientRect(m_hWnd, &rect);
        //CCoordMgr::GetInst()->InitDisplay(rect.right, rect.bottom);

        //DOUBLE MinWorldX = 0;
        //DOUBLE MinWorldY = 0;
        //DOUBLE MaxWorldX = 0;
        //DOUBLE MaxWorldY = 0;

        double XYLeft, XYRight, XYTop, XYBottom;
        double GeoLeft, GeoRight, GeoTop, GeoBottom;

        mCoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &XYLeft, &XYTop);
        mCoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &XYRight, &XYBottom);
        mCoord.x_y_to_degrees_lat_long(XYLeft, XYTop, &GeoTop, &GeoLeft);
        mCoord.x_y_to_degrees_lat_long(XYRight, XYBottom, &GeoBottom, &GeoRight);

        m_Level = CalcLevel(GeoLeft, GeoTop, GeoRight, GeoBottom, rect);

        UpdateResolution();

        CalculateVisibleRange();

        StartWorkerThreads(4);
    }

}


CWMSClient::~CWMSClient()
{
    m_bStopThreads = true;
    m_cv.notify_all();
    for (auto& t : m_workerThreads)
    {
        if (t.joinable())
            t.join();
    }
}


void CWMSClient::StartWorkerThreads(int nThreadCount) 
{
    m_bStopThreads = false;
    for (int i = 0; i < nThreadCount; ++i) 
    {
        m_workerThreads.emplace_back([this]() 
        {
            CWMSClient::DownloadWorker(this, m_hWnd);
        });
    }
}


void CWMSClient::AddDownloadRequest(int z, int x, int y, CString url)
{
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        // 중복 요청 방지 로직 (이미 큐에 있는지 체크) 추가 권장
        m_workQueue.push_back(new TileRequest{ z, x, y, url });
    }
    m_cv.notify_one();
}


// 마우스 이동 시 기존 대기열 삭제 (가장 중요)
void CWMSClient::ClearRequests()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    for (auto req : m_workQueue)
        delete req;

    m_workQueue.clear();
}


/*
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
*/

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


void CWMSClient::DownloadWorker(CWMSClient* pEngine, HWND hNotifyWnd) 
{
    CInternetSession session(_T("WMS_Session"));

    while (true) {
        TileRequest* pReq = nullptr;
        {
            // 1. 큐에서 안전하게 데이터 꺼내기
            std::unique_lock<std::mutex> lock(pEngine->m_queueMutex);
            pEngine->m_cv.wait(lock, [pEngine] {
                return !pEngine->m_workQueue.empty() || pEngine->m_bStopThreads;
                });

            if (pEngine->m_bStopThreads) break;

            pReq = pEngine->m_workQueue.front();
            pEngine->m_workQueue.pop_front();
        }

        if (pReq)
        {
            CHttpFile* pFile = nullptr;
            try 
            {
                pFile = (CHttpFile*)session.OpenURL(pReq->sUrl, 1,
                    INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_TRANSFER_BINARY);

                if (pFile) 
                {
                    // 2. Content-Length를 확인하여 미리 메모리 확보 (Append 오류 방지)
                    DWORD dwContentLength = 0;
                    if (pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, dwContentLength)) 
                    {
                        pReq->data.SetSize(0, dwContentLength > 0 ? dwContentLength : 1024 * 64);
                    }

                    BYTE buffer[8192];
                    UINT nRead = 0;
                    while ((nRead = pFile->Read(buffer, sizeof(buffer))) > 0)
                    {
                        // 3. Append 대신 GetData()와 직접 복사 사용 (더 안정적)
                        int nCurrentSize = (int)pReq->data.GetSize();
                        pReq->data.SetSize(nCurrentSize + nRead);
                        memcpy(pReq->data.GetData() + nCurrentSize, buffer, nRead);
                    }

                    pFile->Close();
                    delete pFile;

                    // 성공 시에만 View로 전송
                    if (pReq->data.GetSize() > 0)
                    {
                        ::PostMessage(hNotifyWnd, WM_TILE_DOWNLOAD_COMPLETE, (WPARAM)pReq, (LPARAM)pEngine);
                    }
                    else 
                    {
                        delete pReq;
                    }
                }
            }
            catch (CInternetException* e) 
            {
                // 4. 예외 처리 필수 (네트워크 끊김 등)
                if (pFile) 
                {
                    pFile->Close(); 
                    delete pFile; 
                }
                delete pReq;
                e->Delete();
            }
        }
    }
}


IStream* CWMSClient::CreateStreamFromBuffer(const CByteArray& data)
{
    if (data.GetSize() == 0) return nullptr;

    // 1. 전역 메모리 할당 (GMEM_MOVEABLE 필수)
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data.GetSize());
    if (hGlobal == NULL) return nullptr;

    // 2. 메모리 잠금 및 데이터 복사
    void* pDest = GlobalLock(hGlobal);
    if (pDest != NULL) 
    {
        memcpy(pDest, data.GetData(), data.GetSize());
        GlobalUnlock(hGlobal);
    }
    else 
    {
        GlobalFree(hGlobal);
        return nullptr;
    }

    // 3. 메모리 핸들을 기반으로 IStream 생성
    IStream* pStream = nullptr;
    // TRUE를 인자로 주면, pStream->Release() 시 hGlobal도 자동으로 해제됩니다.
    if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) != S_OK)
    {
        GlobalFree(hGlobal);
        return nullptr;
    }

    return pStream;
}


void CWMSClient::OnTileDownloaded(TileRequest* pReq) 
{
    // 1. 이미지 로드 (CImage* 생성)
    CImage* pNewImg = new CImage();
    IStream* pStream = CreateStreamFromBuffer(pReq->data); // 스트림 생성 로직

    if (pNewImg->Load(pStream) == S_OK)
    {
        // 2. TileData 객체 생성 (중요!)
        TileData* pTile = new TileData();
        pTile->x = pReq->x;
        pTile->y = pReq->y;
        pTile->z = pReq->z;
        pTile->image = pNewImg; // CImage*를 TileData의 멤버에 대입

        // 3. 키 생성 및 Map 저장
        CString key;
        key.Format(_T("%d_%d_%d"), pReq->z, pReq->x, pReq->y);

        // 에러 해결 지점: pNewImg(CImage*)가 아니라 pTile(TileData*)을 넣어야 함
        m_tileCache[key] = pTile;

        // 4. LRU 리스트 관리
        m_lruList.push_front(key);
        pTile->itList = m_lruList.begin();
    }
    else 
    {
        delete pNewImg;
    }

    // 메모리 정리 및 화면 갱신
    delete pReq;
    ::InvalidateRect(m_hWnd, NULL, FALSE);
}


void CWMSClient::UpdateResolution()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // EPSG:4326 Resolution
    m_resolution = 360.0 / (pow(2.0, m_Level + 1) * 256.0);
}


void CWMSClient::CalculateVisibleRange() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!::IsWindow(m_hWnd))
        return;

    CRect rc;
    GetClientRect(m_hWnd, &rc);

    //int w = CCoordMgr::GetInst()->m_nScreenWidth;
    //int h = CCoordMgr::GetInst()->m_nScreenHeight;
    
    double fS = mCoordMgr->m_fScale * (1.0 / m_resolution);

    double wmX, wmY, topLat, leftLon;
    double wMX, wMY, bottomLat, rightLon;

    //mCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &wmX, &wmY, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, double fCenterX_W, double fCenterY_W, fS);
    mCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &wmX, &wmY, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, fS);
    mCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &wMX, &wMY, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, fS);
    mCoord.x_y_to_degrees_lat_long(wmX, wmY, &topLat, &leftLon);
    mCoord.x_y_to_degrees_lat_long(wMX, wMY, &bottomLat, &rightLon);

    double nX = pow(2.0, m_Level + 1);
    double nY = pow(2.0, m_Level);

    double degPerTileX = 360.0 / nX;
    double degPerTileY = 180.0 / nY;

    // 2. 인덱스 계산 (정확한 위치를 위해 floor 사용)
    // 48도 이상에서 안 나오는 문제를 해결하기 위해 부동소수점 오차(0.000001) 반영
    //m_startX = (int)floor((leftLon + 180.0) / degPerTileX);
    //m_endX = (int)floor((rightLon + 180.0) / degPerTileX);
    //m_startY = (int)floor((90.0 - topLat) / degPerTileY);
    //m_endY = (int)floor((90.0 - bottomLat) / degPerTileY);

    m_startX = (int)floor((leftLon + 180.0 + 1e-9) / degPerTileX);
    m_endX = (int)floor((rightLon + 180.0 - 1e-9) / degPerTileX);
    m_startY = (int)floor((90.0 - topLat + 1e-9) / degPerTileY);
    m_endY = (int)floor((90.0 - bottomLat - 1e-9) / degPerTileY);


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
}

void CWMSClient::CalculateVisibleRange(CRect rect)
{
    // 1. 화면의 절반 크기를 기반으로 월드 좌표(경위도) 상의 가시 거리 계산
    // m_resolution: 1픽셀당 도(degree) 단위 거리
    double halfW = rect.Width() / 2.0;   
    double halfH = rect.Height() / 2.0;

    // 2. 화면 좌상단(Left-Top)과 우하단(Right-Bottom)의 경위도 좌표 계산
    double leftLon = m_centerLon - (halfW * m_resolution);
    double topLat = m_centerLat + (halfH * m_resolution);
    double rightLon = m_centerLon + (halfW * m_resolution);
    double bottomLat = m_centerLat - (halfH * m_resolution);

    // 3. 타일 한 장의 크기(도 단위) 계산
    // Level 0: 360/2=180, Level 1: 360/4=90 ...
    double degWidth = 360.0 / pow(2.0, m_Level + 1);
    double degHeight = 180.0 / pow(2.0, m_Level);

    // 4. 경위도 좌표를 타일 인덱스(정수)로 변환
    // 경도는 -180부터 시작하므로 +180을 더해 0~360 기준으로 변환
    // 위도는 90(북극)부터 내려오므로 90-Lat로 변환
    m_startX = (int)((leftLon + 180.0) / degWidth);
    m_startY = (int)((90.0 - topLat) / degHeight);

    // 끝 인덱스는 올림(ceil) 효과를 주기 위해 정밀하게 계산
    m_endX = (int)((rightLon + 180.0) / degWidth);
    m_endY = (int)((90.0 - bottomLat) / degHeight);

    // 5. 인덱스 유효 범위 방어 코드 (지구 타일 범위를 벗어나지 않도록)
    // X축 타일 개수: 2^(Level+1), Y축 타일 개수: 2^Level
    int maxIdxX = (int)pow(2.0, m_Level + 1) - 1;
    int maxIdxY = (int)pow(2.0, m_Level) - 1;

    m_startX = max(0, m_startX);
    m_startY = max(0, m_startY);
    m_endX = min(maxIdxX, m_endX);
    m_endY = min(maxIdxY, m_endY);
}

/*
void CWMSClient::CalculateVisibleRange()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!::IsWindow(m_hWnd))
        return;

    RECT rect;
    GetClientRect(m_hWnd, &rect);

    // 1. 화면의 절반 크기를 기반으로 월드 좌표(경위도) 상의 가시 거리 계산
    // m_resolution: 1픽셀당 도(degree) 단위 거리
    double halfW = rect.width() / 2.0;
    double halfH = rect.height() / 2.0;

    // 2. 화면 좌상단(Left-Top)과 우하단(Right-Bottom)의 경위도 좌표 계산
    double leftLon = m_centerLon - (halfW * m_resolution);
    double topLat = m_centerLat + (halfH * m_resolution);
    double rightLon = m_centerLon + (halfW * m_resolution);
    double bottomLat = m_centerLat - (halfH * m_resolution);

    // 3. 타일 한 장의 크기(도 단위) 계산
    // Level 0: 360/2=180, Level 1: 360/4=90 ...
    double degWidth = 360.0 / pow(2.0, m_Level + 1);
    double degHeight = 180.0 / pow(2.0, m_Level);

    // 4. 경위도 좌표를 타일 인덱스(정수)로 변환
    // 경도는 -180부터 시작하므로 +180을 더해 0~360 기준으로 변환
    // 위도는 90(북극)부터 내려오므로 90-Lat로 변환
    m_startX = (int)((leftLon + 180.0) / degWidth);
    m_startY = (int)((90.0 - topLat) / degHeight);

    // 끝 인덱스는 올림(ceil) 효과를 주기 위해 정밀하게 계산
    m_endX = (int)((rightLon + 180.0) / degWidth);
    m_endY = (int)((90.0 - bottomLat) / degHeight);

    // 5. 인덱스 유효 범위 방어 코드 (지구 타일 범위를 벗어나지 않도록)
    // X축 타일 개수: 2^(Level+1), Y축 타일 개수: 2^Level
    int maxIdxX = (int)pow(2.0, m_Level + 1) - 1;
    int maxIdxY = (int)pow(2.0, m_Level) - 1;

    m_startX = max(0, m_startX);
    m_startY = max(0, m_startY);
    m_endX = min(maxIdxX, m_endX);
    m_endY = min(maxIdxY, m_endY);
}
*/

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


double CWMSClient::getResolution()
{
    return m_resolution;
}


void CWMSClient::OnDrawTile(CDC* pDC)
{
    // 1. 현재 레벨의 타일 한 장당 위경도 크기 계산
    double degWidth = 360.0 / pow(2.0, m_Level + 1);
    double degHeight = 180.0 / pow(2.0, m_Level);

    double fS = mCoordMgr->m_fScale * (1.0 / m_resolution);

    // 2. 가시 범위 내의 타일 루프 (m_startX ~ m_endX, m_startY ~ m_endY)
    for (int y = m_startY; y <= m_endY; ++y)
    {
        for (int x = m_startX; x <= m_endX; ++x)
        {
            // 타일의 실제 위경도 좌표 계산
            double tileLeftLon = (x * degWidth) - 180.0;
            double tileTopLat = 90.0 - (y * degHeight);

            // 3. 캐시에서 이미지가 있는지 확인
            TileData* pTile = GetTileFromCache(x, y, m_Level);

            if (pTile && pTile->image && !pTile->image->IsNull())
            {
                // 캐시에 있는 경우: 화면 좌표로 변환하여 그리기
                long sx, sy, ex, ey;
                mCoordMgr->WorldXY2ScreenXY(tileLeftLon - m_centerLon, tileTopLat - m_centerLat, &sx, &sy, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, fS);
                mCoordMgr->WorldXY2ScreenXY((tileLeftLon + degWidth) - m_centerLon, (tileTopLat - degHeight) - m_centerLat, &ex, &ey, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, fS);

                pTile->image->StretchBlt(pDC->GetSafeHdc(), sx, sy, ex - sx, ey - sy, 0, 0, 256, 256, SRCCOPY);
            }
            else
            {
                // 4. 캐시에 없는 경우: 스레드 풀에 다운로드 요청
                // 이미 큐에 들어가 있는지 확인 후 추가
                if (!IsInQueue(x, y, m_Level))
                {
                    AddDownloadRequest(x, y, m_Level, MakeUrl(x, y, m_Level));
                }

                // (옵션) 다운로드 중일 때 그릴 기본 배경이나 "Loading" 처리
                DrawLoadingPlaceholder(pDC, tileLeftLon, tileTopLat, degWidth, degHeight);
            }
        }
    }
}


void CWMSClient::DrawLoadingPlaceholder(CDC* pDC, double tileLeftLon, double tileTopLat, double degWidth, double degHeight)
{
    long sx, sy, ex, ey;
    // 타일의 위경도 영역을 화면 좌표(Pixel)로 변환
    mCoordMgr->WorldXY2ScreenXY(tileLeftLon - m_centerLon, tileTopLat - m_centerLat, &sx, &sy, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, mCoordMgr->m_fScale * (1.0 / m_resolution));
    mCoordMgr->WorldXY2ScreenXY((tileLeftLon + degWidth) - m_centerLon, (tileTopLat - degHeight) - m_centerLat, &ex, &ey, mCoordMgr->m_fCenterX, mCoordMgr->m_fCenterY, mCoordMgr->m_fScale * (1.0 / m_resolution));

    CRect rect(sx, sy, ex, ey);

    // 1. 연한 회색 바탕 그리기
    pDC->FillSolidRect(rect, RGB(240, 240, 240));

    // 2. 테두리 그리기 (타일 경계 확인용)
    CPen pen(PS_SOLID, 1, RGB(200, 200, 200));
    CPen* pOldPen = pDC->SelectObject(&pen);
    pDC->MoveTo(sx, sy);
    pDC->LineTo(ex, sy);
    pDC->LineTo(ex, ey);
    pDC->LineTo(sx, ey);
    pDC->LineTo(sx, sy);

    // 3. "Loading..." 텍스트 표시 (선택 사항)
    pDC->SetTextColor(RGB(150, 150, 150));
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(_T("Loading..."), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


    pDC->SelectObject(pOldPen);
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
    CString url = MakeUrl(x, y, z);

    // 이전에 만든 스레드 풀 매니저의 AddRequest 호출
    // m_pTilePool->AddRequest(z, x, y, url);

    // 큐 중복 방지를 위해 set 등에 저장
    m_queuedKeys.insert(MakeKey(x, y, z));
}


void CWMSClient::PrepareCoordMgr(CRect clientRect)
{
    if (!mCoordMgr) return;

    // 1. 해상도의 역수를 Scale로 설정 (1픽셀당 이동할 World 좌표의 비중)
    // m_resolution은 UpdateResolution()에서 계산된 값 (degree/pixel)
    mCoordMgr->m_fScale = 1.0 / m_resolution;

    // 2. 화면의 물리적 중심점 설정 (View의 크기가 변할 수 있으므로 매번 갱신)
    mCoordMgr->SetDisplayCenter(clientRect.Width() / 2.0, clientRect.Height() / 2.0);
}


TileData* CWMSClient::GetTileFromCache(int x, int y, int z)
{
    // 1. 타일 고유 키 생성 (예: "10_15_20" -> z_x_y)
    CString sKey;
    sKey.Format(_T("%d_%d_%d"), z, x, y);

    // 2. Map에서 해당 키가 있는지 찾기 
    auto it = m_tileCache.find(sKey);

    if (it != m_tileCache.end())
    {
        // 3. 타일 데이터 포인터 추출
        TileData* pTile = it->second;

        // 4. LRU(Least Recently Used) 관리:
        // 사용된 타일이므로 m_lruList의 현재 위치에서 제거하고 맨 앞(front)으로 이동
        if (pTile->itList != m_lruList.end())
        {
            m_lruList.erase(pTile->itList); // 기존 위치에서 삭제
            m_lruList.push_front(sKey);     // 맨 앞으로 추가
            pTile->itList = m_lruList.begin(); // 새 위치(반복자) 저장
        }

        return pTile; // 찾은 타일 반환
    }

    // 캐시에 없는 경우
    return nullptr;
}


void CWMSClient::AddTileToCache(int x, int y, int z, CImage* pImg)
{
    CString sKey;
    sKey.Format(_T("%d_%d_%d"), z, x, y);

    // 1. 이미 존재한다면 교체 (보통 발생하지 않음)
    if (m_tileCache.count(sKey)) 
    {
        // 기존 데이터 포인터 가져오기
        TileData* pOldData = m_tileCache[sKey];

        if (pOldData != nullptr)
        {
            // 내부 CImage 리소스 해제 (매우 중요)
            if (pOldData->image != nullptr)
            {
                if (!pOldData->image->IsNull()) {
                    pOldData->image->Destroy(); // GDI 핸들 반환
                }
                delete pOldData->image;       // CImage 객체 메모리 해제
                pOldData->image = nullptr;
            }

            // LRU 리스트에서 해당 키 제거
            m_lruList.erase(pOldData->itList);
            delete pOldData;
        }
        
        m_tileCache.erase(sKey);    //Map에서 해당 항목 제거 (이후 새로운 데이터를 넣기 위해)
    }

    // 2. 캐시 용량 초과 시 가장 오래된 타일 제거 (LRU)
    if (m_tileCache.size() >= MAX_CACHE_SIZE)
    {
        CString oldestKey = m_lruList.back(); // 리스트 끝이 가장 오래된 것
        TileData* pOldTile = m_tileCache[oldestKey];

        if (!pOldTile->image->IsNull()) 
            pOldTile->image->Destroy();

        delete pOldTile;

        m_tileCache.erase(oldestKey);
        m_lruList.pop_back();
    }

    // 3. 새 타일 생성 및 저장
    TileData* pNewTile = new TileData;
    pNewTile->x = x; pNewTile->y = y; pNewTile->z = z;
    // pImg는 이미 외부에서 생성되어 전달됨
    pNewTile->image->Attach(pImg->Detach());
    delete pImg; // 임시 객체 삭제

    m_lruList.push_front(sKey);
    pNewTile->itList = m_lruList.begin();
    m_tileCache[sKey] = pNewTile;
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

    //SetCenter(m_centerLon, m_centerLat);

    CRect rect;
    ::GetClientRect(m_hWnd, &rect);
    CalculateVisibleRange(rect);

    //CalculateVisibleRange();
}

/*
void CWMSClient::MoveMap(int deltaX, int deltaY)
{
    // 1. 픽셀 변화량에 해상도를 곱해 경위도 변화량 계산
    // deltaX: 양수면 오른쪽 이동, deltaY: 양수면 아래쪽 이동 (스크린 기준)

    double dLon = (double)deltaX * m_resolution;
    double dLat = (double)deltaY * m_resolution;

    // 2. 중심점 업데이트
    // 마우스를 오른쪽으로 밀면(deltaX > 0), 지도의 중심은 왼쪽(Lon 감소)으로 가야 함
    m_centerLon -= dLon;
    // 마우스를 아래로 밀면(deltaY > 0), 지도의 중심은 위쪽(Lat 증가)으로 가야 함
    m_centerLat += dLat;

    // 3. 경도 범위 제한 (-180 ~ 180)
    if (m_centerLon > 180.0)  m_centerLon -= 360.0;
    if (m_centerLon < -180.0) m_centerLon += 360.0;

    // 4. 위도 범위 제한 (-90 ~ 90)
    if (m_centerLat > 90.0)  m_centerLat = 90.0;
    if (m_centerLat < -90.0) m_centerLat = -90.0;

    // 5. 변화된 중심점에 맞춰 가시 범위 재계산
    CRect rect;
    ::GetClientRect(m_hWnd, &rect);
    CalculateVisibleRange();
}
*/
void CWMSClient::MoveCenter(double dLon, double dLat)
{
    m_centerLon += dLon;
    m_centerLat += dLat;


    // 경도 범위 제한 (-180 ~ 180)
    if (m_centerLon > 180.0) m_centerLon -= 360.0;
    if (m_centerLon < -180.0) m_centerLon += 360.0;


    // 가시 범위 다시 계산 (startX, endX 등) 
    CalculateVisibleRange();
}


void CWMSClient::SetCenter(double lon, double lat)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_centerLon = lon;
    m_centerLat = lat;

    CalculateVisibleRange(); // 중심 바뀌면 범위 재계산
}


void CWMSClient::ZoomControl()
{


    CRect rect;
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

    CalculateVisibleRange(rect);



}

void CWMSClient::UpdateScale(int nLevel)
{
    // 1. 현재 레벨의 가로 타일 개수 계산 (EPSG:4326은 보통 2^(L+1))
    // Level 0 -> 2개, Level 1 -> 4개 ...
    double numTilesX = pow(2.0, nLevel + 1);

    // 2. 전체 픽셀 너비 계산
    double totalPixelWidth = numTilesX * 256.0;

    // 3. m_fScale 계산 (1도당 몇 픽셀인가?)
    m_fScale = totalPixelWidth / 360.0;
}



/*
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
*/
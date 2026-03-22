#pragma once

#include <afxmt.h>
#include <afxinet.h>

#undef	AFX_DATA
#ifdef	WMSMGR_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif


#include <atlimage.h>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <list>
#include <set>
#include <condition_variable>
#include <objbase.h>
#include <gdiplus.h>


#define     MAX_CACHE_SIZE              3000
#define     WM_TILE_DOWNLOAD_COMPLETE   WM_USER + 1  


struct TileData
{
    int x, y, z;
    CImage *image;
    std::list<CString>::iterator itList;     // 리스트 내 위치를 저장하여 삭제 시 O(1) 성능 보장
};

struct TileRequest 
{
    int x, y, z;
    CString sUrl;
    CByteArray data;
};


class AFX_DATA CWMSClient
{
public:
    CWMSClient(HWND hWnd);
    virtual ~CWMSClient();

public:
    void OnDrawTile(CDC* pDC);
    void MoveMap(int dx, int dy);
    void MoveCenter(double dLon, double dLat);
    void OnSize(int w, int h);

    double GetResolution() const;
    double GetCenterLon() const;
    double GetCenterLat() const;
    int GetZoom() const;
    void SetCenter(double lon, double lat);
    void SetZoom(int zoom);
    void ClampCenter();
    long CalcLevel(double Geoleft, double Geotop, double Georight, double Geobottom, CRect ClinetRect);

    // 스레드 풀 제어
    void StartWorkerThreads(int nCount = 8);
    void StopWorkerThreads();
    void AddDownloadRequest(int z, int x, int y, CString url);
    void ClearRequests();

    // 외부(View)에서 호출될 결과 처리 함수
    void OnTileDownloaded(TileRequest* pReq);

private:
    CCoordMgr* mCoordMgr;
    CCoord mCoord;

    HWND    m_hWnd;
    double  m_centerLon, m_centerLat;
    double  m_resolution;
    int     m_nWidth, m_nHeight;
    int     m_Level, m_startX, m_startY, m_endX, m_endY;
    double m_fScale;

public:
    double getResolution();
    void CalculateVisibleRange();
    void CalculateVisibleRange(CRect rect);
    void ZoomControl();
    void UpdateScale(int nLevel);


private:
    // 1. 캐시 및 LRU 로직
    void AddTileToCache(int x, int y, int z, CImage* pImg);
    TileData* GetTileFromCache(int x, int y, int z);
    void EvictOldest(); // 오래된 타일 제거
    void GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat);
    CString MakeUrl(int x, int y, int z);
    void RequestTile(int x, int y, int z);
    bool IsInQueue(int x, int y, int z);
    void UpdateResolution();
    void PrepareCoordMgr(CRect clientRect);
    void DrawLoadingPlaceholder(CDC* pDC, double tileLeftLon, double tileTopLat, double degWidth, double degHeight);
    IStream* CreateStreamFromBuffer(const CByteArray& data);

private:
    // 스레드 함수 (정적 함수로 선언하여 스레드 생성 시 사용)
    static void DownloadWorker(CWMSClient* pEngine, HWND hNotifyWnd);

    std::map<CString, TileData*> m_tileCache;
    std::list<CString> m_lruList; // 최근 사용 순서 저장 (앞쪽이 최신)
    std::set<CString> m_queuedKeys;

    std::vector<std::thread> m_workerThreads;
    std::deque<TileRequest*> m_workQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    bool m_bStopThreads = false;
    //HWND m_hNotifyWnd; // View의 핸들 (PostMessage용)

    // 타일 캐시 (간단한 예시: 좌표를 키로 사용하는 map)
    //std::map<CString, CImage*> m_tileCache;
    //std::mutex m_cacheMutex;

    CString MakeKey(int x, int y, int z)
    {
        CString s;
        s.Format(_T("%d_%d_%d"), z, x, y);

        return s;
    };
};


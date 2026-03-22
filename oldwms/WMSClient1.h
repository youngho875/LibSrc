#pragma once

#include <afxmt.h>

#undef	AFX_DATA
#ifdef	WMSMGR_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#include <atlimage.h>
#include <unordered_map>
#include <list>
#include <deque>
#include <synchapi.h> // SRWLock용
#include <set>
#include <map>
#include <gdiplus.h>




#define     MAX_CACHE_SIZE  1000


struct TileData
{
    int x, y, z;
    CImage image;
    std::list<CString>::iterator itList;     // 리스트 내 위치를 저장하여 삭제 시 O(1) 성능 보장
};


struct TileRequest 
{
    int x, y, z;
};


class AFX_DATA CWMSClient
{
public:
    CWMSClient(HWND hWnd);
    virtual ~CWMSClient();


public:
    void OnDrawTile(CDC* pDC);
    void MoveMap(int dx, int dy);
    void OnSize(int w, int h);

    double GetResolution() const;
    double GetCenterLon() const;
    double GetCenterLat() const;
    int GetZoom() const;
    void SetCenter(double lon, double lat);
    void SetZoom(int zoom);
    void ClampCenter();
    long CalcLevel(double Geoleft, double Geotop, double Georight, double Geobottom, CRect ClinetRect);


private:
    CCoordMgr* mCoordMgr;
    CCoord mCoord;

    HWND    m_hWnd;
    double  m_centerLon, m_centerLat;
    //double  m_resolution;


    int m_nWidth, m_nHeight;

    // 가시 범위 타일 인덱스
    int m_Level, m_startX, m_startY, m_endX, m_endY;
    //double m_minVisibleLon, m_maxVisibleLon, m_minVisibleLat, m_maxVisibleLat;

public:
    double  m_resolution;

    void UpdateResolution();
    void CalculateVisibleRange();
    void CleanUpWorkQueue();

private:
    // 1. 캐시 및 LRU 로직
    void AddTileToCache(int x, int y, int z, CImage* pImg);
    TileData* GetTileFromCache(int x, int y, int z);
    void EvictOldest(); // 오래된 타일 제거
    void GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat);
    CString MakeUrl(int x, int y, int z);
    //void UpdateResolution();
    void RequestTile(int x, int y, int z);
    //void CalculateVisibleRange();
    bool IsInQueue(int x, int y, int z);
    //void CleanUpWorkQueue();

    std::map<CString, TileData*> m_tileCache;
    std::list<CString> m_lruList; // 최근 사용 순서 저장 (앞쪽이 최신)
    std::set<CString> m_queuedKeys;


    // 동기화 (SRW Lock)
    SRWLOCK m_srwCache; // 캐시용 RW Lock
    CCriticalSection m_csQueue; // 큐용 CS

    // 3. 비동기 스레드 및 큐
    std::deque<TileRequest> m_workQueue;
    HANDLE m_hThread;
    HANDLE m_hEvent;
    bool m_bIsRunning;

    static UINT DownloadThread(LPVOID pParam);


    // 유틸리티
    CString MakeKey(int x, int y, int z)
    {
        CString s;
        s.Format(_T("%d_%d_%d"), z, x, y);

        return s;
    };
};

#undef	AFX_DATA
#define	AFX_DATA
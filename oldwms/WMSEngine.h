#pragma once

#undef	AFX_DATA
#ifdef	WMSMGR_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#include <atlimage.h> // CImage 사용을 위해 필요
#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include "gdal_priv.h"
#include "curl/curl.h"

#define MAX_CACHE_SIZE	500		// 최대 캐쉬 갯수

struct TileData 
{
	CImage image;        // 타일 이미지
	int x, y, z;         // 타일 좌표 (Z/X/Y)
	double minLon, maxLon, minLat, maxLat; // 타일의 위경도 영역(EPSG:4326)
	ULONGLONG lastUsedTime; // 캐시 교체 알고리즘(LRU)용 (선택사항)
};

// 다운로드 요청 구조체
struct TileRequest {
	int x, y, z;
};

// curl 데이터를 담을 구조체
struct MemBuffer {
	std::vector<unsigned char> data;
};


class AFX_DATA CWMSEngine 
{
public:
	CWMSEngine(HWND hWnd);
	virtual ~CWMSEngine();

	//void Initialize(HWND hWnd);
	void MoveMap(int dx, int dy);
	void WMSDraw(CDC* pDC);
	void OnSize(int w, int h);

	void SetMapCenter(double lon, double lat);
	void ClearCache();
	// 오래된 타일 삭제
	void RemoveOldestTile();
	BOOL LoadTileGDAL(const CString& strPath, TileData& tile);
	CString MakeWmsUrl(int x, int y, int z);
	BOOL DownloadTile(int x, int y, int z, const CString& savePath);
	void OnDrawMap(CDC* pDC);


private:
private:
	CCoordMgr* mCoordMgr;
	CCoord mCoord;


	double GetGridInterval();
	void DrawGrid(HDC hdc);

	void RequestTileDownload(int x, int y, int z);

private:
	// 핵심 변수
	HWND    m_hWnd;
	double  m_centerLon, m_centerLat;
	double  m_resolution;
	//int     m_zoom;

	int m_nWidth, m_nHeight;

	// 가시 범위 타일 인덱스
	int m_Level, m_startX, m_startY, m_endX, m_endY;

	// 마우스 제어
	bool    m_bDragging;
	POINT   m_lastMousePos;

	// LRU 캐시
	std::unordered_map<std::string, std::pair<TileData*, std::list<std::string>::iterator>> m_tileCache;
	std::list<std::string> m_usageList;
	

	// 내부 함수
	void UpdateResolution();
	void CalculateVisibleRange();
	void AddToCache(const std::string& key, TileData* pTile);
	TileData* GetOrFetchTile(int x, int y, int z);
	TileData* DownloadAndLoadMFC(int x, int y, int z);
	void GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat);


	// 좌표 변환 (CoordMgr 연동)
	//void LatLonToScreen(double lon, double lat, long* nXS, long* nYS);
	//void ScreenToLatLon(int nXS, int nYS, double* outLon, double* outLat);

public:
	void SetCenter(double lon, double lat);
	void SetZoom(int zoom);
	double GetResolution() const;
	double GetCenterLon() const;
	double GetCenterLat() const;
	int GetZoom() const;

	
};

/*
class CWMSEngine
{
public:
	CWMSEngine::CWMSEngine();
	virtual ~CWMSEngine();

protected:
	HWND m_hWnd;

	BOOL    m_bDragging;      // 드래그 중인지 여부
	CPoint  m_lastMousePos;   // 이전 마우스 위치 (픽셀)

	// 타일 관리용 구조체
	std::unordered_map<std::string, std::pair<TileData*, std::list<std::string>::iterator>> m_tileCache;

	// 타일 사용 순서를 기록하는 리스트 (가장 앞이 최신, 가장 뒤가 오래된 것)
	std::list<std::string> m_usageList;

	// 최대 캐시 개수 제한 (예: 200개)
	//const int MAX_CACHE_SIZE = 50;
	static size_t WriteMemCallback(void* contents, size_t size, size_t nmemb, void* userp);

	void AddToCache(const std::string& key, TileData* pTile);
	void RemoveOldestTile();

	// 현재 화면에 그려야 할 타일의 인덱스 범위
	// 영역(Area) 계산 루프에서 결정되는 값들입니다.
	int m_startX;
	int m_endX;
	int m_startY;
	int m_endY;

	// 3. 지도 상태 변수
	int m_zoom;          // 현재 줌 레벨
	double m_centerLon;  // 중심 경도
	double m_centerLat;  // 중심 위도
	double m_resolution; // 현재 픽셀당 위경도 해상도

	// 관련 함수
	void ClearCache();
	void CalculateVisibleRange(HWND hWnd);
	void UpdateResolution();
	//void PixelToLatLon(HWND hWnd, CPoint pt, double& lon, double& lat);
	void PixelToLatLon(HWND hWnd, POINT pt, double& lon, double& lat);

public:
	BOOL LoadTileGDAL(const CString& strPath, TileData& tile);
	//BOOL LoadTileGDAL(const CString& strPath, TileInfo& tile);
	void GetTileBBox(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat);
	void GetTileBBox2(int x, int y, int z, double& minLon, double& minLat, double& maxLon, double& maxLat);
	double GetLonFromX(int x, int z);
	double GetLatFromY(int y, int z);
	CString MakeWmsUrl(int x, int y, int z);

	TileData* GetOrFetchTile_directory(int x, int y, int z);
	TileData* GetOrFetchTile(int x, int y, int z);
	TileData* DownloadAndLoadMemGDAL(int x, int y, int z);
	TileData* DownloadAndLoadMFC(int x, int y, int z);

	BOOL DownloadTile(int x, int y, int z, const CString& savePath);
	//TileData* GetOrFetchTile(int x, int y, int z);
	//TileData* GetOrFetchTile2(int x, int y, int z);
	BOOL CreateDirectoryRecursive(const CString& strFilePath);
	TileData* LoadTileFromDisk(int z, int x, int y);

	void SetMapCenter(double lon, double lat);
	void OnMoveToTarget();
};
*/

#undef	AFX_DATA
#define	AFX_DATA
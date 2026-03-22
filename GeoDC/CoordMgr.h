#pragma once

#include <afxdisp.h>


#undef	AFX_DATA
#ifdef	GEODC_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class AFX_DATA CCoordMgr
{
private:
	// Singleton: 생성자와 대입 연산자를 private로 이동하여 외부 생성 방지
	CCoordMgr(void);
	CCoordMgr(const CCoordMgr&) = delete;            
	CCoordMgr& operator= (const CCoordMgr&) = delete; 

public:
	static CCoordMgr* GetInst(void);
	static void ReleaseInst(void); // 기존 코드 호환용
	void CCoordMgr::SetInst(CCoordMgr* instance);

public:
	void InitDisplayParameter(CRect rect);
	void InitDisplay(int nWidth, int nHeight);
	void SetCenterCoord(double lon, double lat);

	/*
	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
				long* nXScreenCoord, long* nYScreenCoord,
				double fCenterX, double fCenterY,
				double fWorldCenterX, double fWorldCenterY,
				double fScale);
	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
				long* nXScreenCoord, long* nYScreenCoord);
	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
				double* fXScreenCoord, double* fYScreenCoord);
	void ScreenXY2WorldXY(long nXScreenCoord, long nYScreenCoord,
				double* fXWorldCoord, double* fYWorldCoord,
				double fCenterX, double fCenterY,
				double fWorldCenterX, double fWorldCenterY,
				double fScale);
	void ScreenXY2WorldXY(long nXScreenCoord, long nYScreenCoord,
				double* fXWorldCoord, double* fYWorldCoord);
	*/
	
	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
				 long	*nXScreenCoord, long *nYScreenCoord, 
				 double fCenterX, double fCenterY, 
		 		 double fScale);
	void ScreenXY2WorldXY(int	nXScreenCoord, int nYScreenCoord,
				 double *fXWorldCoord, double *fYWorldCoord, 								
				 double fCenterX, double fCenterY, 
				 double fScale);
	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
					double *fXScreenCoord, double *fYScreenCoord, 
					double fCenterX, double fCenterY, 
					double fScale);

	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
					long	*nXScreenCoord, long *nYScreenCoord);

	void WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
					double *fXScreenCoord, double *fYScreenCoord);

	void ScreenXY2WorldXY(int nXScreenCoord, int nYScreenCoord,
						 double *fXWorldCoord, double *fYWorldCoord);

	

	double DeltaXYToBearing(double fDeltaX, double fDeltaY);
	// 성능 향상을 위해 짧은 수학 함수는 inline으로 선언
	inline double DeltaXYToRange(double fDeltaX, double fDeltaY)
	{
		return sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY);
	};

	//double DeltaXYToRange(double fDeltaX, double fDeltaY);
	static	double DeltaVxVyToHeading(double fVelFactorX, double fVelFactorY);
	static	double DeltaVxVyToSpeed(double fVelFactorX, double fVelFactorY);

	void	SetDisplayCenter(double fCenterX, double fCenterY);
	void	GetDisplayCenter(double *fCenterX, double *fCenterY);

	void	SetNewCenter(double fNewCenterX, double fNewCenterY);

	void	CalcNewCenterAndScale(CRect rect);
	void	static HeadingSpeedToVxVy(int nSpeed, int nHeading, double *fVx, double *fVy);
	void	UTM_TO_WGP(double x, double y, int izone, double &lon, double &lat);

	void	PowerSet(CCoordMgr* inst);

	void WorldLonLat2ScreenXY(double fLonWorldCoord, double fLatWorldCoord,
		double* fXScreenCoord, double* fYScreenCoord,
		double fCenterX, double fCenterY,
		double fScale);
	void WorldLonLat2ScreenXY(double fLonWorldCoord, double fLatWorldCoord,
		long* fXScreenCoord, long* fYScreenCoord,
		double fCenterX, double fCenterY,
		double fScale);

public:
	double	m_fCenterX; 
	double	m_fCenterY; 
	double	m_fScale; 
	int		m_nScreenWidth; 
	int		m_nScreenHeight; 
	int		m_nDisplayViewWidth;
	int		m_nDisplayViewHeight; 
	double	m_fHorVerRatio;
	bool m_bFlat;
	double m_fWorldCenterX; // 현재 화면 중심 경도
	double m_fWorldCenterY; // 현재 화면 중심 위도
};

//#define GP_COORDMGR CCoordMgr::GetInst()

#undef	AFX_DATA
#define	AFX_DATA






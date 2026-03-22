#pragma once




#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class CGGeoProj;

typedef	void	(CGGeoProj::*InverseFunction)(POINTGP *pGP, POINTPLANE *pPlane, int nPoints);
typedef void	(CGGeoProj::*ForwardFunction)(POINTPLANE *pPlane, POINTUTM *pUTM, int nPoints);


/////////////////////////////////////////////////////////////////////////////
// CGGeoProj command target

class CGPointGP;
class CGPointGK;
class CGPointUTM;
class CGPointUPS;

/**
* 투영 변환 관리(각종좌표에서 지리좌표로 또는 반대) class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGGeoProj
{
public:
	CGGeoProj(int eInx = EWGE);
	virtual ~CGGeoProj();
	int	SetEllipsoid(int eInx);
	int	GetEllipsoid() const;
// Attributes
public:
					// Using projections
					//-----------------------------------

protected:
	POINTGP		m_Base;		// All projections
	double		m_ScaleFactor;	// 
	ELLIPSOID	*m_pEllipsoid;	// All projections

	// Lambert Conformal Conic Projection
	double m_StdLat1;
	double m_StdLat2;

private:
	int m_nEllipsoidInx;

// Operations
public:
	double SetProjScaleFactor(double dScaleFactor);
	double GetProjScaleFactor() const;
	CGPointGP SetProjBase(CGPointGP gp);
	CGPointGP SetProjBase(POINTGP gpBase);
	CGPointGP SetProjBase(double lon, double lat);
	CGPointGP GetProjBase() const;

	// Lambert Conformal Conic Projection
	void SetStdParallel(double dStdParallel1, double dStdParallel2);
	
	//
	// 투영을 하지 않은 경우
	void ForwardGP(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints=1);
	void ForwardGP(POINTPLANE *pPlane, CGPointUTM *pUTM, int nPoints=1);
//	void ForwardGP(POINTPLANE *pPlane, CGPointUPS *pUPS, int nPoints=1);
	void InverseGP(CGPointGP * pGP, POINTPLANE * pPlane, int nPoints=1);
	void InverseGP(CGPointUTM * pUTM, POINTPLANE * pPlane, int nPoints=1);

	void ForwardTM(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints=1);
	void InverseTM(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints=1);

	//
	// UTM 투영을 사용한 경우 
	void ForwardUTM(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints=1);
	void ForwardUTM(POINTPLANE *pPlane, CGPointUTM *pUTM, int nPoints=1);
//	void ForwardUTM(POINTPLANE *pPlane, CGPointUPS *pUPS, int nPoints=1);
	void InverseUTM(CGPointGP *pGP, POINTPLANE * pPlane, int nPoints=1);
	void InverseUTM(CGPointUTM *pUTM, POINTPLANE * pPlane, int nPoints=1);

	// UTM 좌표를 지리좌표로 변환
	void ForwardUTM(CGPointUTM *pUTM, CGPointGP *pGP, int nPoints=1, int nZone=-1);
	// 지리좌표를 UTM 좌표로 변환
	void InverseUTM(CGPointGP *pGP, CGPointUTM *pUTM, int nPoints=1);

	// GK 투영을 사용한 경우 
	void ForwardGK(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints=1);
	void InverseGK(CGPointGP *pGP, POINTPLANE * pPlane, int nPoints=1);

	// GK 좌표를 지리좌표로 변환
	void ForwardGK(CGPointGK *pGK, CGPointGP *pGP, int nPoints=1, int nZone=-1);
	// 지리좌표를 GK 좌표로 변환
	void InverseGK(CGPointGP *pGP, CGPointGK *pGK, int nPoints=1);

	//
	// UPS 투영을 사용한 경우 
	void ForwardUPS(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints=1);
	void InverseUPS(CGPointGP *pGP, POINTPLANE * pPlane, int nPoints=1);

	// UPS 좌표를 지리좌표로 변환
	void ForwardUPS(CGPointUPS *pUPS, CGPointGP *pGP, int nPoints=1);
	// 지리좌표를 UPS 좌표로 변환
	void InverseUPS(CGPointGP *pGP, CGPointUPS *pUPS, int nPoints=1);

	void ForwardLCC(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints = 1);
	void InverseLCC(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints = 1);

	void ForwardMER(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints = 1);
	void InverseMER(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints = 1);

private:
	CGPointGP SetProjBase(int nUtmZone);
	int	   GetUtmZone(CGPointGP gp) const;
};

#define	GPtoUTM(Obj, pUtm, pGp, pPlane, nPoint) \
	(Obj).Forward(pPlane, pGP, nPoint); \
	(Obj).Inverse(pUtm, pPlane, nPoint)

#define UTMtoGP(Obj, pGp, pUtm, pPlane, nPoint) \
	(Obj).Forward(pPlane, pUtm, nPoint); \
	(Obj).Inverse(pGp, pPlane, nPoint)

#undef	AFX_DATA
#define	AFX_DATA



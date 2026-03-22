// GCalcDistance.h: interface for the CGCalcDistance class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define GDIST_ELLIPSOID 1
#define GDIST_UTM		2

class CGPointGeo;

class CGPointGP;
class CGPointUTM;
class CGEllipsoid;

/**
* 좌표 거리계산 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGCalcDistance
{
public:
	CGCalcDistance(int eInx = EWGE);	// default WGS84
	virtual ~CGCalcDistance();

public:
	void   SetEllipsoid(int eInx);

	double CalcDistance(CGPointGP& gpA, CGPointGP& gpB, int nMethod = R2_SAME_SURFACE);
	double CalcDistance(CGPointUTM& utmA, CGPointUTM& utmB);
	double CalcDistance(CGPointGeo *pPointGeoA, CGPointGeo *pPointGeoB, int nCalcDist = GDIST_UTM);
	CGPointGP OffsetDistance(CGPointGP& gp, double x, double y, int nUnit = GUNIT_M);

private:
	CGEllipsoid m_Ellipsoid;
};

#undef	AFX_DATA
#define	AFX_DATA



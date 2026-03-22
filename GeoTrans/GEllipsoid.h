// GEllipsoid.h: interface for the CGEllipsoid class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "GGeoObjStruct.h"


static ELLIPSOID __ellipsoid__[] = {
	{ 1.0, -1.0, 0.0 }, 
	{ 6377563.396, 299.3249646000, 0.00667053999998536320 }, //EAAY : Airy 1830
	{ 6377397.155, 299.1528128000, 0.00667437223180214480 }, //EBES : Bessel1841
	{ 6378206.400, 294.9786982000, 0.00676865799760964420 }, //ECLK : Clarke1866
	{ 6378249.145, 293.4650000000, 0.00680351128284906530 }, //ECLJ : Clarke1880
	{ 6377276.345, 300.8017000000, 0.00663784663019968780 }, //EEVE : Everest1830
	{ 6378144.000, 298.3000000000, 0.00669342162296594330 }, //EFIS : Fischer1960(Mercury)
	{ 6378150.000, 298.3000000000, 0.00669342162296594330 }, //EFIT : Fischer1968
	{ 6378160.000, 298.2471674270, 0.00669460532856765420 }, //EGRS : GRS1967
	{ 6378140.000, 298.2570000000, 0.00669438499958795000 }, //EGRT : GRS1975
	{ 6378137.000, 298.2572221010, 0.00669438002290078690 }, //EGRU : GRS1980
	{ 6378270.000, 297.0000000000, 0.00672267002233332100 }, //EHGH : Hough1956
	{ 6378388.000, 297.0000000000, 0.00672267002233332100 }, //EINT : International
	{ 6378245.000, 298.3000000000, 0.00669342162296594330 }, //EKRA : Krassovsky1940
	{ 6378160.000, 298.2500000000, 0.00669454185458763770 }, //ESUA : South American1969
	{ 6378165.000, 298.3000000000, 0.00669342162296594330 }, //EWGA : WGS60
	{ 6378145.000, 298.2500000000, 0.00669454185458763770 }, //EWGB : WGS66
	{ 6378135.000, 298.2600000000, 0.00669431777826672270 }, //EWGC : WGS72
	{ 6378137.000, 298.2572235630, 0.00669437999014131650 }	 //EWGE : WGS84
};

#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define R1_ARITHMETIC	1
#define R2_SAME_SURFACE	2
#define R3_SAME_VOLUME	3

/**
* 타원체 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGEllipsoid
{
public:
	CGEllipsoid();
	CGEllipsoid(CString name, double a, double rf);
	CGEllipsoid(int eInx);
	virtual ~CGEllipsoid();

// Operations
public:
	int  GetEllipsoid() const;

	void SetEllipsoid(CString name, double a, double rf);
	void SetEllipsoid(int eInx);

	CString GetName() const;

	double  GetSemiMajorAxis() const;
	double  GetSemiMinorAxis() const;

	double  GetFlattening() const;
	double  GetReciprocalFlattening() const;

	double  GetFirstEccentricity() const;
	double  GetFirstEccentricitySquared() const;

	double  GetSecondEccentricity() const;
	double  GetSecondEccentricitySquared() const;

	double  GetMeanRadius(int nMethod = R1_ARITHMETIC) const;

private:
	CString	m_Name;	      // 이름
	double	m_a;		// 장축(Semi-Major Axis)
	double	m_rf;		// 편평도의 역수(Reciprocal Flattening)
};

#undef	AFX_DATA
#define	AFX_DATA


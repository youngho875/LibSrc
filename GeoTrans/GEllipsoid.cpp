// GEllipsoid.cpp: implementation of the CGEllipsoid class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GGeoObjStruct.h"

#include <math.h>
#include "GEllipsoid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGEllipsoid::CGEllipsoid()
{
	m_Name = "";	// 이름
	m_a	   = 0.0;	// 장축(Semi-Major Axis)
	m_rf   = 0.0;	// 편평도의 역수(Reciprocal Flattening)
}

CGEllipsoid::CGEllipsoid(CString name, double a, double rf)
{
	this->SetEllipsoid(name, a, rf);
}

CGEllipsoid::CGEllipsoid(int eInx)
{
	this->SetEllipsoid(eInx);
}

CGEllipsoid::~CGEllipsoid()
{
}

void CGEllipsoid::SetEllipsoid(CString name, double a, double rf)
{
	m_Name = name;	// 이름
	m_a	   = a;		// 장축(Semi-Major Axis)
	m_rf   = rf;	// 편평도의 역수(Reciprocal Flattening)
}

int CGEllipsoid::GetEllipsoid() const
{
	int eInx = -1;

	if (this->m_Name == "Bessel1841")					eInx = EBES;
//	else if (this->m_Name == "Airy 1830")				eInx = EAAY;
	else if (this->m_Name == "Clarke1866")				eInx = ECLK;
	else if (this->m_Name == "Clarke1880")				eInx = ECLJ;
	else if (this->m_Name == "Everest1830")				eInx = EEVE;
//	else if (this->m_Name == "Fischer1960(Mercury)")	eInx = EFIS;
//	else if (this->m_Name == "Fischer1968")				eInx = EFIT;
	else if (this->m_Name == "GRS1967")					eInx = EGRS;
//	else if (this->m_Name == "GRS1975")					eInx = EGRT;
//	else if (this->m_Name == "GRS1980")					eInx = EGRU;
//	else if (this->m_Name == "Hough1956")				eInx = EHGH;
	else if (this->m_Name == "International")			eInx = EINT;
//	else if (this->m_Name == "Krassovsky1940")			eInx = EKRA;
//	else if (this->m_Name == "South American1969")		eInx = ESUA;
//	else if (this->m_Name == "WGS60")					eInx = EWGA;
//	else if (this->m_Name == "WGS66")					eInx = EWGB;
	else if (this->m_Name == "WGS72")					eInx = EWGC;
	else if (this->m_Name == "WGS84")					eInx = EWGE;
	else eInx = -1;

	ASSERT(eInx>=EFIRST && eInx<=ELAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(eInx>=EFIRST && eInx<=ELAST)) return eInx;	// -1

	return eInx;
}

void CGEllipsoid::SetEllipsoid(int eInx)
{
	ASSERT(eInx>=EFIRST && eInx<=ELAST);
	this->m_Name = "";
	this->m_a = __ellipsoid__[eInx].semiMajor;
	this->m_rf = __ellipsoid__[eInx].rcpFlat;

	switch (eInx) {
//	case EAAY : m_Name = "Airy 1830";			break;
	case EBES : m_Name = "Bessel1841";			break;
	case ECLK : m_Name = "Clarke1866";			break;
	case ECLJ : m_Name = "Clarke1880";			break;
	case EEVE : m_Name = "Everest1830";			break;
//	case EFIS : m_Name = "Fischer1960(Mercury)";	break;
//	case EFIT : m_Name = "Fischer1968";			break;
	case EGRS : m_Name = "GRS1967";				break;
//	case EGRT : m_Name = "GRS1975";				break;
	case EGRU : m_Name = "GRS1980";				break;
//	case EHGH : m_Name = "Hough1956";			break;
	case EINT : m_Name = "International";		break;
//	case EKRA : m_Name = "Krassovsky1940";		break;
//	case ESUA : m_Name = "South American1969";	break;
//	case EWGA : m_Name = "WGS60";				break;
//	case EWGB : m_Name = "WGS66";				break;
	case EWGC : m_Name = "WGS72";				break;
	case EWGE : m_Name = "WGS84";				break;
	}
}

CString CGEllipsoid::GetName() const
{	
	if (this->m_Name.IsEmpty()) return _T("");
	return this->m_Name;
}

double CGEllipsoid::GetSemiMajorAxis() const
{
	if (this->m_Name.IsEmpty()) return 0.0;
	return this->m_a;
}

// b = a * (1 - f) = a * (1 - (1/rf))
double CGEllipsoid::GetSemiMinorAxis() const
{
	if (this->m_Name.IsEmpty()) return 0.0;
	double b;
	b = this->m_a * (1.0 - (1.0 / this->m_rf));
	return b;
}

// f = (a-b)/a = 1/rf
double CGEllipsoid::GetFlattening() const
{
	if (this->m_Name.IsEmpty()) return 0.0;
	return (1.0 / this->m_rf);
}

// rf = 1/f
double CGEllipsoid::GetReciprocalFlattening() const
{
	if (this->m_Name.IsEmpty()) return 0.0;
	return this->m_rf;
}

// e = sqrt(e**2)
double CGEllipsoid::GetFirstEccentricity() const
{
	if (this->m_Name.IsEmpty()) return 0.0;

	double eSquared;
	eSquared = GetFirstEccentricitySquared();
	return ::sqrt(eSquared);
}

// e**2 = 2f - f**2 = (a**2 - b**2)/a**2
double CGEllipsoid::GetFirstEccentricitySquared() const
{
	if (this->m_Name.IsEmpty()) return 0.0;

	double eSquared;
	eSquared = (2.0 / this->m_rf) - (1.0 / (this->m_rf * this->m_rf));
	return eSquared;
}

// e' = e * (1 - e**2)**(-1/2)
double CGEllipsoid::GetSecondEccentricity() const
{
	if (this->m_Name.IsEmpty()) return 0.0;

	double ep;
	double e = this->GetFirstEccentricity();
	double eSquared = this->GetFirstEccentricitySquared();
	ep = ::sqrt(1.0 - eSquared);
	ep = 1.0 / ep;
	ep = e * ep;
	return ep;
}

// e'**2 = e' * e'
double CGEllipsoid::GetSecondEccentricitySquared() const
{
	if (this->m_Name.IsEmpty()) return 0.0;

	double epSquared;
	double ep = GetSecondEccentricity();
	epSquared = ep * ep;
	return epSquared;
}

// Mean Radius : R1, R2, R3
// 1. arithmetic mean(R1) :
//    R1 = (a + a + b) / 3 = a * (1 - (f / 3)) = a * (1 - (1.0 / (3 * rf)))
// 2. radius of sphere(R2) having the same surface area as the Ellipsoid :
//    R2 = (1 / 2) * a * [2 + ((1-e**2)/e) * ln((1+e)/(1-e))]**(1/2)
// 3. radius of a sphere(R3) having the same volume as the Ellipsoid :
//    (a**2 * b)**(1/3) = a * (1 - e**2)**(1/6)
double CGEllipsoid::GetMeanRadius(int nMethod) const
{
	if (this->m_Name.IsEmpty()) return 0.0;

	double Radius = 0.0;

	double b = GetSemiMinorAxis();
	double e = GetFirstEccentricity();
	double eSquared = GetFirstEccentricitySquared();
	double temp1, temp2;

	switch (nMethod)
	{
	case R1_ARITHMETIC :	// R1
		Radius = (this->m_a + this->m_a + b) / 3.0;
		break;
	case R2_SAME_SURFACE :	// R2
		temp1 = (1.0 - eSquared) / e;
		temp2 = ::log((1.0 + e) / (1.0 - e));
		Radius = 2.0 + temp1 * temp2;
		Radius = ::sqrt(Radius);
		Radius = Radius * (0.5) * this->m_a;
		break;
	case R3_SAME_VOLUME :	// R3
		temp1 = 1.0 - eSquared;
		temp2 = ::pow(temp1, (1.0/6.0));
		Radius = this->m_a * temp2;
		break;
	}
	return Radius;
}

// GGeoProj.cpp : implementation file
//

#include "pch.h"

#include "GGeoObjStruct.h"
#include "GEllipsoid.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"
#include "GGeoGK.h"
#include "GGeoUPS.h"
#include "GGeoUTM.h"

#include "GStringGeo.h"
#include "GGeoProj.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGeoProj

CGGeoProj::CGGeoProj(int eInx)
{
	m_pEllipsoid = 0;
	this->SetEllipsoid(eInx);	// 타원체의 값을 설정한다.

	m_Base.lat = PRIME_MERIDIAN;// 경도 기본값 : 그리니치 천문대
	m_Base.lon = EQUATOR;		// 위도 기본값 : 적도
	m_ScaleFactor = 1.0;		// 기본값 1.0

	// Lambert Conformal Conic Projection
//	m_StdLat1 = 33.0;
//	m_StdLat2 = 45.0;
	m_StdLat1 = -45.0;
	m_StdLat2 = 45.0;
}

CGGeoProj::~CGGeoProj()
{
}

int CGGeoProj::SetEllipsoid(int eInx)
{
	ASSERT(eInx >= EFIRST && eInx <= ELAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(eInx >= EFIRST && eInx <= ELAST)) return -1;

	int prevEllipsoidInx = this->m_nEllipsoidInx;

	m_pEllipsoid = &__ellipsoid__[eInx];
	this->m_nEllipsoidInx = eInx;

	return prevEllipsoidInx;
}

int CGGeoProj::GetEllipsoid() const
{
	return this->m_nEllipsoidInx;
}

/////////////////////////////////////////////////////////////////////////////
// simple transformations

/////////////////////////////////////////////////////////////////////////////
// CGGeoProj operations
void CGGeoProj::ForwardMER(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	double	toRadian = 3.1415926535 / 180;

	// 지리좌표를 Mercator Projection 에 의한 평면좌표를 계산한다.
	// 계산에 필요한 값을 읽음
	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	CGEllipsoid		ellipsoid(m_nEllipsoidInx);
	
	double	e = ellipsoid.GetFirstEccentricity();
//	double	eP2 = ellipsoid.GetFirstEccentricitySquared();
	double	a = ellipsoid.GetSemiMajorAxis();

	double	x, y;
	x = y = 0.0;
	
	double	baseLat, baseLon, rad_baseLat, rad_baseLon;
	double	lat, lon, rad_lat, rad_lon;
	double	s_lat;
	baseLat = m_Base.lat;		// 기준위도
	baseLon = m_Base.lon;		// 기준경도
	rad_baseLat = baseLat * toRadian;
	rad_baseLon = baseLon * toRadian;

	double e_sin_lat;
	double temp1, temp2, temp3, temp4;

	e_sin_lat = 0.0;
	temp1 = temp2 = temp3 = temp4 = 0.0;

	int	i = 0;
	for(i=0; i<nPoints; i++)
	{
		lat = pGP[i].m_P.lat;			// 위도
		lon = pGP[i].m_P.lon;			// 경도
		rad_lat = lat * toRadian;
		rad_lon = lon * toRadian;
		s_lat = ::sin(rad_lat);

		x = a * ((lon - baseLon) * toRadian);

		e_sin_lat = e * s_lat;
		temp1 = 1.0 + e_sin_lat;
		temp2 = 1.0 - e_sin_lat;
		temp3 = (1.0 + s_lat) / (1.0 - s_lat);
		temp4 = temp2 / temp1;
		temp4 = ::pow(temp4, e);

		y = (a / 2.0) * ::log(temp3 * temp4);

		pPlane[i].x = x;
		pPlane[i].y = y;

		lat = lon = rad_lat = rad_lon = s_lat = e_sin_lat = 0.0;
		temp1 = temp2 = temp3 = temp4 = 0.0;
	} // end for
}

void CGGeoProj::InverseMER(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	double	PI         = 3.1415926535;
	double	NaturalLog = 2.7182818285;
	double	toDecimal	= 180.0 / PI;
	double	toRadian	= PI / 180.0;
	// 계산에 필요한 값을 읽음

	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	CGEllipsoid		ellipsoid(m_nEllipsoidInx);

	double	e = ellipsoid.GetFirstEccentricity();
//	double	eP2 = ellipsoid.GetFirstEccentricitySquared();
//	double	eP4 = eP2 * eP2;
//	double	eP6 = eP4 * eP2;
//	double	eP8 = eP4 * eP4;
	double	a = ellipsoid.GetSemiMajorAxis();

	double	t = 0.0;
	double	x, y;
	double	X, rad_X;
	x = y = X = rad_X = 0.0;

	double	lat, lon, s_lat;
	lat = lon = s_lat = 0.0;

	double	temp1, temp2, temp3, temp4;
	double	tsin1, tsin2, tsin3, tsin4;
	temp1 = temp2 = temp3 = temp4 = 0.0;
	tsin1 = tsin2 = tsin3 = tsin4 = 0.0;
	double	latPre = 0.0;
	double	e_sin_lat = 0.0;

	int i = 0;
	for(i=0; i < nPoints; i++)
	{
		x = pPlane[i].x;
		y = pPlane[i].y;

		t = ::pow(NaturalLog, (-y / a));
		X = 90.0 - (2 * (::atan(t) * toDecimal));

/*		temp1 = (eP2 / 2.0) + (5.0 * eP4 / 24.0) + (eP6 / 12.0) + (13.0 * eP8 / 360.0);
		temp2 = (7.0 * eP4 / 48.0) + (29.0 * eP6 / 240.0) + (811.0 * eP8 / 11520.0);
		temp3 = (7.0 * eP6 / 120.0) + (81.0 * eP8 / 1120.0);
		temp4 = (4279.0 * eP8 / 161280.0);
		tsin1 = 2 * X;
		tsin2 = 4 * X;
		tsin3 = 6 * X;
		tsin4 = 8 * X;
		tsin1 = ::sin(tsin1 * toRadian);
		tsin2 = ::sin(tsin2 * toRadian);
		tsin3 = ::sin(tsin3 * toRadian);
		tsin4 = ::sin(tsin4 * toRadian);
*/
//		lat = X + (temp1 * tsin1) + (temp2 * tsin2) + (temp3 * tsin3) + (temp4 * tsin4);
		lon = (x / a) * toDecimal + m_Base.lon;

		lat  = 90.0 - (2.0 * ::atan(t)) * toDecimal;
		do {
			latPre = lat;

			temp1 = lat * toRadian;
			e_sin_lat = e * ::sin(temp1);

			temp2 = 1.0 - e_sin_lat;
			temp3 = 1.0 + e_sin_lat;
			temp4 = (temp2 / temp3);
			temp4 = ::pow(temp4, (e / 2.0));
			temp4 = t * temp4;

			lat = 2.0 * ::atan(temp4);
			lat *= toDecimal;
			lat = 90.0 - lat;
		} while ( ::fabs(lat - latPre) > 0.00000001);

		pGP[i].m_P.lon = lon;
		pGP[i].m_P.lat = lat;

		x = y = t = X = 0.0;
		temp1 = temp2 = temp3 = temp4 = 0.0;
		tsin1 = tsin2 = tsin3 = tsin4 = 0.0;
	}
}

void CGGeoProj::ForwardLCC(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	double	toRadian = 3.1415926535 / 180;

	// 지리좌표를 Lambert Conrofmal Conic 투영에 의한 평면좌표를 계산한다.
	// 계산에 필요한 값을 읽음
	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다.
	// 표준 위선의 값을 읽어온다.

	double	e2 = m_pEllipsoid->eSquare;
	double	a = m_pEllipsoid->semiMajor;
	double	e = ::sqrt(e2);

	double	rad_lat0, rad_lon0;
	double	rad_lat1, rad_lat2;

	rad_lat0 = m_Base.lat * toRadian;
	rad_lon0 = m_Base.lon * toRadian;
	rad_lat1 = m_StdLat1 * toRadian;
	rad_lat2 = m_StdLat2 * toRadian;

	double	c_lat1, c_lat2, s_lat0, s_lat1, s_lat2, ss_lat1, ss_lat2;
	c_lat1 = ::cos(rad_lat1);
	c_lat2 = ::cos(rad_lat2);
	s_lat0 = ::sin(rad_lat0);
	s_lat1 = ::sin(rad_lat1);
	s_lat2 = ::sin(rad_lat2);
	ss_lat1 = s_lat1 * s_lat1;
	ss_lat2 = s_lat2 * s_lat2;
	
	double	m1, m2;
	// m = cos(lat) / [1 - e**2 * sin**2(lat)]**(1/2)
	m1 = c_lat1 / ::pow((1.0 - e2 * ss_lat1), 0.5);
	m2 = c_lat2 / ::pow((1.0 - e2 * ss_lat2), 0.5);

	double	t0, t1, t2;
	// t = tan(pi/4 - lat/2)/[1 - e*sin(lat))/(1 + e*sin(lat))]**(e/2)
	t0 = ::tan(45.0 * toRadian - rad_lat0 / 2.0) /
		::pow( ((1.0 - e * s_lat0) / (1.0 + e * s_lat0)), (e / 2.0) );
	t1 = ::tan(45.0 * toRadian - rad_lat1 / 2.0) /
		::pow( ((1.0 - e * s_lat1) / (1.0 + e * s_lat1)), (e / 2.0) );
	t2 = ::tan(45.0 * toRadian - rad_lat2 / 2.0) /
		::pow( ((1.0 - e * s_lat2) / (1.0 + e * s_lat2)), (e / 2.0) );

	double	n;
	// n = (ln(m1) - ln(m2)) / (ln(t1) - ln(t2))
	n = (::log(m1) - ::log(m2)) / (::log(t1) - ::log(t2));

	double	F;
	// F = m1 / (n * t1**n)
	F = m1 / (n * ::pow(t1, n));

	double	ro0;
	// ro = a * F * t**n
	ro0 = a * F * ::pow(t0, n);

	double	t;
	double	ro;
	double	seta;

	double	rad_lat, rad_lon;
	double	s_lat;
	int	i;

	for(i = 0; i < nPoints; i++)
	{
		rad_lon = pGP[i].m_P.lon * toRadian;
		rad_lat = pGP[i].m_P.lat * toRadian;

		s_lat = ::sin(rad_lat);
		// t = tan(pi/4 - lat/2)/[1 - e*sin(lat))/(1 + e*sin(lat))]**(e/2)
		t = ::tan(45.0 * toRadian - rad_lat / 2.0) /
			::pow( ((1.0 - e * s_lat) / (1.0 + e * s_lat)), (e / 2.0) );
		// ro = a * F * t**n
		ro = a * F * ::pow(t, n);
		// seta = n * (lon - lon0);
		seta = n * (rad_lon - rad_lon0);
		
		// x = ro * sin(seta)
		pPlane[i].x = ro * ::sin(seta);
		// y = ro0 - ro * cos(seta)
		pPlane[i].y = ro0 - ro * ::cos(seta);
	}
}

void CGGeoProj::InverseLCC(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	double	toRadian = 3.1415926535 / 180.0;
	double	toDegree = 180.0 / 3.1415926535;

	// 지리좌표를 Lambert Conrofmal Conic 투영에 의한 평면좌표를 계산한다.
	// 계산에 필요한 값을 읽음
	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다.
	// 표준 위선의 값을 읽어온다.

	double	e2 = m_pEllipsoid->eSquare;
	double	a = m_pEllipsoid->semiMajor;
	double	e = ::sqrt(e2);

	double	rad_lat0, rad_lon0;
	double	rad_lat1, rad_lat2;

	rad_lat0 = m_Base.lat * toRadian;
	rad_lon0 = m_Base.lon * toRadian;
	rad_lat1 = m_StdLat1 * toRadian;
	rad_lat2 = m_StdLat2 * toRadian;

	double	c_lat1, c_lat2, s_lat0, s_lat1, s_lat2, ss_lat1, ss_lat2;
	c_lat1 = ::cos(rad_lat1);
	c_lat2 = ::cos(rad_lat2);
	s_lat0 = ::sin(rad_lat0);
	s_lat1 = ::sin(rad_lat1);
	s_lat2 = ::sin(rad_lat2);
	ss_lat1 = s_lat1 * s_lat1;
	ss_lat2 = s_lat2 * s_lat2;
	
	double	m1, m2;
	// m = cos(lat) / [1 - e**2 * sin**2(lat)]**(1/2)
	m1 = c_lat1 / ::pow((1.0 - e2 * ss_lat1), 0.5);
	m2 = c_lat2 / ::pow((1.0 - e2 * ss_lat2), 0.5);

	double	t0, t1, t2;
	// t = tan(pi/4 - lat/2)/[1 - e*sin(lat))/(1 + e*sin(lat))]**(e/2)
	t0 = ::tan(45.0 * toRadian - rad_lat0 / 2.0) /
		::pow( ((1.0 - e * s_lat0) / (1.0 + e * s_lat0)), (e / 2.0) );
	t1 = ::tan(45.0 * toRadian - rad_lat1 / 2.0) /
		::pow( ((1.0 - e * s_lat1) / (1.0 + e * s_lat1)), (e / 2.0) );
	t2 = ::tan(45.0 * toRadian - rad_lat2 / 2.0) /
		::pow( ((1.0 - e * s_lat2) / (1.0 + e * s_lat2)), (e / 2.0) );

	double	n;
	// n = (ln(m1) - ln(m2)) / (ln(t1) - ln(t2))
	n = (::log(m1) - ::log(m2)) / (::log(t1) - ::log(t2));

	double	F;
	// F = m1 / (n * t1**n)
	F = m1 / (n * ::pow(t1, n));

	double	ro0;
	// ro = a * F * t**n
	ro0 = a * F * ::pow(t0, n);

	double	t;
	double	ro;
	double	seta;

	double	x, y;
	int	i;
	double tempLat1, tempLat2, tempLat3, diffLat;

	for(i = 0; i < nPoints; i++)
	{
		x = pPlane[i].x;
		y = pPlane[i].y;

		// ro = +-(x**2 + (ro0 - y)**2)**(1/2)	// n값을 부호를 취한다.
		ro = ::pow((x * x + (ro0 - y) * (ro0 - y)), 0.5);
		if (n < 0) ro *= -1;

		// seta = arctan[x / (ro0 - y)]
		seta = ::atan(x / (ro0 - y)) * toDegree;

		// t = (ro / (a * F)**(1/n)
		t = ::pow((ro / (a * F)), (1.0 / n));

		// lon = seta / n + lat0;
		pGP[i].m_P.lon = ((seta / n) + m_Base.lon);
		
		// lat = pi / 2 - 2 * arctan{t * [(1 - e * sin(lat)/(1 + e * sin(lat)]**(e/2)}
		tempLat1 = 90.0 - 2.0 * ::atan(t) * toDegree;
		double rad_tempLat1;
		do {
			rad_tempLat1 = tempLat1 * toRadian;
			tempLat2 = ::pow((1.0 - e * ::sin(rad_tempLat1)) / (1.0 + e * ::sin(rad_tempLat1)), (e/2.0));
			tempLat3 = 90.0 - 2.0 * ::atan(t * tempLat2) * toDegree;
			diffLat = ::fabs(tempLat1 - tempLat3);
			tempLat1 = tempLat3;
		} while (diffLat > 0.00000001);
		pGP[i].m_P.lat = tempLat1;
	}
}

// 매핑모드가 지리좌표로 투영을 하지 않는다. 직접 화면좌표로 변환
void CGGeoProj::ForwardGP(POINTPLANE * pPlane, CGPointGP * pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	int	i;

	for(i=0; i<nPoints; i++) {
		pPlane[i].x = pGP[i].m_P.lon;
		pPlane[i].y = pGP[i].m_P.lat;
	}
}

// Transverse Mercator Projection : Forward Projection
void CGGeoProj::ForwardTM(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	double	toRadian = 3.1415926535 / 180;

	// 지리좌표를 TM투영에 의한 평면좌표를 계산한다.
	// 계산에 필요한 값을 읽음
	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.

	double	eSqr = m_pEllipsoid->eSquare;
	double	eSqr2 = eSqr * eSqr;
	double	eSqr3 = eSqr * eSqr2;
	double	semiMajor = m_pEllipsoid->semiMajor;
	double	seSqr = eSqr / ( 1 - eSqr );	// Secondary Eccentricity Square

	double	rad_lat;
	double	s_lat, c_lat, t_lat;
	double	ss_lat, cc_lat;
	double	N, T, T2, C, C2, A, A2, A3, A4, A5, A6;
	double	numerator;
	//double	baseLat = m_Base.lat;
	double	baseLon = m_Base.lon;
	double	temp1, temp2;

	// y 값을 계산하는데 사용하는 변수
	// M  : 적도(Equator)에서 위도(lat)까지 자오선상에서의 거리
	// M0 : 표준경선(central meridian)을 가로지르는 origin의 위도(Transverse Mercator 두영법은 적도)에서의 거리 
	double	M, M0;
	double	t1, t2, t3, t4;
	double	rad_latBase = m_Base.lat * toRadian;

	int	i;
	double	lat, lon;
	for(i=0; i<nPoints; i++) {
		lat = pGP[i].m_P.lat;
		lon = pGP[i].m_P.lon;

		// 계산에 사용되는 경위도의 sine, cosine, tangent 값을 계산
		rad_lat = lat * toRadian;
		s_lat	= sin(rad_lat);
		c_lat	= cos(rad_lat);
		t_lat	= tan(rad_lat);
		ss_lat	= s_lat * s_lat;
		cc_lat	= c_lat * c_lat;

		//
		// 계산에 사용되는 중간값을 계산 
		//

		// N = a / ( 1 - ( e제곱)(sine제곱위도) )의 1/2 power
		numerator = 1 - eSqr * ss_lat;
		N = semiMajor / pow(numerator, 0.5);

		// T = tangent제곱위도
		T = t_lat * t_lat;
		T2 = T * T;

		// C = (e'제곱)(cosine제곱위도)
		C = seSqr * cc_lat;
		C2 = C * C;

		// A = (경도 - 표준경도)(cosine위도)
		A = ( (lon - baseLon) * toRadian ) * c_lat;
		A2 = A * A;
		A3 = A * A2;
		A4 = A * A3;
		A5 = A * A4;
		A6 = A * A5;

		// x 값을 계산
		// x = k0 * N [ A + (1-T+C)A**3/6 + (5-18T+T**2+72C-58e'**2)A**5/120 ]
		temp1 = A + ( 1 - T + C ) * A3 / 6.0 ;
		temp2 = ( 5.0 - 18.0 * T + T2 + 72.0 * C - 58.0 * seSqr ) * A5 / 120.0;
	
		pPlane[i].x = m_ScaleFactor * N * ( temp1 + temp2 );


		//
		// 수열을 사용하여 M 값을 계산 : 수열의 뒷부분은 오차에 대한 의미가
		// 적으므로 생략함
		// M  : 적도(Equator)에서 위도(lat)까지 자오선상에서의 거리
		// M0 : 표준경선(central meridian)을 가로지르는 origin의
		//      위도(Transverse Mercator 두영법은 적도)에서의 거리 
		//
		t1 = ( 1.0 - eSqr/4.0 - 3.0*eSqr2/64.0 - 5*eSqr3/256.0 ) * rad_lat;
		t2 = ( 3.0*eSqr/8.0 + 3.0*eSqr2/32.0 + 45.0*eSqr3/1024.0 ) * sin(rad_lat*2);
		t3 = ( 15.0*eSqr2/256.0 + 45.0*eSqr3/1024.0 ) * sin(rad_lat*4);
		t4 = ( 35.0*eSqr3/3072.0 ) * sin(rad_lat*6);

		M = semiMajor * ( t1 - t2 + t3 - t4 );

		// 위도가 0도(적도)이면 t1, t2, t3, t4 의 값이 0 : 따라서 M0의 값도 0이 된다.
		if ( m_Base.lat == 0.0 ) {// 적도
			M0 = semiMajor * 0.0;
		}
		else {
			t1 = ( 1.0 - eSqr/4.0 - 3.0*eSqr2/64.0 - 5*eSqr3/256.0 ) * rad_latBase;
			t2 = ( 3.0*eSqr/8.0 + 3.0*eSqr2/32.0 + 45.0*eSqr3/1024.0 ) * sin(rad_latBase*2);
			t3 = ( 15.0*eSqr2/256.0 + 45.0*eSqr3/1024.0 ) * sin(rad_latBase*4);
			t4 = ( 35.0*eSqr3/3072.0 ) * sin(rad_latBase*6);

			M0 = semiMajor * ( t1 - t2 + t3 - t4 );
		}

		// y 값을 계산
		// y = k0 [ M-M0 + N(tangent위도)(A**2/2 + (5-T+9C+4C**2)A**4/24 + (61-58T+T**2+600C-330e'**2)A**4/24 ]
		temp1 = A2/2.0 + ( 5.0 - T + 9.0*C + 4.0*C2 ) * A4 / 24.0;
		temp2 = ( 61.0 - 58.0*T + T2 + 600.0*C - 330.0*seSqr) * A6 / 720.0;
		
		pPlane[i].y = m_ScaleFactor * ( M - M0 + N * t_lat * ( temp1 + temp2 ) );
	} // end for
}

// Transverse Mercator Projection : Inverse Projection
void CGGeoProj::InverseTM(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	int i;

	double	PI = 3.1415926535;
	double	toDecimal	= 180.0 / PI;
	double	toRadian	= PI / 180.0;
	// 계산에 필요한 값을 읽음

	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.

	double	baseLon = m_Base.lon, baseLat = m_Base.lat;
	double	rad_latBase = baseLat * toRadian;
	double	t1, t2, t3, t4;
	double	semiMajor = m_pEllipsoid->semiMajor;
	double	eSqr = m_pEllipsoid->eSquare;
	double	seSqr = eSqr / ( 1 - eSqr );	// Secondary Eccentricity Square

	// 중간값을 계산
	// M  : 적도(Equator)에서 위도(arg_latlong.latitude)까지 자오선상에서의 거리
	// M0 : 표준경선(central meridian)을 가로지르는 origin의 위도(Transverse Mercator 두영법은 적도)에서의 거리 
	double	M, M0;
	// 위도가 0도(적도)이면 t1, t2, t3, t4 의 값이 0 : 따라서 M0의 값도 0이 된다.
	if (baseLat==0.0 ) {// 적도
		M0 = 0.0;
	}
	else {
		t1 = ( 1.0 - eSqr/4.0 - 3.0*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3)/256.0 ) * rad_latBase;
		t2 = ( 3.0*eSqr/8.0 + 3.0*pow(eSqr,2.0)/32.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*2);
		t3 = ( 15.0*pow(eSqr,2.0)/256.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*4);
		t4 = ( 35.0*pow(eSqr,3.0)/3072.0 ) * sin(rad_latBase*6);

		M0	= semiMajor * ( t1 - t2 + t3 - t4 );
	}

	for(i=0; i<nPoints; i++) {

		// M = M0 + y / k0
		M	= M0 + pPlane[i].y / m_ScaleFactor;

		// 계산에 사용되는 중간값을 계산 

		double	temp1,temp2, temp3, temp4;
		double	e1;
		// e1 = ( 1 - (1-e**2)**(1/2) ) / ( 1 + (1-e**2)**1/2 )
		temp1	= ( 1 - eSqr );
		temp2	= pow( temp1, 0.5 );
		e1		= ( 1 - temp2 ) / ( 1 + temp2 );

		double	micro, fp_lat;
		// micro = M / [ a * ( 1 - e**2/4 - 3e**4/64 - 5e**6/256 - .... ) ]  : 단위 : radian
		temp1	= 1 - eSqr/4.0 - 3*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3.0)/256.0;
		micro	= M / ( semiMajor * temp1 );
		// "footpoint latitude" or 경위도좌표에 대응되는 y좌표값을 가진 중앙경선에서의 위도 
		// fp_lat = micro + (3e1/2-27e1**3/32+...)sin(2*micro) + (21e1**2/16-55e1**4/32+...)sin(4*micro)
		//                + (151e1**4/96+...)sin(6*micro) + (1097e1**4/512-...)sin(8*micro)+...
		temp1	= (    3.0 * e1          /   2.0 - 27.0 * pow(e1,3.0) / 32.0 ) * sin(2.0*micro);
		temp2	= (   21.0 * pow(e1,2.0) /  16.0 - 55.0 * pow(e1,4.0) / 32.0 ) * sin(4.0*micro);
		temp3	= (  151.0 * pow(e1,3.0) /  96.0 ) * sin(6.0*micro);
		temp4	= ( 1097.0 * pow(e1,4.0) / 512.0 ) * sin(8.0*micro);
		fp_lat	= micro + temp1 + temp2 + temp3 + temp4;	// 단위 : radian
		// 계산에 사용되는 footpoint latitude의 sine, cosine, tangent 값을 계산
		double	sfp_lat, cfp_lat, tfp_lat, ssfp_lat, csfp_lat, tsfp_lat;
		sfp_lat	= sin(fp_lat);
		cfp_lat	= cos(fp_lat);
		tfp_lat	= tan(fp_lat);
		ssfp_lat	= sfp_lat * sfp_lat;
		csfp_lat	= cfp_lat * cfp_lat;
		tsfp_lat	= tfp_lat * tfp_lat;

		double	C1, T1, N1, R1, D;
		// C1 = e'**2 * (cos(fp_lat))**2
		C1	= seSqr * csfp_lat;
		// T1 = (tan(fp_lat))**2
		T1	= tsfp_lat;
		// N1 = a                / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(1/2)
		// R1 = a * ( 1 - e**2 ) / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(3/2)
		temp1	= ( 1 - eSqr * ssfp_lat );
		N1	= semiMajor / pow(temp1, 0.5);
		R1	= semiMajor * ( 1 - eSqr ) / pow(temp1, 1.5);
		// D = x / ( N1 * k0 );
		D	= pPlane[i].x / ( N1 * m_ScaleFactor );

		double	Dp2, Dp3, Dp4, Dp5, Dp6, C1p2, T1p2;
		Dp2		= pow( D, 2.0 );
		Dp3		= pow( D, 3.0 );
		Dp4		= Dp2 * Dp2;
		Dp5		= Dp2 * Dp3;
		Dp6		= Dp3 * Dp3;
		C1p2	= pow( C1, 2.0 );
		T1p2	= pow( T1, 2.0 );

		// 위도계산 
		// geo.latitude		= fp_lat -  ( N1 * tan(fp_lat) / R1 )
		//						             * [ D**2 / 2 - ( 5 + 3T1 + 10C1 - 4C1**2 - 9e'**2) * D**4 / 24
		//						                          + ( 61 + 90T1 + 298C1 + 45T1**2 - 252e'**2 
		//						                                 - 3C1**2) * D**6 / 720 ]
		temp1	= (   N1 * tfp_lat / R1 );
		temp2	= (  Dp2 /  2.0 );
		temp3	= (  5.0 +  3.0*T1 +  10.0*C1 -  4.0*C1p2 -   9.0*seSqr            ) * Dp4 /  24.0;
		temp4	= ( 61.0 + 90.0*T1 + 298.0*C1 + 45.0*T1p2 - 252.0*seSqr - 3.0*C1p2 ) * Dp6 / 720.0;
		pGP[i].m_P.lat = ( fp_lat - temp1 * ( temp2 - temp3 + temp4 ) ) * toDecimal;
		// 경도계산 								 
		// geo.longitude	= 표준경선 + [ D - ( 1 + 2T1 + C1 ) * D**3 / 6 
		//                                   + ( 5 - 2C1 + 28T1 - 3C1**2 + 8e'**2 + 24T1**2 ) * D**5 / 120 ] / cos(sp_lat);
		temp1	= ( 1.0 + 2.0*T1 + C1 ) * Dp3 / 6.0;
		temp2	= ( 5.0 - 2.0*C1 + 28.0*T1 - 3.0*C1p2 + 8.0*seSqr + 24.0*T1p2 ) * Dp5 / 120.0;
		temp3	= ( D - temp1 + temp2 ) / cfp_lat;;
		pGP[i].m_P.lon = baseLon + temp3 * toDecimal;

	}
}

// 매핑모드가 GP이므로 UTM을 지리좌표로 변환한 후에 지리좌표를 평면좌표로 변환한다.
void CGGeoProj::ForwardGP(POINTPLANE * pPlane, CGPointUTM * pUTM, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	// UTM좌표를 지리좌표로 변환한다.
	
	CGPointGP *pGP = new CGPointGP[nPoints];	// 지리좌표를 저장할 메모리를 할당한다.
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// 이전에 설정된 값을 저장
	CGPointGP prevBase = GetProjBase();
	double  prevScaleFactor = GetProjScaleFactor();

	int i;
	for ( i=0; i<nPoints; i++)
	{
		// UTM좌표를 지리좌표로 변환하기 위한 값을 설정한다.
		SetProjBase( ((6.0*(double)(pUTM[i].m_P.zone))-183.0), 0.0 );
		pPlane[i].x = pUTM[i].m_P.x - UTM_FALSE_EASTING;	// False Easting : 500 km 를 빼서 투영한 평면좌표 계산 
		pPlane[i].y = pUTM[i].m_P.y; // False Northing : 10,000 km값은 계산되지 않는다.
		InverseUTM(&pGP[i], &pPlane[i], 1);
	}
	// 이전에 설정된 값으로 되돌린다.
	SetProjBase(prevBase);
	SetProjScaleFactor(prevScaleFactor);

	// 생성된 지리좌표를 화면좌표로 변환한다.
	// 매핑모드가 지리좌표이다.
	ForwardGP(pPlane, pGP, nPoints);
	
	// 지리좌표의 메모리 영역을 해제한다.
	delete[] pGP;
}
//void CGGeoProj::ForwardGP(POINTPLANE * pPlane, CGPointUPS * pUPS, int nPoints)
//{
//}

// 패핑모드가 지리좌표이므로 투영을 하지 않고 LP를 지리좌표로 변환한다.
void CGGeoProj::InverseGP(CGPointGP * pGP, POINTPLANE * pPlane, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	int	i;

	for(i=0; i<nPoints; i++) {
		pGP[i].m_P.lon = pPlane[i].x;
		pGP[i].m_P.lat = pPlane[i].y;
	}
}

// 매핑시 투영을 하지 않은 상태이므로 평면좌표를 지리좌표로 변환한 후에 UTM좌표로 변환한다.
void CGGeoProj::InverseGP(CGPointUTM * pUTM, POINTPLANE * pPlane, int nPoints)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	CGPointGP *pGP = new CGPointGP[nPoints];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// 평면좌표를 지리좌표로 변환한다.
	InverseGP(pGP, pPlane, nPoints);

	// 이전의 값을 저장한다.
	CGPointGP prevBase = GetProjBase();
	double prevScaleFactor = SetProjScaleFactor(0.9996);	// UTM좌표변환시 ScaleFactor

	char utmZone, utmDgn;

	POINTPLANE *pTempPlane = new POINTPLANE[nPoints];
	if (!(pTempPlane))	// 메모리가 할당되지 않음
		return;

	// 지리좌표를 UTM좌표로 변환한다.
	int i;
	for ( i=0; i<nPoints; i++ )
	{
		utmZone = (char) (pGP[i].m_P.lon / 6.0 + 31.0);
		if ( utmZone > 60 ) utmZone = 60;
		if ( utmZone < 1  ) utmZone = 1;
		SetProjBase( ((6.0*(double)(utmZone))-183.0), 0.0 );

		ForwardUTM(&pTempPlane[i], &pGP[i], 1);

		utmDgn = (char) ( (80.0+pGP[i].m_P.lat) / 8.0 );
		utmDgn = (char) ('C' + utmDgn);
		if ( utmDgn >= 'I' ) utmDgn++;	// zone의 위도에서 'I'를 제거
		if ( utmDgn >= 'O' ) utmDgn++;	// zone의 위도에서 'O'를 제거
		if ( utmDgn >= 'Y' ) utmDgn--;	// zone의 위도에서 'Y'를 제거

		pUTM[i].m_P.zone = utmZone;
		pUTM[i].m_P.dgn = utmDgn;
		pUTM[i].m_P.x = pTempPlane[i].x + UTM_FALSE_EASTING;
		pUTM[i].m_P.y = pTempPlane[i].y;
	}

	// 이전의 값을 되돌린다.
	SetProjBase(prevBase);
	SetProjScaleFactor(prevScaleFactor);

	delete[] pGP;
	delete[] pTempPlane;
}

// 매핑모드가 UTM투영이고, UTM좌표로의 변환이므로
// 투영의 원점과 비교하여 같으면 직접 UTM좌표로 계산한다.
// 투영의 원점과 같지 않으면 지리좌표로 변환한 후에 UTM좌표로 변환한다.
void CGGeoProj::InverseUTM(CGPointUTM * pUTM, POINTPLANE * pPlane, int nPoints)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	CGPointGP prevBase = GetProjBase();
	double prevScaleFactor = SetProjScaleFactor(UTM_SCALE_FACTOR);	// UTM좌표변환시 ScaleFactor

	CGPointGP *pTempGP = new CGPointGP[nPoints];
	if (!(pTempGP))	// 메모리가 할당되지 않음
		return;

	// 평면좌표를 지리좌표로 변환한다.
	InverseUTM(pTempGP, pPlane, nPoints);
	// 지리좌표를 UTM좌표로 변환한다.

	char utmZoneCur;
	utmZoneCur = (char) (m_Base.lon / 6.0 + 31.0);
	if ( utmZoneCur > 60 ) utmZoneCur = 60;
	if ( utmZoneCur < 1  ) utmZoneCur = 1;

	POINTPLANE tmpPlane;
	char utmZone, utmDgn;
	int i;
	for ( i=0; i<nPoints; i++ )
	{
		utmZone = (char) (pTempGP[i].m_P.lon / 6.0 + 31.0);
		if ( utmZone > 60 ) utmZone = 60;
		if ( utmZone < 1  ) utmZone = 1;
		
		utmDgn = (char) ( (80.0+pTempGP[i].m_P.lat) / 8.0 );
		utmDgn = (char) ('C' + utmDgn);
		if ( utmDgn >= 'I' ) utmDgn++;	// zone의 위도에서 'I'를 제거
		if ( utmDgn >= 'O' ) utmDgn++;	// zone의 위도에서 'O'를 제거
		if ( utmDgn >= 'Y' ) utmDgn--;	// zone의 위도에서 'Y'를 제거

		pUTM[i].m_P.zone = utmZone;
		pUTM[i].m_P.dgn = utmDgn;

		if ( utmZoneCur == utmZone ) // 구역이 같은 경우
		{
			pUTM[i].m_P.x = pPlane[i].x + UTM_FALSE_EASTING;
			pUTM[i].m_P.y = pPlane[i].y;
		}
		else	// 지리좌표를 UTM좌표로 변환한다.
		{
			SetProjBase( ((6.0*(double)(utmZone))-183.0), 0.0 );
			
			ForwardUTM(&tmpPlane, &pTempGP[i], 1);

			pUTM[i].m_P.x = tmpPlane.x + UTM_FALSE_EASTING;
			pUTM[i].m_P.y = tmpPlane.y;
		}
	}
	SetProjBase( prevBase );
	SetProjScaleFactor(prevScaleFactor);

	delete[] pTempGP;
}

// 매핑모드가 UTM투영을 사용
// 화면좌표를 지리좌표로 변환시 역투영하여 계산한다.
void CGGeoProj::InverseUTM(CGPointGP * pGP, POINTPLANE * pPlane, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	int i;

	double	PI = 3.1415926535;
	double	toDecimal	= 180.0 / PI;
	double	toRadian	= PI / 180.0;
	// 계산에 필요한 값을 읽음

	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.

	double	baseLon = m_Base.lon, baseLat = m_Base.lat;
	double	rad_latBase = baseLat * toRadian;
	double	t1, t2, t3, t4;
	double	semiMajor = m_pEllipsoid->semiMajor;
	double	eSqr = m_pEllipsoid->eSquare;
	double	seSqr = eSqr / ( 1 - eSqr );	// Secondary Eccentricity Square

	// 중간값을 계산
	// M  : 적도(Equator)에서 위도(arg_latlong.latitude)까지 자오선상에서의 거리
	// M0 : 표준경선(central meridian)을 가로지르는 origin의 위도(Transverse Mercator 두영법은 적도)에서의 거리 
	double	M, M0;
	// 위도가 0도(적도)이면 t1, t2, t3, t4 의 값이 0 : 따라서 M0의 값도 0이 된다.
	if (baseLat==0.0 ) {// 적도
		M0 = 0.0;
	}
	else {
		t1 = ( 1.0 - eSqr/4.0 - 3.0*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3)/256.0 ) * rad_latBase;
		t2 = ( 3.0*eSqr/8.0 + 3.0*pow(eSqr,2.0)/32.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*2);
		t3 = ( 15.0*pow(eSqr,2.0)/256.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*4);
		t4 = ( 35.0*pow(eSqr,3.0)/3072.0 ) * sin(rad_latBase*6);

		M0	= semiMajor * ( t1 - t2 + t3 - t4 );
	}

	for(i=0; i<nPoints; i++) {

		// M = M0 + y / k0
		M	= M0 + pPlane[i].y / m_ScaleFactor;

		// 계산에 사용되는 중간값을 계산 

		double	temp1,temp2, temp3, temp4;
		double	e1;
		// e1 = ( 1 - (1-e**2)**(1/2) ) / ( 1 + (1-e**2)**1/2 )
		temp1	= ( 1 - eSqr );
		temp2	= pow( temp1, 0.5 );
		e1		= ( 1 - temp2 ) / ( 1 + temp2 );

		double	micro, fp_lat;
		// micro = M / [ a * ( 1 - e**2/4 - 3e**4/64 - 5e**6/256 - .... ) ]  : 단위 : radian
		temp1	= 1 - eSqr/4.0 - 3*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3.0)/256.0;
		micro	= M / ( semiMajor * temp1 );
		// "footpoint latitude" or 경위도좌표에 대응되는 y좌표값을 가진 중앙경선에서의 위도 
		// fp_lat = micro + (3e1/2-27e1**3/32+...)sin(2*micro) + (21e1**2/16-55e1**4/32+...)sin(4*micro)
		//                + (151e1**4/96+...)sin(6*micro) + (1097e1**4/512-...)sin(8*micro)+...
		temp1	= (    3.0 * e1          /   2.0 - 27.0 * pow(e1,3.0) / 32.0 ) * sin(2.0*micro);
		temp2	= (   21.0 * pow(e1,2.0) /  16.0 - 55.0 * pow(e1,4.0) / 32.0 ) * sin(4.0*micro);
		temp3	= (  151.0 * pow(e1,3.0) /  96.0 ) * sin(6.0*micro);
		temp4	= ( 1097.0 * pow(e1,4.0) / 512.0 ) * sin(8.0*micro);
		fp_lat	= micro + temp1 + temp2 + temp3 + temp4;	// 단위 : radian
		// 계산에 사용되는 footpoint latitude의 sine, cosine, tangent 값을 계산
		double	sfp_lat, cfp_lat, tfp_lat, ssfp_lat, csfp_lat, tsfp_lat;
		sfp_lat	= sin(fp_lat);
		cfp_lat	= cos(fp_lat);
		tfp_lat	= tan(fp_lat);
		ssfp_lat	= sfp_lat * sfp_lat;
		csfp_lat	= cfp_lat * cfp_lat;
		tsfp_lat	= tfp_lat * tfp_lat;

		double	C1, T1, N1, R1, D;
		// C1 = e'**2 * (cos(fp_lat))**2
		C1	= seSqr * csfp_lat;
		// T1 = (tan(fp_lat))**2
		T1	= tsfp_lat;
		// N1 = a                / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(1/2)
		// R1 = a * ( 1 - e**2 ) / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(3/2)
		temp1	= ( 1 - eSqr * ssfp_lat );
		N1	= semiMajor / pow(temp1, 0.5);
		R1	= semiMajor * ( 1 - eSqr ) / pow(temp1, 1.5);
		// D = x / ( N1 * k0 );
		D	= pPlane[i].x / ( N1 * m_ScaleFactor );

		double	Dp2, Dp3, Dp4, Dp5, Dp6, C1p2, T1p2;
		Dp2		= pow( D, 2.0 );
		Dp3		= pow( D, 3.0 );
		Dp4		= Dp2 * Dp2;
		Dp5		= Dp2 * Dp3;
		Dp6		= Dp3 * Dp3;
		C1p2	= pow( C1, 2.0 );
		T1p2	= pow( T1, 2.0 );

		// 위도계산 
		// geo.latitude		= fp_lat -  ( N1 * tan(fp_lat) / R1 )
		//						             * [ D**2 / 2 - ( 5 + 3T1 + 10C1 - 4C1**2 - 9e'**2) * D**4 / 24
		//						                          + ( 61 + 90T1 + 298C1 + 45T1**2 - 252e'**2 
		//						                                 - 3C1**2) * D**6 / 720 ]
		temp1	= (   N1 * tfp_lat / R1 );
		temp2	= (  Dp2 /  2.0 );
		temp3	= (  5.0 +  3.0*T1 +  10.0*C1 -  4.0*C1p2 -   9.0*seSqr            ) * Dp4 /  24.0;
		temp4	= ( 61.0 + 90.0*T1 + 298.0*C1 + 45.0*T1p2 - 252.0*seSqr - 3.0*C1p2 ) * Dp6 / 720.0;
		pGP[i].m_P.lat = ( fp_lat - temp1 * ( temp2 - temp3 + temp4 ) ) * toDecimal;
		// 경도계산 								 
		// geo.longitude	= 표준경선 + [ D - ( 1 + 2T1 + C1 ) * D**3 / 6 
		//                                   + ( 5 - 2C1 + 28T1 - 3C1**2 + 8e'**2 + 24T1**2 ) * D**5 / 120 ] / cos(sp_lat);
		temp1	= ( 1.0 + 2.0*T1 + C1 ) * Dp3 / 6.0;
		temp2	= ( 5.0 - 2.0*C1 + 28.0*T1 - 3.0*C1p2 + 8.0*seSqr + 24.0*T1p2 ) * Dp5 / 120.0;
		temp3	= ( D - temp1 + temp2 ) / cfp_lat;;
		pGP[i].m_P.lon = baseLon + temp3 * toDecimal;

	}
}

// UTM 투영을 사용하여 UTM 좌표를 지리좌표로 변환한다.
void CGGeoProj::InverseUTM(CGPointGP *pGP, CGPointUTM *pUTM, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	// 투영에 관련된 값을 저장한다.
	CGPointGP prevBase = GetProjBase();
	double  prevScaleFactor = GetProjScaleFactor();

	// UTM 투영에 사용되는 Scale Factor 값을 설정한다.
	SetProjScaleFactor(UTM_SCALE_FACTOR);

	int i;

	double	PI = 3.1415926535;
	double	toDecimal	= 180.0 / PI;
	double	toRadian	= PI / 180.0;
	// 계산에 필요한 값을 읽음

	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.


	double	baseLon, baseLat;
	double	rad_latBase;
//	double	t1, t2, t3, t4;

	// UTM 좌표를 평면좌표 값으로 저장하기 위한 값
	POINTPLANE *pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	for ( i = 0; i < nPoints; i++ )
	{
		// False Easting : 500 km 를 빼서 투영한 평면좌표 계산
		pPlane[i].x = pUTM[i].m_P.x - UTM_FALSE_EASTING;
		pPlane[i].y = pUTM[i].m_P.y;
	}
	
	// 타원체의 값은 하나로 한다.
	double	semiMajor = m_pEllipsoid->semiMajor;
	double	eSqr = m_pEllipsoid->eSquare;
	double	seSqr = eSqr / ( 1 - eSqr );	// Secondary Eccentricity Square

	// 중간값을 계산
	// M  : 적도(Equator)에서 위도(arg_latlong.latitude)까지 자오선상에서의 거리
	// M0 : 표준경선(central meridian)을 가로지르는 origin의 위도(Transverse Mercator 투영법은 적도)에서의 거리 
	double	M, M0;
/*	// 위도가 0도(적도)이면 t1, t2, t3, t4 의 값이 0 : 따라서 M0의 값도 0이 된다.
	if (baseLat==0.0 ) {// 적도
		M0 = 0.0;
	}
	else {
		t1 = ( 1.0 - eSqr/4.0 - 3.0*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3)/256.0 ) * rad_latBase;
		t2 = ( 3.0*eSqr/8.0 + 3.0*pow(eSqr,2.0)/32.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*2);
		t3 = ( 15.0*pow(eSqr,2.0)/256.0 + 45.0*pow(eSqr,3)/1024.0 ) * sin(rad_latBase*4);
		t4 = ( 35.0*pow(eSqr,3.0)/3072.0 ) * sin(rad_latBase*6);

		M0	= semiMajor * ( t1 - t2 + t3 - t4 );
	}
*/
	M0 = 0.0;	// UTM 좌표의 원점은 위도가 적도이므로

	for( i = 0; i < nPoints; i++ ) {

		SetProjBase( ((6.0*(double)(pUTM[i].m_P.zone))-183.0), 0.0 );

		baseLon = m_Base.lon;
		baseLat = m_Base.lat;
		rad_latBase = baseLat * toRadian;

		// M = M0 + y / k0
		M	= M0 + pPlane[i].y / m_ScaleFactor;

		// 계산에 사용되는 중간값을 계산 

		double	temp1,temp2, temp3, temp4;
		double	e1;
		// e1 = ( 1 - (1-e**2)**(1/2) ) / ( 1 + (1-e**2)**1/2 )
		temp1	= ( 1 - eSqr );
		temp2	= pow( temp1, 0.5 );
		e1		= ( 1 - temp2 ) / ( 1 + temp2 );

		double	micro, fp_lat;
		// micro = M / [ a * ( 1 - e**2/4 - 3e**4/64 - 5e**6/256 - .... ) ]  : 단위 : radian
		temp1	= 1 - eSqr/4.0 - 3*pow(eSqr,2.0)/64.0 - 5*pow(eSqr,3.0)/256.0;
		micro	= M / ( semiMajor * temp1 );
		// "footpoint latitude" or 경위도좌표에 대응되는 y좌표값을 가진 중앙경선에서의 위도 
		// fp_lat = micro + (3e1/2-27e1**3/32+...)sin(2*micro) + (21e1**2/16-55e1**4/32+...)sin(4*micro)
		//                + (151e1**4/96+...)sin(6*micro) + (1097e1**4/512-...)sin(8*micro)+...
		temp1	= (    3.0 * e1          /   2.0 - 27.0 * pow(e1,3.0) / 32.0 ) * sin(2.0*micro);
		temp2	= (   21.0 * pow(e1,2.0) /  16.0 - 55.0 * pow(e1,4.0) / 32.0 ) * sin(4.0*micro);
		temp3	= (  151.0 * pow(e1,3.0) /  96.0 ) * sin(6.0*micro);
		temp4	= ( 1097.0 * pow(e1,4.0) / 512.0 ) * sin(8.0*micro);
		fp_lat	= micro + temp1 + temp2 + temp3 + temp4;	// 단위 : radian
		// 계산에 사용되는 footpoint latitude의 sine, cosine, tangent 값을 계산
		double	sfp_lat, cfp_lat, tfp_lat, ssfp_lat, csfp_lat, tsfp_lat;
		sfp_lat	= sin(fp_lat);
		cfp_lat	= cos(fp_lat);
		tfp_lat	= tan(fp_lat);
		ssfp_lat	= sfp_lat * sfp_lat;
		csfp_lat	= cfp_lat * cfp_lat;
		tsfp_lat	= tfp_lat * tfp_lat;

		double	C1, T1, N1, R1, D;
		// C1 = e'**2 * (cos(fp_lat))**2
		C1	= seSqr * csfp_lat;
		// T1 = (tan(fp_lat))**2
		T1	= tsfp_lat;
		// N1 = a                / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(1/2)
		// R1 = a * ( 1 - e**2 ) / [ 1 - (e**2) * (sin(fp_lat))**2 ]**(3/2)
		temp1	= ( 1 - eSqr * ssfp_lat );
		N1	= semiMajor / pow(temp1, 0.5);
		R1	= semiMajor * ( 1 - eSqr ) / pow(temp1, 1.5);
		// D = x / ( N1 * k0 );
		D	= pPlane[i].x / ( N1 * m_ScaleFactor );

		double	Dp2, Dp3, Dp4, Dp5, Dp6, C1p2, T1p2;
		Dp2		= pow( D, 2.0 );
		Dp3		= pow( D, 3.0 );
		Dp4		= Dp2 * Dp2;
		Dp5		= Dp2 * Dp3;
		Dp6		= Dp3 * Dp3;
		C1p2	= pow( C1, 2.0 );
		T1p2	= pow( T1, 2.0 );

		// 위도계산 
		// geo.latitude		= fp_lat -  ( N1 * tan(fp_lat) / R1 )
		//						             * [ D**2 / 2 - ( 5 + 3T1 + 10C1 - 4C1**2 - 9e'**2) * D**4 / 24
		//						                          + ( 61 + 90T1 + 298C1 + 45T1**2 - 252e'**2 
		//						                                 - 3C1**2) * D**6 / 720 ]
		temp1	= (   N1 * tfp_lat / R1 );
		temp2	= (  Dp2 /  2.0 );
		temp3	= (  5.0 +  3.0*T1 +  10.0*C1 -  4.0*C1p2 -   9.0*seSqr            ) * Dp4 /  24.0;
		temp4	= ( 61.0 + 90.0*T1 + 298.0*C1 + 45.0*T1p2 - 252.0*seSqr - 3.0*C1p2 ) * Dp6 / 720.0;
		pGP[i].m_P.lat = ( fp_lat - temp1 * ( temp2 - temp3 + temp4 ) ) * toDecimal;
		// 경도계산 								 
		// geo.longitude	= 표준경선 + [ D - ( 1 + 2T1 + C1 ) * D**3 / 6 
		//                                   + ( 5 - 2C1 + 28T1 - 3C1**2 + 8e'**2 + 24T1**2 ) * D**5 / 120 ] / cos(sp_lat);
		temp1	= ( 1.0 + 2.0*T1 + C1 ) * Dp3 / 6.0;
		temp2	= ( 5.0 - 2.0*C1 + 28.0*T1 - 3.0*C1p2 + 8.0*seSqr + 24.0*T1p2 ) * Dp5 / 120.0;
		temp3	= ( D - temp1 + temp2 ) / cfp_lat;;
		pGP[i].m_P.lon = baseLon + temp3 * toDecimal;

	}
	// 이전의 설정값으로 되돌린다.
	SetProjBase( prevBase );
	SetProjScaleFactor(prevScaleFactor);

	delete[] pPlane;
}


// 매핑모드가 UTM투영
// 지리좌표를 UTM투영하여 화면좌표를 계산한다.
void CGGeoProj::ForwardUTM(POINTPLANE * pPlane, CGPointGP * pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	double	toRadian = 3.1415926535 / 180;

	// 지리좌표를 UTM투영에 의한 평면좌표를 계산한다.
	// 계산에 필요한 값을 읽음
	// 타원체의  a, e제곱 값을 읽어온다.
	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.

	double	eSqr = m_pEllipsoid->eSquare;
	double	eSqr2 = eSqr * eSqr;
	double	eSqr3 = eSqr * eSqr2;
	double	semiMajor = m_pEllipsoid->semiMajor;
	double	seSqr = eSqr / ( 1 - eSqr );	// Secondary Eccentricity Square

	double	rad_lat;
	double	s_lat, c_lat, t_lat;
	double	ss_lat, cc_lat;
	double	N, T, T2, C, C2, A, A2, A3, A4, A5, A6;
	double	numerator;
//	double	baseLat = m_Base.lat;
	double	baseLon = m_Base.lon;
	double	temp1, temp2;

	// y 값을 계산하는데 사용하는 변수
	// M  : 적도(Equator)에서 위도(lat)까지 자오선상에서의 거리
	// M0 : 표준경선(central meridian)을 가로지르는 origin의 위도(Transverse Mercator 두영법은 적도)에서의 거리 
	double	M, M0;
	double	t1, t2, t3, t4;
	double	rad_latBase = m_Base.lat * toRadian;

	int	i;
	double	lat, lon;
	for(i=0; i<nPoints; i++) {
		lat = pGP[i].m_P.lat;
		lon = pGP[i].m_P.lon;

		// 계산에 사용되는 경위도의 sine, cosine, tangent 값을 계산
		rad_lat = lat * toRadian;
		s_lat	= sin(rad_lat);
		c_lat	= cos(rad_lat);
		t_lat	= tan(rad_lat);
		ss_lat	= s_lat * s_lat;
		cc_lat	= c_lat * c_lat;

		//
		// 계산에 사용되는 중간값을 계산 
		//

		// N = a / ( 1 - ( e제곱)(sine제곱위도) )의 1/2 power
		numerator = 1 - eSqr * ss_lat;
		N = semiMajor / pow(numerator, 0.5);

		// T = tangent제곱위도
		T = t_lat * t_lat;
		T2 = T * T;

		// C = (e'제곱)(cosine제곱위도)
		C = seSqr * cc_lat;
		C2 = C * C;

		// A = (경도 - 표준경도)(cosine위도)
		A = ( (lon - baseLon) * toRadian ) * c_lat;
		A2 = A * A;
		A3 = A * A2;
		A4 = A * A3;
		A5 = A * A4;
		A6 = A * A5;

		// x 값을 계산
		// x = k0 * N [ A + (1-T+C)A**3/6 + (5-18T+T**2+72C-58e'**2)A**5/120 ]
		temp1 = A + ( 1 - T + C ) * A3 / 6.0 ;
		temp2 = ( 5.0 - 18.0 * T + T2 + 72.0 * C - 58.0 * seSqr ) * A5 / 120.0;
	
		pPlane[i].x = m_ScaleFactor * N * ( temp1 + temp2 );


		//
		// 수열을 사용하여 M 값을 계산 : 수열의 뒷부분은 오차에 대한 의미가
		// 적으므로 생략함
		// M  : 적도(Equator)에서 위도(lat)까지 자오선상에서의 거리
		// M0 : 표준경선(central meridian)을 가로지르는 origin의
		//      위도(Transverse Mercator 두영법은 적도)에서의 거리 
		//
		t1 = ( 1.0 - eSqr/4.0 - 3.0*eSqr2/64.0 - 5*eSqr3/256.0 ) * rad_lat;
		t2 = ( 3.0*eSqr/8.0 + 3.0*eSqr2/32.0 + 45.0*eSqr3/1024.0 ) * sin(rad_lat*2);
		t3 = ( 15.0*eSqr2/256.0 + 45.0*eSqr3/1024.0 ) * sin(rad_lat*4);
		t4 = ( 35.0*eSqr3/3072.0 ) * sin(rad_lat*6);

		M = semiMajor * ( t1 - t2 + t3 - t4 );

		// 위도가 0도(적도)이면 t1, t2, t3, t4 의 값이 0 : 따라서 M0의 값도 0이 된다.
		if ( m_Base.lat == 0.0 ) {// 적도
			M0 = semiMajor * 0.0;
		}
		else {
			t1 = ( 1.0 - eSqr/4.0 - 3.0*eSqr2/64.0 - 5*eSqr3/256.0 ) * rad_latBase;
			t2 = ( 3.0*eSqr/8.0 + 3.0*eSqr2/32.0 + 45.0*eSqr3/1024.0 ) * sin(rad_latBase*2);
			t3 = ( 15.0*eSqr2/256.0 + 45.0*eSqr3/1024.0 ) * sin(rad_latBase*4);
			t4 = ( 35.0*eSqr3/3072.0 ) * sin(rad_latBase*6);

			M0 = semiMajor * ( t1 - t2 + t3 - t4 );
		}

		// y 값을 계산
		// y = k0 [ M-M0 + N(tangent위도)(A**2/2 + (5-T+9C+4C**2)A**4/24 + (61-58T+T**2+600C-330e'**2)A**4/24 ]
		temp1 = A2/2.0 + ( 5.0 - T + 9.0*C + 4.0*C2 ) * A4 / 24.0;
		temp2 = ( 61.0 - 58.0*T + T2 + 600.0*C - 330.0*seSqr) * A6 / 720.0;
		
		pPlane[i].y = m_ScaleFactor * ( M - M0 + N * t_lat * ( temp1 + temp2 ) );
	} // end for
}

// 매핑모드가 UTM투영
// UTM좌표의 구역이 현재의 UTM투영 구역과 같으면 그대로 화면좌표가 된다.
// 그렇지 않으면 지리좌표로 변환한 후에 화면좌표로 변환한다.
void CGGeoProj::ForwardUTM(POINTPLANE * pPlane, CGPointUTM * pUTM, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	// 이전에 설정된 값을 저장
	CGPointGP prevBase = GetProjBase();
//	double  prevScaleFactor = GetProjScaleFactor();

	CGPointGP	gpTemp;
	double utmZoneLon;
	int i;
	for ( i=0; i<nPoints; i++ )
	{
		utmZoneLon = ( 6.0 * (double)( pUTM[i].m_P.zone ) ) - 183.0;
		if ( utmZoneLon == m_Base.lon )	// UTM좌표가 현재 UTM투영과 같은 구역에 있는 경우
		{
			// UTM좌표를 평면좌표로 계산한다.
			pPlane[i].x = pUTM[i].m_P.x - UTM_FALSE_EASTING;	// False Easting : 500 km 를 빼서 투영한 평면좌표 계산 
			pPlane[i].y = pUTM[i].m_P.y; // False Northing : 10,000 km값은 계산되지 않는다.
		}
		else	// UTM좌표가 다른 구역에 존재하는 경우 
		{
			// UTM좌표를 지리좌표로 변환한 후에 다시 현재 투영되는 구역의 평면좌표로 계산한다.
			SetProjBase( utmZoneLon, 0.0 );
			pPlane[i].x = pUTM[i].m_P.x - UTM_FALSE_EASTING;	// False Easting : 500 km 를 빼서 투영한 평면좌표 계산 
			pPlane[i].y = pUTM[i].m_P.y; // False Northing : 10,000 km값은 계산되지 않는다.
			InverseUTM(&gpTemp, &pPlane[i], 1);

			// 이전에 설정된 값으로 설정한다.
			SetProjBase(prevBase);
			// 이전 구역을 기준으로 변경된 지리좌표를 평면좌표로 변환한다.
			ForwardUTM(&pPlane[i], &gpTemp, 1);
		}
	}
}

// UTM 투영을 사용하여 지리좌표를 UTM 좌표로 변환한다.
void CGGeoProj::ForwardUTM(CGPointUTM *pUTM, CGPointGP *pGP, int nPoints, int nZone)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	// 투영에 관련된 값을 저장한다.
	CGPointGP prevBase = GetProjBase();
	double  prevScaleFactor = GetProjScaleFactor();
	
	// UTM 투영에 사용되는 Scale Factor 값을 설정한다.
	SetProjScaleFactor(UTM_SCALE_FACTOR);

	char utmZone, utmDgn;

	POINTPLANE *pTempPlane = new POINTPLANE[nPoints];
	if (!(pTempPlane))	// 메모리가 할당되지 않음
		return;

	// 지리좌표를 UTM좌표로 변환한다.
	int i;
	if ( 1 <= nZone && nZone <= 60 )	// UTM zone이 1 - 60 사이의 값일 경우
	{
		utmZone = (char) nZone;
		SetProjBase( ((6.0 * (double)(utmZone)) - 183.0), 0.0 );

		for ( i = 0; i < nPoints; i++ )
		{
			ForwardUTM(&pTempPlane[i], &pGP[i], 1);

			utmDgn = (char) ( (80.0 + pGP[i].m_P.lat) / 8.0 );
			utmDgn = (char) ('C' + utmDgn);
			if ( utmDgn >= 'I' ) utmDgn++;	// zone의 위도에서 'I'를 제거
			if ( utmDgn >= 'O' ) utmDgn++;	// zone의 위도에서 'O'를 제거
			if ( utmDgn >= 'Y' ) utmDgn--;	// zone의 위도에서 'Y'를 제거

			pUTM[i].m_P.zone = utmZone;
			pUTM[i].m_P.dgn = utmDgn;
			pUTM[i].m_P.x = pTempPlane[i].x + UTM_FALSE_EASTING;
			pUTM[i].m_P.y = pTempPlane[i].y;
		}
	}
	else	// 지리좌푤 값을 가지고 UTM zone 값을 계산할 경우
	{
		for ( i = 0; i < nPoints; i++ )
		{
			utmZone = (char) (pGP[i].m_P.lon / 6.0 + 31.0);
			if ( utmZone > 60 ) utmZone = 60;
			if ( utmZone < 1  ) utmZone = 1;
			SetProjBase( ((6.0 * (double)(utmZone)) - 183.0), 0.0 );

			ForwardUTM(&pTempPlane[i], &pGP[i], 1);

			utmDgn = (char) ( (80.0 + pGP[i].m_P.lat) / 8.0 );
			utmDgn = (char) ('C' + utmDgn);
			if ( utmDgn >= 'I' ) utmDgn++;	// zone의 위도에서 'I'를 제거
			if ( utmDgn >= 'O' ) utmDgn++;	// zone의 위도에서 'O'를 제거
			if ( utmDgn >= 'Y' ) utmDgn--;	// zone의 위도에서 'Y'를 제거

			pUTM[i].m_P.zone = utmZone;
			pUTM[i].m_P.dgn = utmDgn;
			pUTM[i].m_P.x = pTempPlane[i].x + UTM_FALSE_EASTING;
			pUTM[i].m_P.y = pTempPlane[i].y;
		}
	}

	delete[] pTempPlane;

	// 이전의 설정값으로 되돌린다.
	SetProjBase( prevBase );
	SetProjScaleFactor(prevScaleFactor);
}

// GK 투영을 사용한 경우 
void CGGeoProj::ForwardGK(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints)
{
	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

//	int i = 0;
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	double prevScaleFactor = this->GetProjScaleFactor();

	// GK 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(GK_SCALE_FACTOR);
	// 투영의 원점은 설정하지 않는다. 이전에 설정되어 있어야 한다.

	// 평면좌표 ==> GK의 평면좌표
	this->ForwardTM(pPlane, pGP, nPoints);

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjScaleFactor(prevScaleFactor);
}

void CGGeoProj::InverseGK(CGPointGP *pGP, POINTPLANE *pPlane, int nPoints)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pPlane);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPlane)) return;

//	int i = 0;
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	double prevScaleFactor = this->GetProjScaleFactor();

	// GK 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(GK_SCALE_FACTOR);
	// 투영의 원점은 설정하지 않는다. 이전에 설정되어 있어야 한다.

	// GK의 평면좌표 ==> 평면좌표
	this->InverseTM(pGP, pPlane, nPoints);

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjScaleFactor(prevScaleFactor);
}

// GK 좌표를 지리좌표로 변환
void CGGeoProj::ForwardGK(CGPointGK *pGK, CGPointGP *pGP, int nPoints, int nZone)
{
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	CGPointGP prevBaseGP = this->GetProjBase();
	double prevScaleFactor = this->GetProjScaleFactor();

	// GK 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(GK_SCALE_FACTOR);
	// 투영의 원점은 설정하지 않는다. 이전에 설정되어 있어야 한다.

	POINTPLANE *pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	// 지리좌표 ==> GK의 평면좌표 ==> CGPointGK 좌표

	int i, utmZone;
	if ( 1 <= nZone && nZone <= 60 )	// GK zone이 1 - 60 사이의 값일 경우
	{	// 특정 zone의 GK 좌표로 변환
		utmZone = nZone + 60;
		if ( utmZone > 60 ) utmZone -= 30;
		this->SetProjBase(utmZone);	// 특정 zone의 투영 원점을 설정한다.

		for ( i = 0; i < nPoints; i++ )
		{
			// 지리좌표 ==> GK의 평면좌료
			this->ForwardTM(&pPlane[i], &pGP[i], 1);

			// GK 의 평면좌표 ==> CGPointGK 좌표
			pGK[i].m_P.zone = (char) nZone;	// GK 의 zone
			pGK[i].m_P.x = pPlane[i].x + GK_FALSE_EASTING;
			pGK[i].m_P.y = pPlane[i].y;
		}
	}
	else
	{	// 지리좌표를 보고 투영의 원점을 설정한다.
		for ( i = 0; i < nPoints; i++ )
		{
			// 각 지리좌표로 투영할 원점을 설정한다.
			this->SetProjBase(this->GetUtmZone(pGP[i]));
			
			// 평면좌표 ==> GK의 평면좌료
			this->ForwardTM(&pPlane[i], &pGP[i], 1);

			pGK[i].m_P.zone = (char) (this->GetUtmZone(pGP[i]) - 30);
			if ( pGK[i].m_P.zone < 0 ) pGK[i].m_P.zone += 60;	// 서경
			pGK[i].m_P.x = pPlane[i].x + GK_FALSE_EASTING;
			pGK[i].m_P.y = pPlane[i].y;
		}
	}

	delete[] pPlane;

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjBase(prevBaseGP);
	this->SetProjScaleFactor(prevScaleFactor);
}

// 지리좌표를 GK 좌표로 변환
void CGGeoProj::InverseGK(CGPointGP *pGP, CGPointGK *pGK, int nPoints)
{
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	CGPointGP prevBaseGP = this->GetProjBase();
	double prevScaleFactor = this->GetProjScaleFactor();

	// GK 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(GK_SCALE_FACTOR);
	// 투영의 원점은 설정하지 않는다. 이전에 설정되어 있어야 한다.

	POINTPLANE *pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	int utmZone;

	for ( int i = 0; i < nPoints; i++ )
	{
//		utmZone = pGK[i].m_P.zone + 60;
		utmZone = pGK[i].m_P.zone;
		if ( utmZone > 30 ) utmZone -= 30;
		else
			utmZone += 30;

		// 각 지리좌표를 투영할 원점을 설정한다.
		this->SetProjBase(utmZone);
		
		// CGPointGK 좌표 ==> GK의 평면좌표
		pPlane[i].x = pGK[i].m_P.x - GK_FALSE_EASTING;
		pPlane[i].y = pGK[i].m_P.y;

		// GK의 평면좌표 ==> 지리좌표
		this->InverseTM(&pGP[i], &pPlane[i], 1);
	}

	delete[] pPlane;

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjBase(prevBaseGP);
	this->SetProjScaleFactor(prevScaleFactor);
}

//void CGGeoProj::ForwardUTM(POINTPLANE * pPlane, CGPointUPS * pUPS, int nPoints)
//{
	// UPS좌표를 지리좌표로 변환한 후에 ForwarsUTM함수를 부른다.
//}

void CGGeoProj:: ForwardUPS(POINTPLANE *pPlane, CGPointGP *pGP, int nPoints)
{
	double	PI = 3.1415926535;
	double	toRadian	= PI / 180;
	// 계산에 필요한 값을 읽음

	// 타원체의 origin의 지리좌표 값을 읽어온다. 위도는 90N 또는 90S 경도는 0E
	CGPointGP	lOrigin	= this->GetProjBase();
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.
	double ScaleFactor = this->m_ScaleFactor;	// UPS : 0.994

	// 타원체에서 사용되는 값을 계산한다.
	double t, ro;
	double	SemiMajor, FirstEccent;
	// 타원체의 장축
	SemiMajor = this->m_pEllipsoid->semiMajor;
	// First Eccentricity 값을 계산
	FirstEccent	= ::sqrt( this->m_pEllipsoid->eSquare );

	// 중간값을 계산
	BOOL north;
	double tmpLon, tmpLat;
	double rad_lat, rad_tan_value, rad_diff_lon;
	double s_lat, t_value, s_diff_lon, c_diff_lon;;
	double temp1, temp2, temp3, temp4, temp5;

	int i;
	for ( i = 0; i < nPoints; i++ )
	{
		if ( lOrigin.m_P.lat > 0.0 )
		{
			north = TRUE;
			tmpLon = pGP[i].m_P.lon;	//arg_latlong.longitude;
			tmpLat = pGP[i].m_P.lat;	//arg_latlong.latitude;
		}
		else
		{
			north = FALSE;
			tmpLon =  -pGP[i].m_P.lon;
			tmpLat =  -pGP[i].m_P.lat;
			lOrigin.m_P.lon = -lOrigin.m_P.lon;
		}

		// 계산에 사용되는 위도의 sine 값을 계산
		rad_lat	= tmpLat * toRadian;
		rad_tan_value = ( 45.0 - (tmpLat/2.0)  ) * toRadian;
		rad_diff_lon  = ( tmpLon - lOrigin.m_P.lon ) * toRadian;
		
		s_lat	= sin(rad_lat);
		t_value = tan(rad_tan_value);
		s_diff_lon = sin(rad_diff_lon);
		c_diff_lon = cos(rad_diff_lon);

		temp1 = 1 - ( FirstEccent * s_lat );
		temp2 = 1 + ( FirstEccent * s_lat );
		temp3 = temp1 / temp2;
		temp4 = pow( temp3, (FirstEccent/2) );

		t = t_value / temp4;

		// 초기화
		temp1 = temp2 = temp3 = temp4 = temp5= 0.0;

		temp1 = 1 + FirstEccent;
		temp2 = 1 - FirstEccent;
		temp3 = pow( temp1, temp1 );
		temp4 = pow( temp2, temp2 );
		temp5 = temp3 * temp4;
		temp5 = pow( temp5, 0.5 );

		ro = 2 * SemiMajor * ScaleFactor * t / temp5;

		pPlane[i].x = ro * s_diff_lon;
		pPlane[i].y = (-1.0) * ro * c_diff_lon;

		if ( north != TRUE ) 
		{
			pPlane[i].x *= -1;
			pPlane[i].y *= -1;
		}
	}
}

void CGGeoProj::InverseUPS(CGPointGP *pGP, POINTPLANE * pPlane, int nPoints)
{
	double tmpLon, tmpLat;

	double	PI = 3.1415926535;
	double	toRadian	= PI / 180;
	// 계산에 필요한 값을 읽음

	// 타원체의 origin의 경위도 값을 읽어온다. 위도는 적도이며, 경도는 표분경선
	CGPointGP	lOrigin	= GetProjBase();
	// Transverse Mercator 투영법에서의 scale factor 값을 읽어온다.
	double ScaleFactor = this->m_ScaleFactor; // UPS : 0.994

	// 타원체의 값을 계산한다.
	double a = this->m_pEllipsoid->semiMajor;
	double FirstEccent = ::sqrt(this->m_pEllipsoid->eSquare);
	double temp1, temp2, temp3, temp4, temp5;

	// 중간값을 계산

	POINTPLANE tmpPlane;
	BOOL north;

	double ro, t;

	int i;
	for ( i = 0; i < nPoints; i++ )
	{
		if ( lOrigin.m_P.lat > 0.0 )
		{
			north = TRUE;
			tmpPlane.x = pPlane[i].x;
			tmpPlane.y = pPlane[i].y;
		}
		else
		{
			north = FALSE;
			tmpPlane.x = -pPlane[i].x;
			tmpPlane.y = -pPlane[i].y;
			lOrigin.m_P.lon = -lOrigin.m_P.lon;
		}

		// ro = ( x**2 + y**2 ) **(1/2)
		ro = ( tmpPlane.x * tmpPlane.x ) + ( tmpPlane.y * tmpPlane.y );
		ro = pow( ro, 0.5 );

		// t = ro * { [(1+e)**(1+e)]*[(1-e)**(1-e)] }**(1/2) / ( 2 * a * k0 )
		temp1 = 1 + FirstEccent;
		temp2 = 1 - FirstEccent;
		temp3 = pow( temp1, temp1 );
		temp4 = pow( temp2, temp2 );
		temp5 = temp3 * temp4;
		temp5 = pow( temp5, 0.5 );
		t = ro * temp5 / ( 2 * a * ScaleFactor );

		// 초기화
		temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;

		// longitude = longitude0 + arctan[ x / (-y) ]
		temp1 = tmpPlane.y * (-1);	
		temp2 = tmpPlane.x / temp1;
		temp3 = atan( temp2 ) / toRadian;
		tmpLon = lOrigin.m_P.lon + temp3;
		if ( north != TRUE )
		{
			if ( temp1 < 0 ) // arctan의 분모 값이 음수인 경우
				tmpLon = (-1) * ( tmpLon + 180.0 );
			else tmpLon *= (-1);
		}
		else
		{
			if ( temp1 < 0.0 ) // arctan의 분모 값이 음수인 경우
				tmpLon = ( tmpLon + 180.0 );
		}
		if ( -180.0 > tmpLon )	// 경도가 -180.0도 보다 작은 경우
			tmpLon += 360.0;
		if ( 180.0 < tmpLon )	// 경도가 180.0도 보다 큰 경우
			tmpLon -= 360.0;
		
		// latitude = pi/2 - 2 arctan{ t * [ (1-e*sin(latitude)) / (1+e*sin(latitude)) ]**(1/2) }
		tmpLat = 90.0 - 2.0 * ( atan( t ) / toRadian );
		for ( int j=0; j<=2; j++ )
		{
			temp1 = sin( tmpLat * toRadian );
			temp2 = 1 - ( FirstEccent * temp1 );
			temp3 = 1 + ( FirstEccent * temp1 );
			temp4 = temp2 / temp3;
			temp5 = t * ( pow( temp4, (FirstEccent/2) ) );
			tmpLat = 90.0 - 2.0 * ( atan( temp5 ) / toRadian );

			temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;
		}

		if ( north != TRUE )
		{
			tmpLat *= -1;
		}
		pGP[i].m_P.lon = tmpLon;
		pGP[i].m_P.lat = tmpLat;
	}
}

void CGGeoProj::ForwardUPS(CGPointUPS *pUPS, CGPointGP *pGP, int nPoints)
{
	int i = 0;
	BOOL flag;	// 투영의 원점을 결정하는 변수
	BOOL bSame = TRUE;	// 모든 좌표의 투영원점이 같은가를 검사

	if ( pGP[0].m_P.lat >= EQUATOR ) flag = TRUE;	// 북반구
	else flag = FALSE;	// 남반구

	for ( i = 1; i < nPoints && bSame; i++ )
	{
		switch ( flag )
		{
		case TRUE :	// 북반구
			if ( pGP[i].m_P.lat <  EQUATOR ) bSame = FALSE;
			break;
		case FALSE:	// 남반구
			if ( pGP[i].m_P.lat >= EQUATOR ) bSame = FALSE;
			break;
		}
	}
	if ( !bSame ) return;
		
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	CGPointGP prevBaseGP = this->GetProjBase();
	double prevScaleFactor = this->GetProjScaleFactor();

	CGPointGP Npolar(PRIME_MERIDIAN, NORTH_POLE);	// 북극
	CGPointGP Spolar(PRIME_MERIDIAN, SOUTH_POLE);	// 남극

	// UPS 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(UPS_SCALE_FACTOR);
	// UPS 투영의 원점을 설정한다.
	if ( flag ) // 북반구
		this->SetProjBase(&Npolar);
	else		// 남반구
		this->SetProjBase(&Spolar);
	
	POINTPLANE *pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	// 지리좌표 ==> 평면좌표 : UPS 투영
	this->ForwardUPS(pPlane, pGP, nPoints);

	// 평면좌표 ==> UPS 좌표 : False Easting, False Northing
	for ( i = 0; i < nPoints; i++ )
	{
		if ( pGP[i].m_P.lat > 0.0 )
			pUPS[i].m_P.hemisphere = NORTH;	// 북반구
		else
			pUPS[i].m_P.hemisphere = SOUTH;	// 남반구

		pUPS[i].m_P.x = pPlane[i].x + UPS_FALSE_EASTING;
		pUPS[i].m_P.y = pPlane[i].y + UPS_FALSE_NORTHING;
	}

	delete[] pPlane;

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjBase(&prevBaseGP);
	this->SetProjScaleFactor(prevScaleFactor);
}

void CGGeoProj::InverseUPS(CGPointGP *pGP, CGPointUPS *pUPS, int nPoints)
{
	int i = 0;
	BOOL flag;	// 투영의 원점을 결정하는 변수
	BOOL bSame = TRUE;	// 모든 좌표의 투영원점이 같은가를 검사

	if ( pUPS[0].m_P.hemisphere == NORTH ) flag = TRUE;	// 북반구
	else flag = FALSE;	// 남반구

	for ( i = 1; i < nPoints && bSame; i++ )
	{
		switch ( flag )
		{
		case TRUE :	// 북반구
			if ( pUPS[i].m_P.hemisphere != NORTH ) bSame = FALSE;
			break;
		case FALSE:	// 남반구
			if ( pUPS[i].m_P.hemisphere != SOUTH ) bSame = FALSE;
			break;
		}
	}
	if ( !bSame ) return;
		
	// 현재 설정되어 있는 투영 파라미터를 저장한다.
	CGPointGP prevBaseGP = this->GetProjBase();
	double prevScaleFactor = this->GetProjScaleFactor();

	CGPointGP Npolar(PRIME_MERIDIAN, NORTH_POLE);	// 북극
	CGPointGP Spolar(PRIME_MERIDIAN, SOUTH_POLE);	// 남극

	// UPS 투영법의 Scale Factor 값을 설정한다.
	this->SetProjScaleFactor(UPS_SCALE_FACTOR);
	// UPS 투영의 원점을 설정한다.
	if ( flag ) // 북반구
		this->SetProjBase(&Npolar);
	else		// 남반구
		this->SetProjBase(&Spolar);

	POINTPLANE *pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	// UPS 좌표 ==> 평면좌표 : False Easting, False Northing
	for ( i = 0; i < nPoints; i++ )
	{
		pPlane[i].x = pUPS[i].m_P.x - UPS_FALSE_EASTING;
		pPlane[i].y = pUPS[i].m_P.y - UPS_FALSE_NORTHING;
	}

	// 평면좌표 ==> 지리좌표
	this->InverseUPS(pGP, pPlane, nPoints);

	delete[] pPlane;

	// 이전의 투영법 파라미터 값으로 되돌린다.
	this->SetProjBase(&prevBaseGP);
	this->SetProjScaleFactor(prevScaleFactor);
}

CGPointGP CGGeoProj::SetProjBase(double lonBase, double latBase)
{
	CGPointGP prevBase;
	prevBase.m_P.lon = m_Base.lon;
	prevBase.m_P.lat = m_Base.lat;

	m_Base.lon = lonBase;	m_Base.lat = latBase;

	return prevBase;
}

CGPointGP CGGeoProj::SetProjBase(CGPointGP gp)
{
	return SetProjBase(gp.m_P.lon, gp.m_P.lat);
}

CGPointGP CGGeoProj::SetProjBase(POINTGP gpBase)
{
	return SetProjBase(gpBase.lon, gpBase.lat);
}

CGPointGP CGGeoProj::SetProjBase(int nUtmZone)
{
	ASSERT(1 <= nUtmZone && nUtmZone <= 60);

	CGPointGP retPointGP;
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(1 <= nUtmZone && nUtmZone <= 60)) return retPointGP;

	return this->SetProjBase( ((6.0 * (double)(nUtmZone)) - 183.0), 0.0 );
}

int CGGeoProj::GetUtmZone(CGPointGP gp) const
{
	int utmZone;
	utmZone = (int) (gp.m_P.lon / 6.0 + 31.0);
	if ( utmZone > 60 ) utmZone = 60;
	if ( utmZone < 1  ) utmZone = 1;
	
	return utmZone;
}

CGPointGP CGGeoProj::GetProjBase() const
{
	return CGPointGP(this->m_Base);
}

double CGGeoProj::SetProjScaleFactor(double dScaleFactor)
{
	double prevScaleFactor = m_ScaleFactor;
	m_ScaleFactor = dScaleFactor;
	return prevScaleFactor;
}

double CGGeoProj::GetProjScaleFactor() const
{
	return m_ScaleFactor;
}

void CGGeoProj::SetStdParallel(double dStdParallel1, double dStdParallel2)
{
	this->m_StdLat1 = dStdParallel1;
	this->m_StdLat2 = dStdParallel2;
}
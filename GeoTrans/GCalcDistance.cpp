// GCalcDistance.cpp: implementation of the CGCalcDistance class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"
#include "GGeoUTM.h"

#include "GEllipsoid.h"

#include "GCalcDistance.h"
#include <math.h>

#include "GGeoTrans.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGCalcDistance::CGCalcDistance(int eInx)
{
	this->m_Ellipsoid.SetEllipsoid(eInx);	// default WGS84
}

CGCalcDistance::~CGCalcDistance()
{
}

void CGCalcDistance::SetEllipsoid(int eInx)
{
	this->m_Ellipsoid.SetEllipsoid(eInx);
}

double CGCalcDistance::CalcDistance(CGPointGP & gpA, CGPointGP & gpB, int nMethod)
{
	double Distance;
	double	PI = 3.1415926535;
//	double	toDecimal	= 180.0 / PI;
	double	toRadian	= PI / 180.0;

	double radLatA, radLatB;
	radLatA = (gpA.m_P.lat >= 0.0) ? gpA.m_P.lat * toRadian
								   : gpA.m_P.lat * toRadian * (-1);
	radLatB = (gpB.m_P.lat >= 0.0) ? gpB.m_P.lat * toRadian
								   : gpB.m_P.lat * toRadian * (-1);

	double sinLatA, sinLatB, cosLatA, cosLatB;
	sinLatA = ::sin(radLatA);
	cosLatA = ::cos(radLatA);
	sinLatB = ::sin(radLatB);
	cosLatB = ::cos(radLatB);

	if (gpA.m_P.lat < 0.0) sinLatA *= (-1);
	if (gpB.m_P.lat < 0.0) sinLatB *= (-1);

	double difLonAB, radDifLonAB, cosDifLonAB;
	difLonAB = ::fabs(gpA.m_P.lon - gpB.m_P.lon);
	radDifLonAB = difLonAB * toRadian;
	cosDifLonAB = ::cos(radDifLonAB);

	double cosDistValue, temp1, temp2;
	temp1 = sinLatA * sinLatB;
	temp2 = cosLatA * cosLatB * cosDifLonAB;
	cosDistValue = temp1 + temp2;

	double radAngleAB;
	radAngleAB = ::acos(cosDistValue);

	Distance = this->m_Ellipsoid.GetMeanRadius(nMethod) * radAngleAB;
	return Distance;
}

double CGCalcDistance::CalcDistance(CGPointUTM& utmA, CGPointUTM& utmB)
{
	// UTM 좌표로 거리를 계산할 경우는 같은 Zone에서만 계산한다.
	ASSERT(utmA.m_P.zone == utmB.m_P.zone);
	// Release에서 Down되지 않도록 if문으로 처리
	if (utmA.m_P.zone != utmB.m_P.zone) return (double) -1;

	double Distance;
	double diffEastingSquare, diffNorthingSquare;

	diffEastingSquare = utmA.m_P.x - utmB.m_P.x;
	diffEastingSquare *= diffEastingSquare;
	diffNorthingSquare = utmA.m_P.y - utmB.m_P.y;
	diffNorthingSquare *= diffNorthingSquare;

	Distance = ::sqrt(diffEastingSquare + diffNorthingSquare);

	return Distance;
}

double CGCalcDistance::CalcDistance(CGPointGeo *pPointGeoA,
									CGPointGeo *pPointGeoB, int nCalcDist)
{
	ASSERT(pPointGeoA);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPointGeoA))	return (double) -1;

	ASSERT(pPointGeoB);
	if (!(pPointGeoB))	return (double) -1;

	ASSERT(pPointGeoA->m_nPointType == pPointGeoB->m_nPointType);
	if (!(pPointGeoA->m_nPointType == pPointGeoB->m_nPointType))
		return (double) -1;

	ASSERT(pPointGeoA->m_nPointType == GPTYPE_GP ||
		   pPointGeoB->m_nPointType == GPTYPE_UTM);
	if (!(pPointGeoA->m_nPointType == GPTYPE_GP || pPointGeoB->m_nPointType == GPTYPE_UTM))
		return (double) -1;

	double Distance;

	if (nCalcDist == GDIST_ELLIPSOID)	// 타원체 상의 거리 계산
	{
		switch (pPointGeoA->m_nPointType)	// 지리좌표로 거리 계산
		{
		case GPTYPE_GP :
			Distance = this->CalcDistance((CGPointGP*) pPointGeoA,
										  (CGPointGP*) pPointGeoB);
			break;
		case GPTYPE_UTM :	// UTM 좌표를 지리좌표로 변환한 후에 거리를 계산
			{
				CGGeoTrans trans(this->m_Ellipsoid.GetEllipsoid());
				CGPointGP gpA, gpB;
				trans.toPointGP(&gpA, (CGPointUTM*) pPointGeoA);
				trans.toPointGP(&gpB, (CGPointUTM*) pPointGeoB);
				Distance = this->CalcDistance(gpA, gpB);
			}
			break;
		}
	}
	else if (nCalcDist == GDIST_UTM)	// UTM 좌표에 의한 거리 계산
	{
		switch (pPointGeoA->m_nPointType)	// UTM 좌표로 계산
		{
		case GPTYPE_GP :
			{	// 지리좌표를  UTM 좌표로 변환한 후에 계산
				CGGeoTrans trans(this->m_Ellipsoid.GetEllipsoid());
				CGPointUTM utmA, utmB;
				trans.toPointUTM(&utmA, (CGPointGP*) pPointGeoA);
				trans.toPointUTM(&utmB, (CGPointGP*) pPointGeoB);
				Distance = this->CalcDistance(utmA, utmB);
			}
			break;
		case GPTYPE_UTM :
			Distance = this->CalcDistance((CGPointUTM*) pPointGeoA,
										  (CGPointUTM*) pPointGeoB);
			break;
		}
	}
	else Distance = -1.0;

	return Distance;
}

#include "GGeoProj.h"

CGPointGP CGCalcDistance::OffsetDistance(CGPointGP& gp, double x, double y, int nUnit)
{
	CGPointGP retGP;
	CGPointUTM	utm;

	CGGeoProj proj(this->m_Ellipsoid.GetEllipsoid());
	proj.ForwardUTM(&utm, &gp, 1);
	
	utm.Offset(x, y, nUnit);

	proj.InverseUTM(&retGP, &utm, 1);
	
	return retGP;
}

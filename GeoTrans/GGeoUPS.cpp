// GGeoUPS.cpp: implementation of the CGGeoUPS class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"
#include "GGeoAbst.h"
#include "GGeoUPS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPointUPS::CGPointUPS() : CGPointGeo(GPTYPE_UPS)
{
	this->Empty();
}

CGPointUPS::CGPointUPS(double easting, double northing,
					   VERHEMISPHERE hemisphere) : CGPointGeo(GPTYPE_UPS)
{
	this->m_P.hemisphere = hemisphere;
	this->m_P.x = easting;
	this->m_P.y = northing;
}

CGPointUPS::~CGPointUPS()
{
}

void CGPointUPS::Empty()
{
	this->m_P.hemisphere = NORTH;
	this->m_P.x = 0.0;
	this->m_P.y = 0.0;
}

BOOL CGPointUPS::IsEmpty() const
{
	if (this->m_P.hemisphere == NORTH)
		if (this->m_P.x == 0.0)
			if (this->m_P.y == 0.0) return TRUE;

	return FALSE;
}

const CGPointUPS& CGPointUPS::operator = (const CGPointUPS &right)
{
	if(&right != this) {
		this->m_P.hemisphere = right.m_P.hemisphere;
		this->m_P.x = right.m_P.x;
		this->m_P.y = right.m_P.y;
	}
	return	*this;
}

void CGPointUPS::SetPoint(CGPointGeo *pPoint)
{
	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	ASSERT(pPoint->m_nPointType == GPTYPE_UPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint->m_nPointType == GPTYPE_UPS)) return;

	CGPointUPS *pUPS = (CGPointUPS*) pPoint;

	this->m_P.hemisphere = pUPS->m_P.hemisphere;
	this->m_P.x = pUPS->m_P.x;
	this->m_P.y = pUPS->m_P.y;
}

void CGPointUPS::SetPoint(POINTUPS *ups)
{
	this->m_P.hemisphere = ups->hemisphere;
	this->m_P.x = ups->x;
	this->m_P.y = ups->y;
}

void CGPointUPS::SetPoint(double easting, double northing,
						  VERHEMISPHERE hemisphere)
{
	this->m_P.hemisphere = hemisphere;
	this->m_P.x = easting;
	this->m_P.y = northing;
}

BOOL CGPointUPS::operator==(CGPointUPS& right) const
{
	CString thisStrUPS, rightStrUPS;

	thisStrUPS = this->GetString();
	rightStrUPS = right.GetString();

	if ( thisStrUPS == rightStrUPS ) return TRUE;
	else return FALSE;
}

BOOL CGPointUPS::operator!=(CGPointUPS& right) const
{
	CString thisStrUPS, rightStrUPS;

	thisStrUPS = this->GetString();
	rightStrUPS = right.GetString();

	if ( thisStrUPS != rightStrUPS ) return TRUE;
	else return FALSE;
}

CString CGPointUPS::GetString() const
{
	CString upsStr, hemisphereStr;

	// 반올림된 값임(소수 첫째자리)
	upsStr.Format("%06.0f,%07.0f",
				this->m_P.hemisphere, this->m_P.x, this->m_P.y);
	hemisphereStr = (this->m_P.hemisphere == NORTH) ? "NORTH,"
												: "SOUTH,";

	return (hemisphereStr + upsStr);
}

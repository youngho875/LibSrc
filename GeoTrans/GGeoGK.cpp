// GGeoGK.cpp: implementation of the CGGeoGK class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GGeoObjStruct.h"
#include "GGeoAbst.h"
#include "GGeoGK.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPointGK::CGPointGK() : CGPointGeo(GPTYPE_GK)
{
	this->Empty();
}

CGPointGK::CGPointGK(char zone, double easting, double northing,
					 VERHEMISPHERE hemisphere) : CGPointGeo(GPTYPE_GK)
{
	this->SetPoint(zone, easting, northing, hemisphere);
}

CGPointGK::~CGPointGK()
{
}

void CGPointGK::Empty()
{
	this->m_P.zone = 0;
	this->m_P.x = 0.0;
	this->m_P.y = 0.0;
}

BOOL CGPointGK::IsEmpty() const
{
	if (this->m_P.zone == 0)
		if (this->m_P.x == 0.0)
			if (this->m_P.y == 0.0) return TRUE;

	return FALSE;
}

void CGPointGK::SetPoint(CGPointGeo *pPoint)
{
	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	ASSERT(pPoint->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint->m_nPointType)) return;

	CGPointGK *pGK = (CGPointGK*) pPoint;
	this->SetPoint(pGK);
}

void CGPointGK::SetPoint(POINTGK *gk)
{
	this->m_P.zone = gk->zone;
	this->m_P.x = gk->x;
	this->m_P.y = gk->y;
}

void CGPointGK::SetPoint(CGPointGK *pGK)
{
	this->m_P.zone = pGK->m_P.zone;
	this->m_P.x = pGK->m_P.x;
	this->m_P.y = pGK->m_P.y;
}

void CGPointGK::SetPoint(char zone, double easting, double northing,
						 VERHEMISPHERE hemisphere)
{
	this->m_P.zone = zone;
	this->m_P.x = easting;
	if (hemisphere == SOUTH) northing *= -1;
	this->m_P.y = northing;
}

void CGPointGK::Offset(double x, double y, int nUnit)
{
	switch ( nUnit )
	{
	case GUNIT_MM :
		this->m_P.x += (x / 1000.0);
		this->m_P.y += (y / 1000.0);
		break;
	case GUNIT_CM :
		this->m_P.x += (x / 100.0);
		this->m_P.y += (y / 100.0);
		break;
	case GUNIT_M  :
		this->m_P.x += x;
		this->m_P.y += y;
		break;
	case GUNIT_KM :
		this->m_P.x += (x * 1000.0);
		this->m_P.y += (y * 1000.0);
		break;
	}
}

BOOL CGPointGK::operator==(CGPointGK& right) const
{
	CString gkLeftStr, gkRightStr;
	gkLeftStr = this->GetString();
	gkRightStr = right.GetString();

	if ( gkLeftStr != gkRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGPointGK::operator!=(CGPointGK& right) const
{
	CString gkLeftStr, gkRightStr;
	gkLeftStr = this->GetString();
	gkRightStr = right.GetString();

	if ( gkLeftStr == gkRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

const CGPointGK& CGPointGK::operator = (const CGPointGK &right)
{
	if(&right!=this) {
		this->m_P.x = right.m_P.x;
		this->m_P.y = right.m_P.y;
		this->m_P.zone = right.m_P.zone;
	}
	return	*this;
}

CString CGPointGK::GetString() const
{
	double northing;
	char nChar;
	if ( this->m_P.y >= 0.0 ) {	// North
		northing = this->m_P.y;
		nChar = 'N';
	}
	else {	// South
		northing = this->m_P.y + GK_FALSE_NORTHING;
		nChar = 'S';
	}

	CString gkStr;
	gkStr.Format(_T("%07.0f,%02d,%06.0f,%1c"),
				northing, this->m_P.zone, this->m_P.x, nChar);

	return gkStr;
}

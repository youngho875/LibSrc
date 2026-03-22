// GGeoGP.cpp: implementation of the CGGeoGP class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GGeoAbst.h"

#include "GGeoGP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGSizeGP
CGSizeGP::CGSizeGP() : CGSizeGeo(GPTYPE_GP)
{
	m_S.clon = 0.0;
	m_S.clat = 0.0;
}

CGSizeGP::CGSizeGP(double clon, double clat) : CGSizeGeo(GPTYPE_GP)
{
	m_S.clon = clon;
	m_S.clat = clat;
}

CGSizeGP::~CGSizeGP()
{
}

void CGSizeGP::SetSize(double clon, double clat)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	m_S.clon = clon;
	m_S.clat = clat;
}

void CGSizeGP::SetSize(CGSizeGeo *pSize)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CGSizeGP	*pGP = (CGSizeGP *) pSize;
	m_S.clon = pGP->m_S.clon;
	m_S.clat = pGP->m_S.clat;
}

void CGSizeGP::SetSize(SIZEGP *pSize)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	m_S.clon = pSize->clon;
	m_S.clat = pSize->clat;
}

void CGSizeGP::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	CGSizeGP	*pGP = (CGSizeGP *) pOffset;
	m_S.clon += pGP->m_S.clon;
	m_S.clat += pGP->m_S.clat;
}

void CGSizeGP::Offset(SIZEGP *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_S.clon += pOffset->clon;
	m_S.clat += pOffset->clat;
}

const CGSizeGP &CGSizeGP::operator=(const CGSizeGP &right)
{
	if(&right!=this) {
		this->m_S.clon = right.m_S.clon;
		this->m_S.clat = right.m_S.clat;
	}
	return	*this;
}

BOOL CGSizeGP::operator==(CGSizeGP& sizeGP) const
{
	return (this->m_S.clon == sizeGP.m_S.clon &&
			this->m_S.clat == sizeGP.m_S.clat);
}

BOOL CGSizeGP::operator!=(CGSizeGP& sizeGP) const
{
	return (this->m_S.clon != sizeGP.m_S.clon ||
			this->m_S.clat != sizeGP.m_S.clat);
}

void CGSizeGP::operator+=(CGSizeGP sizeGP)
{
	this->m_S.clon += sizeGP.m_S.clon;
	this->m_S.clat += sizeGP.m_S.clat;
}

void CGSizeGP::operator-=(CGSizeGP sizeGP)
{
	this->m_S.clon -= sizeGP.m_S.clon;
	this->m_S.clat -= sizeGP.m_S.clat;
}

CGSizeGP CGSizeGP::operator+(CGSizeGP sizeGP) const
{
	CGSizeGP retSizeGP;
	retSizeGP.m_S.clon = this->m_S.clon + sizeGP.m_S.clon;
	retSizeGP.m_S.clat = this->m_S.clat + sizeGP.m_S.clat;
	return retSizeGP;
}

CGPointGP CGSizeGP::operator+(CGPointGP gp) const
{
	CGPointGP retGP;
	retGP.m_P.lon = this->m_S.clon + gp.m_P.lon;
	retGP.m_P.lat = this->m_S.clat + gp.m_P.lat;
	return retGP;
}

CGRectGP CGSizeGP::operator+(const CGRectGP rectGP) const
{
	CGRectGP retRectGP;
	retRectGP.m_R.left   = retRectGP.m_R.left   + this->m_S.clon;
	retRectGP.m_R.top    = retRectGP.m_R.top    + this->m_S.clat;
	retRectGP.m_R.right  = retRectGP.m_R.right  + this->m_S.clon;
	retRectGP.m_R.bottom = retRectGP.m_R.bottom + this->m_S.clat;
	return retRectGP;
}

CGSizeGP CGSizeGP::operator-(CGSizeGP sizeGP) const
{
	CGSizeGP retSizeGP;
	retSizeGP.m_S.clon = this->m_S.clon - sizeGP.m_S.clon;
	retSizeGP.m_S.clat = this->m_S.clat - sizeGP.m_S.clat;
	return retSizeGP;	
}

CGPointGP CGSizeGP::operator-(CGPointGP gp) const
{
	CGPointGP retGP;
	retGP.m_P.lon = this->m_S.clon - gp.m_P.lon;
	retGP.m_P.lat = this->m_S.clat - gp.m_P.lat;
	return retGP;
}

CGRectGP CGSizeGP::operator-(const CGRectGP rectGP) const
{
	CGRectGP retRectGP;	// CSize::operator- : CRect(lpRect) - *this 참조
	retRectGP.m_R.left   = retRectGP.m_R.left   - this->m_S.clon;
	retRectGP.m_R.top    = retRectGP.m_R.top    - this->m_S.clat;
	retRectGP.m_R.right  = retRectGP.m_R.right  - this->m_S.clon;
	retRectGP.m_R.bottom = retRectGP.m_R.bottom - this->m_S.clat;
	return retRectGP;
}

CGSizeGP CGSizeGP::operator-() const
{
	return CGSizeGP(-this->m_S.clon, -this->m_S.clat);
}

/////////////////////////////////////////////////////////////////////////////
// CGPointGP
CGPointGP::CGPointGP() : CGPointGeo(GPTYPE_GP)
{
	m_P.lon = 999.0;
	m_P.lat = 999.0;
	m_P.ht	= 999.0;
}

CGPointGP::CGPointGP(double lon, double lat, double height) : CGPointGeo(GPTYPE_GP)
{
	this->m_P.lon = lon;
	this->m_P.lat = lat;
	this->m_P.ht  = height;
}

CGPointGP::CGPointGP(CGPointGP *initGP) : CGPointGeo(GPTYPE_GP)
{
	this->m_P.lon = initGP->m_P.lon;
	this->m_P.lat = initGP->m_P.lat;
	this->m_P.ht  = initGP->m_P.ht;
}

CGPointGP::CGPointGP(POINTGP initGP) : CGPointGeo(GPTYPE_GP)
{
	this->m_P.lon = initGP.lon;
	this->m_P.lat = initGP.lat;
	this->m_P.ht  = initGP.ht;
}

CGPointGP::CGPointGP(CGSizeGP initSizeGP) : CGPointGeo(GPTYPE_GP)
{
	this->m_P.lon = initSizeGP.m_S.clon;
	this->m_P.lat = initSizeGP.m_S.clat;
	this->m_P.ht  = 999.0;
}

CGPointGP::CGPointGP(SIZEGP initSizeGP) : CGPointGeo(GPTYPE_GP)
{
	this->m_P.lon = initSizeGP.clon;
	this->m_P.lat = initSizeGP.clat;
	this->m_P.ht  = 999.0;
}

CGPointGP::~CGPointGP()
{
}

void CGPointGP::operator+=(const CGPointGP gp)
{
	this->m_P.lon += gp.m_P.lon;
	this->m_P.lat += gp.m_P.lat;
}

void CGPointGP::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	ASSERT(pOffset->m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset->m_nPointType==GPTYPE_GP)) return;

	CGSizeGP	*pGP = (CGSizeGP *) pOffset;
	m_P.lon += pGP->m_S.clon;
	m_P.lat += pGP->m_S.clat;
};

void CGPointGP::Offset(SIZEGP *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_P.lon += pOffset->clon;
	m_P.lat += pOffset->clat;
};

void CGPointGP::Offset(double lon, double lat, int nUnit)
{
	switch ( nUnit )
	{
	case GUNIT_SEC :
		this->m_P.lon += (lon / 3600.0);
		this->m_P.lat += (lat / 3600.0);
		break;
	case GUNIT_MIN :
		this->m_P.lon += (lon / 60.0);
		this->m_P.lat += (lat / 60.0);
		break;
	case GUNIT_DEG  :
		this->m_P.lon += lon;
		this->m_P.lat += lat;
		break;
	}
};

const CGPointGP &CGPointGP::operator=(const CGPointGP &right)
{
	if(&right!=this) {
		this->m_P.lon = right.m_P.lon;
		this->m_P.lat = right.m_P.lat;
		this->m_P.ht  = right.m_P.ht;
	}
	return	*this;
}

BOOL CGPointGP::operator==(CGPointGP& right) const
{
//	return ( (this->m_P.lon == gp.m_P.lon) && (this->m_P.lat == gp.m_P.lat) );
	CString gpLeftStr, gpRightStr;
	gpLeftStr = this->GetString();
	gpRightStr = right.GetString();

	if ( gpLeftStr != gpRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGPointGP::operator!=(CGPointGP& right) const
{
//	return ( (this->m_P.lon != gp.m_P.lon) || (this->m_P.lat != gp.m_P.lat) );
	CString gpLeftStr, gpRightStr;
	gpLeftStr = this->GetString();
	gpRightStr = right.GetString();

	if ( gpLeftStr == gpRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGPointGP::operator==(POINTGP gp) const
{
	return ( (this->m_P.lon == gp.lon) && (this->m_P.lat == gp.lat) );
}

BOOL CGPointGP::operator!=(POINTGP gp) const
{
	return ( (this->m_P.lon != gp.lon) || (this->m_P.lat != gp.lat) );
}

void CGPointGP::SetPoint(CGPointGeo *pGeo)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGeo)) return;

	ASSERT(pGeo->m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGeo->m_nPointType==GPTYPE_GP)) return;

	CGPointGP	*pGP = (CGPointGP *) pGeo;
	m_P.lon = pGP->m_P.lon;
	m_P.lat = pGP->m_P.lat;
	m_P.ht  = pGP->m_P.ht;
}

void CGPointGP::SetPoint(CGPointGP *pGP)
{
	m_P.lon = pGP->m_P.lon;
	m_P.lat = pGP->m_P.lat;
	m_P.ht  = pGP->m_P.ht;
}

void CGPointGP::SetPoint(POINTGP *pPoint)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	m_P.lon = pPoint->lon;
	m_P.lat = pPoint->lat;
	m_P.ht  = pPoint->ht;
}

void CGPointGP::SetPoint(double lon, double lat, double height)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	m_P.lon = lon;
	m_P.lat = lat;
	m_P.ht  = height;
}

CString CGPointGP::GetString(int nDecPoint) const
{
	CString lonStr, latStr;
	lonStr = this->GetLongitude(nDecPoint);
	latStr = this->GetLatitude(nDecPoint);

	return (lonStr + latStr);
}

CString CGPointGP::GetLongitude(int nDecPoint) const
{
	ASSERT(-1 <= nDecPoint && nDecPoint <= 7);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(-1 <= nDecPoint && nDecPoint <= 7)) return _T("");

	CString lonStr;
	double lon = this->m_P.lon;
	if (lon < 0) lon *= 1;

	lonStr.Format("%012.9f", lon);

	if (nDecPoint == -1)
		lonStr = lonStr.Left(10);
	else 
	{
		if (nDecPoint == 0) lonStr = lonStr.Left(3);
		else lonStr = lonStr.Left(nDecPoint + 4);
	}
	
	if (this->m_P.lon >= 0) lonStr = lonStr + "E";
	else lonStr = lonStr + "W";

	return lonStr;
}

CString CGPointGP::GetLatitude(int nDecPoint) const
{
	ASSERT(-1 <= nDecPoint && nDecPoint <= 7);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(-1 <= nDecPoint && nDecPoint <= 7)) return _T("");

	CString latStr;
	double lat = this->m_P.lat;
	if (lat < 0) lat *= 1;

	latStr.Format("%011.9f", lat);

	if (nDecPoint == -1)
		latStr = latStr.Left(9);
	else 
	{
		if (nDecPoint == 0) latStr = latStr.Left(2);
		else latStr = latStr.Left(nDecPoint + 3);
	}
	
	if (this->m_P.lat >= 0) latStr = latStr + "N";
	else latStr = latStr + "S";

	return latStr;
}

void CGPointGP::SetPoint(CString* str, int nType, int nSeparator)
{
	if (str->IsEmpty()) return;

//	int nLength = str->GetLength();

	CString strLon, strLat;

	strLon = str->Left(11);
	strLat = (str->Right(11)).Left(10);

	this->m_P.lon = ::atof(strLon);
	this->m_P.lat = ::atof(strLat);
	this->m_P.ht = 999.0;

	nSeparator = nSeparator;
	nType = nType;
}

void CGPointGP::GetPoint(CString* ptStr, int nType, int nSeparator)
{
	ptStr->Format("GP:%9.6f, %9.6f", m_P.lon, m_P.lat);

	nSeparator = nSeparator;
	nType = nType;
}

void CGPointGP::Empty()
{
	this->m_P.lon = 999.0;
	this->m_P.lat = 999.0;
	this->m_P.ht  = 999.0;
}

BOOL CGPointGP::IsEmpty() const
{
	if (this->m_P.lon == 999.0)
		if (this->m_P.lat == 999.0)
			if (this->m_P.ht == 999.0) return TRUE;

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CGRectGP
CGRectGP::CGRectGP() : CGRectGeo(GPTYPE_GP)
{
	this->m_R.left   = 0.0;
	this->m_R.top    = 0.0;
	this->m_R.right  = 0.0;
	this->m_R.bottom = 0.0;
	this->m_R.ht	 = 0.0;
}

CGRectGP::CGRectGP(double left, double top, double right, double bottom, double height)
		: CGRectGeo(GPTYPE_GP)
{
	this->m_R.left   = left;
	this->m_R.top    = top;
	this->m_R.right  = right;
	this->m_R.bottom = bottom;
	this->m_R.ht	 = height;
}

CGRectGP::CGRectGP(const CGRectGP& rectGP) : CGRectGeo(GPTYPE_GP)
{
	this->m_R.left   = rectGP.m_R.left;
	this->m_R.top    = rectGP.m_R.top;
	this->m_R.right  = rectGP.m_R.right;
	this->m_R.bottom = rectGP.m_R.bottom;
	this->m_R.ht	 = rectGP.m_R.ht;
}

CGRectGP::CGRectGP(CGPointGP gp, CGSizeGP sizeGP) : CGRectGeo(GPTYPE_GP)
{
	this->m_R.right  = (this->m_R.left = gp.m_P.lon) + sizeGP.m_S.clon;
	this->m_R.bottom = (this->m_R.top  = gp.m_P.lat) + sizeGP.m_S.clat;
	this->m_R.ht	 = 0.0;
}

CGRectGP::CGRectGP(CGPointGP topLeft, CGPointGP bottomRight) : CGRectGeo(GPTYPE_GP)
{
	this->m_R.left   = topLeft.m_P.lon;
	this->m_R.top    = topLeft.m_P.lat;
	this->m_R.right  = bottomRight.m_P.lon;
	this->m_R.bottom = bottomRight.m_P.lat;
	this->m_R.ht	 = 0.0;
}

CGRectGP::~CGRectGP()
{
}

double CGRectGP::Width() const
{
	return m_R.right - this->m_R.left;
}

double CGRectGP::Height() const
{
	return m_R.top - this->m_R.bottom;
}

CGSizeGP CGRectGP::Size() const
{
	// return CSize(right - left, bottom - top);	// CRect의 경우
	return CGSizeGP(this->m_R.right - this->m_R.left,
					this->m_R.top   - this->m_R.bottom);
}

CGPointGP CGRectGP::TopLeft() const
{
	CGPointGP topLeft;
	topLeft.m_P.lon = this->m_R.left;
	topLeft.m_P.lat = this->m_R.top;
	return topLeft;
}

const CGPointGP CGRectGP::TopLeft()
{
	CGPointGP topLeft;
	topLeft.m_P.lon = this->m_R.left;
	topLeft.m_P.lat = this->m_R.top;
	return topLeft;
}

CGPointGP CGRectGP::BottomRight() const
{
	CGPointGP bottomRight;
	bottomRight.m_P.lon = this->m_R.right;
	bottomRight.m_P.lat = this->m_R.bottom;
	return bottomRight;
}

const CGPointGP CGRectGP::BottomRight()
{
	CGPointGP bottomRight;
	bottomRight.m_P.lon = this->m_R.right;
	bottomRight.m_P.lat = this->m_R.bottom;
	return bottomRight;
}

CGPointGP CGRectGP::CenterPoint() const
{
	return CGPointGP((this->m_R.left + this->m_R.right ) / 2.0,
					 (this->m_R.top  + this->m_R.bottom) / 2.0);
}

// 먼저 NormalizedRect()를 호출하고 사용한다.
// Empty인 경우는 Height 또는 Width가 0과 같거나 작은 경우이다.
BOOL CGRectGP::IsRectEmpty() const
{
	if ( this->Width()  <= 0.0 ) return TRUE;
	if ( this->Height() <= 0.0 ) return TRUE;
	return FALSE;
}

BOOL CGRectGP::IsRectNull() const
{
	return (this->m_R.left == 0.0 && this->m_R.right  == 0.0 &&
			this->m_R.top  == 0.0 && this->m_R.bottom == 0);
}

// 먼저 NormalizedRect()를 호출하고 사용한다.
BOOL CGRectGP::PtInRect(CGPointGP& gp) const
{
	if ( ( this->m_R.left   <= gp.m_P.lon && gp.m_P.lon <= this->m_R.right ) &&
		( this->m_R.bottom <= gp.m_P.lat && gp.m_P.lat <= this->m_R.top ) )
	   return TRUE;	// gp가 CGRectGp안에 존재
	else return FALSE;	// right, top은 외부에 존재하는 것으로 본다.
}

BOOL CGRectGP::PtInRectWidth(CGPointGP& gp) const
{
	if ( this->m_R.left   <= gp.m_P.lon && gp.m_P.lon <= this->m_R.right )
	   return TRUE;	// gp가 CGRectGp안에 존재
	else return FALSE;	// right, top은 외부에 존재하는 것으로 본다.
}

BOOL CGRectGP::PtInRectHeight(CGPointGP& gp) const
{
	if ( this->m_R.bottom <= gp.m_P.lat && gp.m_P.lat <= this->m_R.top ) 
	   return TRUE;	// gp가 CGRectGp안에 존재
	else return FALSE;	// right, top은 외부에 존재하는 것으로 본다.
}

void CGRectGP::SetRect(double lon1, double lat1, double lon2, double lat2)
{
	this->m_R.left	 = lon1;
	this->m_R.top	 = lat1;
	this->m_R.right  = lon2;
	this->m_R.bottom = lat2;
}

void CGRectGP::SetRect(CGPointGeo *pTopLeft, CGPointGeo *pBottomRight)
{
	ASSERT(this->m_nPointType == pTopLeft->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(this->m_nPointType == pTopLeft->m_nPointType)) return;

	ASSERT(this->m_nPointType == pBottomRight->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(this->m_nPointType == pBottomRight->m_nPointType)) return;

	CGPointGP *pTopLeftGP = (CGPointGP*) pTopLeft;
	CGPointGP *pBottomRightGP = (CGPointGP*) pBottomRight;

	this->SetRect(pTopLeftGP->m_P.lon, pTopLeftGP->m_P.lat,
				  pBottomRightGP->m_P.lon, pBottomRightGP->m_P.lat);
}

void CGRectGP::SetRect(CGPointGP& topLeft, CGPointGP& bottomRight)
{
	this->SetRect(topLeft.m_P.lon, topLeft.m_P.lat,
				  bottomRight.m_P.lon, bottomRight.m_P.lat);
}

void CGRectGP::SetRectEmpty()
{
	this->m_R.left = this->m_R.top = this->m_R.right = this->m_R.bottom = 0.0;
}

void CGRectGP::CopyRect(CGRectGP& rectGP)
{
	this->SetRect(rectGP.m_R.left,  rectGP.m_R.top,
				  rectGP.m_R.right, rectGP.m_R.bottom);
}

// 먼저 NormalizedRect()를 호출하고 사용한다.
BOOL CGRectGP::EqualRect(const CGRectGP& rectGP) const
{
	if ( this->m_R.left  == rectGP.m_R.left  && this->m_R.top    == rectGP.m_R.top &&
		 this->m_R.right == rectGP.m_R.right && this->m_R.bottom == rectGP.m_R.bottom )
		 return TRUE;
	else return FALSE;
}

void CGRectGP::InflateRect(double lon, double lat)
{
	this->m_R.left	 -= lon;
	this->m_R.top	 += lat;
	this->m_R.right	 += lon;
	this->m_R.bottom -= lat;
}

void CGRectGP::InflateRect(CGSizeGP& sizeGP)
{
	this->m_R.left	 -= sizeGP.m_S.clon;
	this->m_R.top	 += sizeGP.m_S.clat;
	this->m_R.right	 += sizeGP.m_S.clon;
	this->m_R.bottom -= sizeGP.m_S.clat;
}

void CGRectGP::InflateRect(CGRectGP& rectGP)
{
	// CRect의 경우는 top보다 bottom의 값이 크다.
	// left += lpRect->left;
	// top += lpRect->top;
	// right -= lpRect->right;
	// bottom -= lpRect->bottom;
	this->m_R.left	 -= rectGP.m_R.left;
	this->m_R.top	 += rectGP.m_R.top;
	this->m_R.right	 += rectGP.m_R.right;
	this->m_R.bottom -= rectGP.m_R.bottom;
}

void CGRectGP::InflateRect(double l, double t, double r, double b)
{
	// CRect의 경우는 top보다 bottom의 값이 크다.
	// left -= l;
	// top -= t;
	// right += r;
	// bottom += b;
	this->m_R.left   -= l;
	this->m_R.top    += t;
	this->m_R.right  += r;
	this->m_R.bottom -= b;
}

void CGRectGP::DeflateRect(double lon, double lat)
{
	this->m_R.left	 += lon;
	this->m_R.top	 -= lat;
	this->m_R.right	 -= lon;
	this->m_R.bottom += lat;
}

void CGRectGP::DeflateRect(CGSizeGP& sizeGP)
{
	this->m_R.left	 += sizeGP.m_S.clon;
	this->m_R.top	 -= sizeGP.m_S.clat;
	this->m_R.right	 -= sizeGP.m_S.clon;
	this->m_R.bottom += sizeGP.m_S.clat;
}

void CGRectGP::DeflateRect(CGRectGP& rectGP)
{
	// CRect의 경우는 top보다 bottom의 값이 크다.
	// left -= lpRect->left;
	// top -= lpRect->top;
	// right += lpRect->right;
	// bottom += lpRect->bottom;
	this->m_R.left	 += rectGP.m_R.left;
	this->m_R.top	 -= rectGP.m_R.top;
	this->m_R.right	 -= rectGP.m_R.right;
	this->m_R.bottom += rectGP.m_R.bottom;
}

void CGRectGP::DeflateRect(double l, double t, double r, double b)
{
	// CRect의 경우는 top보다 bottom의 값이 크다.
	// left += l;
	// top += t;
	// right -= r;
	// bottom -= b;
	this->m_R.left   += l;
	this->m_R.top    -= t;
	this->m_R.right  -= r;
	this->m_R.bottom += b;
}

void CGRectGP::NormalizeRect()
{
	double nTemp;
	if ( (this->m_R.left) > (this->m_R.right) )
	{
		nTemp = this->m_R.left;
		this->m_R.left = this->m_R.right;
		this->m_R.right = nTemp;
	}
	//	if ( (this->top) > (this->bottom) ) // CRect의 경우
	if ( (this->m_R.top) < (this->m_R.bottom) )
	{
		nTemp = this->m_R.top;
		this->m_R.top = this->m_R.bottom;
		this->m_R.bottom = nTemp;
	}
}

void CGRectGP::OffsetRect(double lon, double lat)
{
	this->m_R.left   += lon;
	this->m_R.top    += lat;
	this->m_R.right  += lon;
	this->m_R.bottom += lat;
}

void CGRectGP::OffsetRect(CGPointGP& gp)
{
	this->m_R.left		+= gp.m_P.lon;
	this->m_R.top		+= gp.m_P.lat;
	this->m_R.right		+= gp.m_P.lon;
	this->m_R.bottom	+= gp.m_P.lat;
}

void CGRectGP::OffsetRect(CGSizeGP& sizeGP)
{
	this->m_R.left		+= sizeGP.m_S.clon;
	this->m_R.top		+= sizeGP.m_S.clat;
	this->m_R.right		+= sizeGP.m_S.clon;
	this->m_R.bottom	+= sizeGP.m_S.clat;
}

const CGRectGP& CGRectGP::operator=(const CGRectGP &right)
{
	if(&right!=this) {
		this->m_R.left   = right.m_R.left;
		this->m_R.top    = right.m_R.top;
		this->m_R.right  = right.m_R.right;
		this->m_R.bottom = right.m_R.bottom;

	}
	return	*this;
}
BOOL CGRectGP::operator ==(const CGRectGP& rectGP) const
{
	return this->EqualRect(rectGP);
}

BOOL CGRectGP::operator !=(const CGRectGP& rectGP) const
{
	return !( this->EqualRect(rectGP) );
}

void CGRectGP::SetRect(CGRectGeo *pRect)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	CGRectGP	*pGP = (CGRectGP *) pRect;

	m_R.left   = pGP->m_R.left;
	m_R.top    = pGP->m_R.top;
	m_R.right  = pGP->m_R.right;
	m_R.bottom = pGP->m_R.bottom;
}

void CGRectGP::SetRect(RECTGP *pRect)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	m_R.left   = pRect->left;
	m_R.top    = pRect->top;
	m_R.right  = pRect->right;
	m_R.bottom = pRect->bottom;
}

void CGRectGP::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	CGSizeGP	*pGP = (CGSizeGP *) pOffset;

	m_R.left   += pGP->m_S.clon;
	m_R.top    += pGP->m_S.clat;
	m_R.right  += pGP->m_S.clon;
	m_R.bottom += pGP->m_S.clat;
}

void CGRectGP::Offset(SIZEGP *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_GP)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_R.left   += pOffset->clon;
	m_R.top    += pOffset->clat;
	m_R.right  += pOffset->clon;
	m_R.bottom += pOffset->clat;
}


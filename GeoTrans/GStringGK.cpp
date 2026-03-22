// GStringGK.cpp: implementation of the CGStringGK class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGK.h"

#include "GStringGeo.h"
#include "GStringGK.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGStringGK::CGStringGK() : CGStringGeo(GSTYPE_GK)
{
	this->Empty();
}

CGStringGK::CGStringGK(CString str, VERHEMISPHERE hemisphere ) : CGStringGeo(GSTYPE_GK)
{
	if (!this->SetPoint(str, hemisphere))
		this->Empty();
}

CGStringGK::~CGStringGK()
{
}

const CGStringGK& CGStringGK::operator = (const CGStringGK &right)
{
	if (&right != this)
	{
		this->m_hemisphere = right.m_hemisphere;	
		this->m_unit = right.m_unit;

		this->m_zone = right.m_zone;	
		this->m_x = right.m_x;
		this->m_y = right.m_y;
	}
	return	*this;
}

void CGStringGK::Empty()
{
	this->m_hemisphere = NORTH;
	this->m_zone = 0;
	this->m_x = 0.0;
	this->m_y = 0.0;

	this->m_unit = GUNIT_GK_10;
}

BOOL CGStringGK::IsEmpty() const
{
	if (this->m_hemisphere == NORTH)
		if (this->m_zone == 0)
			if (this->m_x == 0.0)
				if (this->m_y == 0.0) return TRUE;
	return FALSE;
}

int CGStringGK::IsValid(CString &str, BOOL bSet)
{
	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();	// Removes newline, space, and tab characters
	tmpStr.TrimRight();	// Removes newline, space, and tab characters

	// GK 좌표의 식별자가 있는가를 조사한다.
	if (tmpStr.Left(1) == "K")
		tmpStr = tmpStr.Right(tmpStr.GetLength() - 1);

	// 모두 숫자열로 되어 있는가를 검사한다.
	if (!this->IsDigit(tmpStr)) return -1;	// 숫자열 이외의 문자가 포함된 에러

	int length = tmpStr.GetLength();

	// GK 좌표의 자리수 에러 검사, 최소 길이 5
	if ((length % 2) == 0 || length < 5) return -1;

	int northStrLen = length / 2;

	BYTE zone = (BYTE) ::atoi(tmpStr.Mid(northStrLen, 2));

	if (!this->IsValidZone(zone)) return -1; // GK 좌표의 zone 값 에러

	if (bSet)
	{
		int nUnit;
		double easting, northing;
		northing = ::atof(tmpStr.Left(northStrLen));
		easting  = ::atof(tmpStr.Right(northStrLen - 1));
		nUnit = northStrLen - 2;

		double Multiple = 100000;
		for (int i = 0; i < nUnit; i++) Multiple /= 10.0;
		
		this->m_zone = zone;
		this->m_x = easting * Multiple;
		this->m_y = northing * Multiple;

		this->m_unit = (BYTE) (nUnit * 2);
	}
	return 1;
}

BOOL CGStringGK::IsValidZone(BYTE zone)
{
	if (1 <= zone && zone <= 60) return TRUE;
	else return FALSE;
}

BOOL CGStringGK::SetPoint(CString &str, VERHEMISPHERE hemisphere)
{
	if (!this->IsValid(str, TRUE)) {
		this->Empty();
		return FALSE;
	}
	this->m_hemisphere = hemisphere;
	return TRUE;
}

BOOL CGStringGK::SetPoint(CGPointGK *pGK, int nUnit)
{
	ASSERT(GUNIT_GK_FIRST <= nUnit && nUnit <= GUNIT_GK_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GUNIT_GK_FIRST <= nUnit && nUnit <= GUNIT_GK_LAST)) return FALSE;

	this->m_unit = (BYTE) nUnit;

	this->m_zone = pGK->m_P.zone;
	this->m_x = pGK->m_P.x;
	if (pGK->m_P.y >= 0.0) {	// 북반구
		this->m_y = pGK->m_P.y;
		this->m_hemisphere = NORTH;
	}
	else {
		this->m_y = pGK->m_P.y + GK_FALSE_NORTHING;
		this->m_hemisphere = SOUTH;
	}

	return TRUE;
}

void CGStringGK::GetPoint(CGPointGK *pGK)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	pGK->m_P.zone = this->m_zone;
	pGK->m_P.x = this->m_x;
	pGK->m_P.y = this->m_y;
	if (this->m_hemisphere == SOUTH)
		pGK->m_P.y -= GK_FALSE_NORTHING;	// 남반구일 경우
}

CString CGStringGK::GetString(int nUnit) const
{
	if (nUnit == -1) nUnit = this->m_unit;

	CString zoneStr, eastStr, northStr, hemiStr;

	zoneStr.Format("%02d", this->m_zone);
	eastStr.Format("%09.1f",   this->m_x * 10);
	northStr.Format("%010.1f", this->m_y * 10);

	if (this->m_hemisphere == NORTH) hemiStr = "N";
	else hemiStr = "S";

	switch(nUnit)
	{
	case GUNIT_GK_12 :	// 42190000 224252000	: 0.1  미터 단위
		eastStr.Format("%07.0f",   this->m_x * 10);	// 소수 둘째자리서 반올림
		northStr.Format("%08.0f", this->m_y * 10);
		eastStr  = eastStr.Left(7);
		northStr = northStr.Left(8);
		break;
	case GUNIT_GK_8 :	// 421900 2242520		: 10   미터 단위
		eastStr  = eastStr.Left(5);
		northStr = northStr.Left(6);
		break;
	case GUNIT_GK_6 :	// 42190 224252			: 100  미터 단위
		eastStr  = eastStr.Left(4);
		northStr = northStr.Left(5);
		break;
	case GUNIT_GK_4 :	// 4219 22425			: 1000 미터 단위
		eastStr  = eastStr.Left(3);
		northStr = northStr.Left(4);
		break;
	case GUNIT_GK_2 :	// 421 2242				: 1만  미터 단위
		eastStr  = eastStr.Left(2);
		northStr = northStr.Left(3);
		break;
	case GUNIT_GK_0 :	// 42 224				: 10만 미터 단위
		eastStr  = eastStr.Left(1);
		northStr = northStr.Left(2);
		break;
	case GUNIT_GK_10 :	// 4219000 22425200		: 1    미터 단위
	default:
		eastStr.Format("%06.0f",  this->m_x);	// 소수 첫째자리서 반올림
		northStr.Format("%07.0f", this->m_y);
		eastStr  = eastStr.Left(6);
		northStr = northStr.Left(7);
		break;
	}

	CString gkStr;
	gkStr.Format("%s%s%s,%s", northStr, zoneStr, eastStr, hemiStr);
	return gkStr;
}



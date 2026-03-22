// GStringUPS.cpp: implementation of the CGStringUPS class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"


#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoUPS.h"

#include "GStringGeo.h"
#include "GStringUPS.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGStringUPS::CGStringUPS() : CGStringGeo(GSTYPE_UPS)
{
}

CGStringUPS::CGStringUPS(CString str) : CGStringGeo(GSTYPE_UPS)
{
	if ( !this->SetPoint(str) ) {
		this->Empty();
	}
}

CGStringUPS::~CGStringUPS()
{
}

const CGStringUPS& CGStringUPS::operator = (const CGStringUPS &right)
{
	if (&right != this)
	{
		this->m_zone = right.m_zone;	

		this->m_xch  = right.m_xch;
		this->m_x    = right.m_x;
		this->m_ych  = right.m_ych;
		this->m_y    = right.m_y;

		this->m_nUnit = right.m_nUnit;
	}
	return	*this;
}

void CGStringUPS::Empty()
{
	this->m_zone = 0;
	this->m_xch = 0;
	this->m_x = 0.0;
	this->m_ych = 0;
	this->m_y = 0.0;
	this->m_nUnit = 0;
}

BOOL CGStringUPS::IsEmpty() const
{
	if (this->m_zone == 0)
		if (this->m_xch == 0)
			if (this->m_x == 0.0)
				if (this->m_ych == 0)
					if (this->m_y == 0.0)
						if (this->m_nUnit == 0)	return TRUE;
	return FALSE;
}

int	CGStringUPS::IsValid(CString &str, BOOL bSet)
{
	CString mgrsStr(str);
	mgrsStr.MakeUpper();	// 대문자로 변환한다.
	mgrsStr.TrimLeft();	// Removes newline, space, and tab characters
	mgrsStr.TrimRight();	// Removes newline, space, and tab characters

	// UPS 좌표의 식별자가 있는가를 조사한다.
	if (mgrsStr.Left(1) == "P")
		mgrsStr = mgrsStr.Right(mgrsStr.GetLength() - 1);

	// UPS zone 값을 검사한다.

	// ZXXxxxxxyyyyy
	//
	//	Z : zone 번호
	//	XX : 경도 구역 식별자
	//	xxxxx : 기준으로부터 경도 방향으로 거리, 1 ~ 5 자리수
	//	yyyyy : 기준으로부터 위도 방향으로 거리. 1 ~ 5 자리수
	// % xxxxx와 yyyyy는 자리수가 같아야 한다.

	if (mgrsStr.IsEmpty()) return -1;

	BOOL bCorrect = FALSE;
	if (mgrsStr.GetLength() < 1) return -1;
	BYTE zone = mgrsStr[0];
	// 문자열의 첫 글자로 zone을 검사한다.
	switch(zone)
	{
	case 'Y' : case 'Z' : case 'A' : case 'B' :
		bCorrect = TRUE; break;
	default : 
		bCorrect = FALSE; break;
	}

	if (!bCorrect) {
//		::TRACE0("UPS좌표의 구역(Zone) 범위 오류입니다.");
		return -1; // zone 오류
	}

	CString tmpStr;
	tmpStr = mgrsStr.Mid(1, 2);
	if (tmpStr.GetLength() < 2) return -1;
	if ( !IsValid100KmChar(tmpStr[0], UPS_EASTING) )  return -1; // 문자에러
	if ( !IsValid100KmChar(tmpStr[1], UPS_NORTHING) ) return -1; // 문자에러

	int len = mgrsStr.GetLength();
	CString	digitStr = mgrsStr.Mid(3, len-3);
	int	digitLen = digitStr.GetLength();

	if ( digitLen > GUNIT_UPS_LAST || digitLen < GUNIT_UPS_FIRST || (digitLen % 2) != 0) {
		// 숫자의 갯수가 맞지 않음.
		return -1;
	}

	// 숫자열에 숫가 아닌 문자가 들어 있는지 검사한다.
	if ( !this->IsDigit(digitStr) ) return -1;

	if ( bSet )
	{
		this->m_nUnit = (BYTE) digitLen;

		this->m_zone = mgrsStr[0];
		this->m_xch = mgrsStr[1];
		this->m_x = ::atof(digitStr.Left(this->m_nUnit / 2));
		this->m_ych = mgrsStr[2];
		this->m_y = ::atof(digitStr.Right(this->m_nUnit / 2));

		this->m_x = this->m_x * (100000 * ::pow(0.1, digitLen / 2));
		this->m_y = this->m_y * (100000 * ::pow(0.1, digitLen / 2));
	}
	return 1;
}

BOOL CGStringUPS::IsValid100KmChar(BYTE ch, int nAxis)
{
	BYTE chMin, chMax;
	ch = (BYTE) toupper(ch);

	chMin = 'A';
	chMax = 'Z';

	if ( chMin > ch || ch > chMax ) return FALSE;
	if ( ch == 'I' || ch == 'O') return FALSE;
	
	nAxis = nAxis;
	return TRUE;
}

BOOL CGStringUPS::SetPoint(CString &str)
{
	if ( IsValid(str, TRUE) < 0 ) {
		this->Empty();
		return FALSE;
	}
	return TRUE;
}

BOOL CGStringUPS::SetPoint(CGPointUPS *pUPS, int nUnit)
{
	ASSERT(GUNIT_UPS_FIRST <= nUnit && nUnit <= GUNIT_UPS_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GUNIT_UPS_FIRST <= nUnit && nUnit <= GUNIT_UPS_LAST)) return FALSE;

	BYTE EastCharArray[] = { 'A', 'B', 'C', 'F', 'G', 'H', 'J', 'K', 'L', 'P',
							 'Q', 'R', 'S', 'T', 'U', 'X', 'Y', 'Z' };

	int indexEasting  = (long) pUPS->m_P.x / (long) 100000;	// 백만, 십만의 값을 인덱스로 한다.
	int indexNorthing = (long) pUPS->m_P.y / (long) 100000;	// 백만, 십만의 값을 인덱스로 한다.
	
	if ( indexEasting >= 20 )	indexEasting -= 20;	// A값 부터 시작한다.
	else indexEasting -= 2;	// Z값 부터 역으로 계산된다. 19를 배열의 마지막인 17로 만든다.
	this->m_xch = EastCharArray[indexEasting];

	if ( pUPS->m_P.hemisphere == NORTH )
	{		// 북극
		if ( pUPS->m_P.x < UPS_FALSE_EASTING ) this->m_zone = 'Y';	// 서경
		else this->m_zone = 'Z';	// 동경
		BYTE NorthCharArrayNorth[] = { 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S',
									   'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'A', 'B', 'C',
									   'D', 'E', 'F', 'G' };
		if ( indexNorthing >= 20 )	indexNorthing -= 20;	// H값 부터 시작한다.
		else indexNorthing += 4;	// G값 부터 역으로 계산된다. 19를 23으로 만든다.
		this->m_ych = NorthCharArrayNorth[indexNorthing];
	}
	else	// 남극
	{
		if ( pUPS->m_P.x < UPS_FALSE_EASTING ) this->m_zone = 'A';	// 서경
		else this->m_zone = 'B';
		BYTE NorthCharArraySouth[] = { 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
									   'Y', 'Z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
									   'J', 'K', 'L', 'M' };
		if ( indexNorthing >= 20 )	indexNorthing -= 20;	// N값 부터 시작한다.
		else indexNorthing += 4;	// M값 부터 역으로 계산된다. 19를 23으로 만든다.
		this->m_ych = NorthCharArraySouth[indexNorthing];
	}

	this->m_x = pUPS->m_P.x - (double) ( ((long)pUPS->m_P.x/(long)100000) * (long)100000 );
	this->m_y = pUPS->m_P.y - (double) ( ((long)pUPS->m_P.y/(long)100000) * (long)100000 );
	return TRUE;
}

void CGStringUPS::GetPoint(CGPointUPS *pUPS)
{
	BOOL north = TRUE;
	if ( this->m_zone == 'A' || this->m_zone == 'B' ) north = FALSE;
	pUPS->m_P.hemisphere = ( north ) ? NORTH : SOUTH;

	BYTE EastCharArray[] = { 'A', 'B', 'C', 'F', 'G', 'H', 'J', 'K', 'L', 'P',
							 'Q', 'R', 'S', 'T', 'U', 'X', 'Y', 'Z' };
	int indexEasting = 0;
	// 10만 미터 격자에서 동향에 대한 문자를 검색한다.
	for ( ; ( indexEasting < 18 ) && ( EastCharArray[indexEasting] != this->m_xch ); indexEasting++ );
	switch(this->m_zone)
	{
	case 'A' : case 'Y' :	// 서경
		indexEasting += 2;
		break;
	case 'B' : case 'Z' :	// 동경
		indexEasting += 20;
		break;
	default :
//		TRACE0("UPS좌표의 구역(Zone) 범위 오류입니다");
		return;
	}
	pUPS->m_P.x = ((double)indexEasting * (double)100000.0) + this->m_x;

	BYTE NorthCharArrayNorth[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
								   'L', 'M', 'N', 'P' };
	BYTE NorthCharArraySouth[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
								   'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
								   'W', 'X', 'Y', 'Z' };
	int indexNorthing = 0;
	// 10만 미터 격자에서 북향에 대한 문자를 검색한다.
	switch(this->m_zone)
	{
	case 'A' :	case 'B' :	// 남극, 서경, 동경
		for ( ; ( indexNorthing < 24 ) && ( NorthCharArraySouth[indexNorthing] != this->m_ych ); indexNorthing++ );
		indexNorthing += 8;	// 역으로 검색하기 때문
		break;
	case 'Y' :	case 'Z' :	// 북극 서경, 동경
		for ( ; ( indexNorthing < 14 ) && ( NorthCharArrayNorth[indexNorthing] != this->m_ych ); indexNorthing++ );
		indexNorthing += 13;
		break;
	default :
//		TRACE0("UPS좌표의 구역(Zone) 범위 오류입니다");
		return;
	}
	pUPS->m_P.y = ((double)indexNorthing * (double)100000.0) + this->m_y;
}

CString	CGStringUPS::GetString(int nUnit) const
{
	if (this->IsEmpty() == TRUE) return _T("");

	// 기존에 지정되어 있는 갯수를 사용한다.
	if (nUnit == -1) nUnit = this->m_nUnit;

	CString zoneStr, eastStr, northStr;

	zoneStr.Format("%02d", this->m_zone);
	eastStr.Format("%08.1f",  this->m_x * 10);
	northStr.Format("%08.1f", this->m_y * 10);

	switch(nUnit)
	{
	case GUNIT_UPS_12 :							// : 0.1  미터 단위(12단계)
		eastStr.Format("%06.0f",  this->m_x * 10);	// 소수 둘째자리서 반올림
		northStr.Format("%06.0f", this->m_y * 10);
		eastStr  = eastStr.Left(6);
		northStr = northStr.Left(6);
		break;
	case GUNIT_UPS_8 :							// : 10   미터 단위(8 단계)	
		eastStr  = eastStr.Left(4);
		northStr = northStr.Left(4);
		break;
	case GUNIT_UPS_6 : 							// : 100  미터 단위(6 단계)
		eastStr  = eastStr.Left(3);
		northStr = northStr.Left(3);
		break;
	case GUNIT_UPS_4 :							// : 1000 미터 단위(4 단계)
		eastStr  = eastStr.Left(2);
		northStr = northStr.Left(2);
		break;
	case GUNIT_UPS_2 : 							// : 1만  미터 단위(2 단계)
		eastStr  = eastStr.Left(1);
		northStr = northStr.Left(1);
		break;
	case GUNIT_UPS_0 : 							// : 10만 미터 단위(0 단계)
		eastStr  = eastStr.Left(0);
		northStr = northStr.Left(0);
		break;
	case GUNIT_UPS_10 :							// : 1    미터 단위(10단계)
	default:
		eastStr.Format(_T("%05.0f"),  this->m_x);	// 소수 첫째자리서 반올림
		northStr.Format(_T("%05.0f"), this->m_y);
		eastStr  = eastStr.Left(5);
		northStr = northStr.Left(5);
		break;
	}

	CString upsStr;
	upsStr.Format(_T("%1c%1c%1c%s%s"), m_zone, m_xch, m_ych, eastStr, northStr);
	return upsStr;
}

BOOL CGStringUPS::operator == (CGStringUPS right) const
{
	CString upsLeftStr, upsRightStr;

	upsLeftStr = this->GetString(GUNIT_UPS_10);
	upsRightStr = right.GetString(GUNIT_UPS_10);

	if ( upsLeftStr != upsRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGStringUPS::operator != (CGStringUPS right) const
{
	CString upsLeftStr, upsRightStr;

	upsLeftStr = this->GetString(GUNIT_UPS_10);
	upsRightStr = right.GetString(GUNIT_UPS_10);

	if ( upsLeftStr == upsRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}


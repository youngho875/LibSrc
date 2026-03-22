// GStringGEOREF.cpp: implementation of the CGStringGEOREF class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"

#include "GStringGeo.h"
#include "GStringGEOREF.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGStringGEOREF::CGStringGEOREF() : CGStringGeo(GSTYPE_GEOREF)
{
	this->Empty();
}

CGStringGEOREF::~CGStringGEOREF()
{
}

CGStringGEOREF::CGStringGEOREF(CString str) : CGStringGeo(GSTYPE_GEOREF)
{
	if ( !this->SetPoint(str) ) {
		this->Empty();
	}
}

const CGStringGEOREF& CGStringGEOREF::operator = (const CGStringGEOREF &right)
{
	if(&right != this) {
		this->m_15DLon	= right.m_15DLon;
		this->m_1DLon	= right.m_1DLon;
		this->m_1MLon	= right.m_1MLon;
		this->m_60SLon	= right.m_60SLon;

		this->m_15DLat	= right.m_15DLat;
		this->m_1DLat	= right.m_1DLat;
		this->m_1MLat	= right.m_1MLat;
		this->m_60SLat	= right.m_60SLat;
	}
	return	*this;
}

void CGStringGEOREF::Empty()
{
	m_15DLon = 0;  // 경도를 15도 간격으로 나눈 INDEX값(24개 문자)
	m_1DLon  = 0;  // 경도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	m_1MLon  = 0;  // 00 ~ 59 사이의 값 (경도 1도를 1분 단위로 분할한 값)
	m_60SLon = 0.0;// 초(second) : 0.0 <= m_60SLon < 60.0

	m_15DLat = 0;  // 위도를 15도 간격으로 나눈 INDEX값(12개 문자)
	m_1DLat  = 0;  // 위도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	m_1MLat  = 0;  // 00 ~ 59 사이의 값 (위도 1도를 1분 단위로 분할한 값)
	m_60SLat = 0.0;// 초(second) : 0.0 <= m_60SLon < 60.0
}

BOOL CGStringGEOREF::IsEmpty() const
{
	// Empty 상태는 모든 초기값이 0이다.
	// 모든 값은 개별적으로 설정될 수 없으므로 1개의 값만 비교해도 된다.
	if (m_15DLon == 0) return FALSE;
	return TRUE;
/*
	m_15DLon = 0;  // 경도를 15도 간격으로 나눈 INDEX값(24개 문자)
	m_1DLon  = 0;  // 경도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	m_1MLon  = 0;  // 00 ~ 59 사이의 값 (경도 1도를 1분 단위로 분할한 값)
	m_60SLon = 0.0;// 초(second) : 0.0 <= m_60SLon < 60.0

	m_15DLat = 0;  // 위도를 15도 간격으로 나눈 INDEX값(12개 문자)
	m_1DLat  = 0;  // 위도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	m_1MLat  = 0;  // 00 ~ 59 사이의 값 (위도 1도를 1분 단위로 분할한 값)
	m_60SLat = 0.0;// 초(second) : 0.0 <= m_60SLon < 60.0
*/
}

int CGStringGEOREF::IsValid(CString &str, BOOL bSet)
{
	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();	// Removes newline, space, and tab characters
	tmpStr.TrimRight();	// Removes newline, space, and tab characters

	// GEOREF 좌표의 식별자가 있는가를 조사한다.
	if ( (tmpStr.GetLength() % 2) == 1)
	{
		if (tmpStr.Left(1) == "F")
			tmpStr = tmpStr.Right(tmpStr.GetLength() - 1);
	}

	CString aaa;
	// Each 15 degree quardrange check
	aaa = tmpStr.Left(2);
	if (Is15DegQuardrange(aaa, bSet)  <= 0) return -1;
	// Each One degree quardrange check
	aaa = tmpStr.Mid(2,2);
	if (IsOneDegQuardrange(aaa, bSet) <= 0) return -1;

	int length = str.GetLength();
	CString digitStr = str.Right(length-4);

	// 숫자열이 GEOREF좌표에 맞는지를 검사 및 설정한다.
	if (Is1MinSecQuardrange(digitStr, bSet) <= 0) return -1;

	return 1;	// 모든 검사과정이 성공적으로 끝남
}

int CGStringGEOREF::Is15DegQuardrange(CString &str, BOOL bSet)
{
	// 문자열의 길이가 2가 아니면 -1을 리턴한다.
	if (str.GetLength() != 2) return -1;
	
	// 문자열에 알파벹이 아닌 문자가 있는 경우 -1을 리턴한다.
	if (!this->IsAlpha(str)) return -1;

	str.MakeUpper();	// 대문자로 변환한다.

	// 경도 범위 검사 : A - Z (I, O 제외)
	if ( str[0] == 'I' || str[0] == 'O') return -1;
	
	// 위도 범위 검사 : A - M (I, O 제외)
	if ( 'A'  > str[1] || str[1] > 'M' ) return -1;
	if ( str[1] == 'I' || str[1] == 'O') return -1;

	// 설정
	if ( bSet ) {
		this->m_15DLon = str[0];
		this->m_15DLat = str[1];
	}
	return TRUE;
}

int CGStringGEOREF::IsOneDegQuardrange(CString &str, BOOL bSet)
{
	// 문자열의 길이가 2가 아니면 -1을 리턴한다.
	if (str.GetLength() != 2) return -1;

	// 문자열에 알파벹이 아닌 문자가 있는 경우 -1을 리턴한다.
	if (!this->IsAlpha(str)) return -1;

	str.MakeUpper();	// 대문자로 변환한다.

	// 경도 범위 검사 : A - Q (I, O 제외)
	if ( 'A'  > str[0] || str[0] > 'Q' ) return -1;
	if ( str[0] == 'I' || str[0] == 'O') return -1;
	
	// 위도 범위 검사 : A - Q (I, O 제외)
	if ( 'A'  > str[1] || str[1] > 'Q' ) return -1;
	if ( str[1] == 'I' || str[1] == 'O') return -1;

	// 설정
	if ( bSet ) {
		this->m_1DLon = str[0];
		this->m_1DLat = str[1];
	}
	return TRUE;
}

int CGStringGEOREF::Is1MinSecQuardrange(CString &str, BOOL bSet)
{
	int length = str.GetLength();
	if (length == 0) { // 문자열이 없음 : 1도 단위의 GEOREF좌표
		if (bSet) {
			this->m_1MLon  = 0;
			this->m_60SLon = 0.0;
			this->m_1MLat  = 0;
			this->m_60SLat = 0.0;
		}
		return TRUE;
	}
	
	if (length % 2 != 0) return -1;	// 숫자가 짝수개가 아니면 에러
	if (!IsDigit(str)) return -1;	// 숫자 이외의 문자열이 포함되면 에러
	if (length < 4) return -1;		// 숫자열이 4미만이면 에러

	int halfLength = length / 2;
	CString leftStr, rightStr;
	leftStr  = str.Left(halfLength);	// 경도 숫자열
	rightStr = str.Right(halfLength);	// 위도 숫자열

	BYTE nMinLon, nMinLat;
	nMinLon = (BYTE) ::atoi(leftStr.Left(2));
	nMinLat = (BYTE) ::atoi(rightStr.Left(2));

	if ( nMinLon < MIN_RANGE_MIN || MAX_RANGE_MIN < nMinLon )
		return FALSE;	// 도 범위 에러(경도)
	if ( nMinLat < MIN_RANGE_MIN || MAX_RANGE_MIN < nMinLat )
		return FALSE;	// 도 범위 에러(위도)

	double dSecLon, dSecLat;
	CString secLeftStr("."), secRightStr(".");
	secLeftStr  += leftStr.Right(halfLength - 2);
	secRightStr += rightStr.Right(halfLength - 2);

	dSecLon = ::atof(secLeftStr);
	dSecLat = ::atof(secRightStr);

	if (bSet) {
		this->m_1MLon  = nMinLon;
		this->m_60SLon = dSecLon;
		this->m_1MLat  = nMinLat;
		this->m_60SLat = dSecLat;
	}
	return TRUE;
}

BOOL CGStringGEOREF::SetPoint(CString &str)
{
	if ( !this->IsValid(str, TRUE) ) {	// 검사와 동시에 설정을 한다.
		this->Empty();
		return FALSE;
	}
	return TRUE;
}


BOOL CGStringGEOREF::SetPoint(CGPointGP *pGP)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return FALSE;

	BYTE Lon15DegCharArray[24] = { 'A', 'B', 'C', 'D',
					'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P',
					'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	BYTE Lat15DegCharArray[12]  = { 'A', 'B', 'C', 'D', 'E', 'F',
	                                'G', 'H', 'J', 'K', 'L', 'M' };
	BYTE OneDegArray[15] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
							 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q' };
	int index;
	double lon, lat;

	//
	// 경도를 설정한다.
	lon = pGP->m_P.lon + 180.0;	// 경도 범위 : 0.0 - 360.0

	index = (int)lon / 15;
	if ( index >= 24 ) index = 23;  // 경도가 360도(180E)인 경우 Z로 매칭
	this->m_15DLon = Lon15DegCharArray[index];	// 경도 15도 구역

	index = (int)lon % 15;
	this->m_1DLon = OneDegArray[index];	// 경도 1 도 구역

	// 분(Minute)에서는 방향성이 없다.	따라서 분만을 계산하기 위해 다시 설정
	lon = (0.0 <= pGP->m_P.lon) ? pGP->m_P.lon : -pGP->m_P.lon;

	double lonMinTmp;
	lonMinTmp = lon - (double)((int)lon);	// 도 단위를 제거
	lonMinTmp *= 60.0;	// 도를 분으로 환산
	this->m_1MLon = (BYTE) lonMinTmp;
	this->m_60SLon = lonMinTmp - (double)((int)lonMinTmp); // 분 단위를 제거

	//
	// 위도를 설정한다.
	lat = pGP->m_P.lat +  90.0;	// 위도 범위 : 0.0 - 180.0
	
	index = (int)lat / 15;
	if ( index >= 12 ) index = 11;  // 위도가 180도(90N)인 경우 Z로 매칭
	this->m_15DLat = Lat15DegCharArray[index];	// 위도 15도 구역

	index = (int)lat % 15;
	this->m_1DLat = OneDegArray[index];	// 위도 1 도 구역

	// 분(Minute)에서는 방향성이 없다.	따라서 분만을 계산하기 위해 다시 설정
	lat = (0.0 <= pGP->m_P.lat) ? pGP->m_P.lat : -pGP->m_P.lat; 

	double latMinTmp;
	latMinTmp = lat - (double)((int)lat);	// 도 단위를 제거
	latMinTmp *= 60.0;	// 도를 분으로 환산
	this->m_1MLat = (BYTE) latMinTmp;
	this->m_60SLat = latMinTmp - (double)((int)latMinTmp); // 분 단위를 제거

	return TRUE;
}

void CGStringGEOREF::GetPoint(CGPointGP *pGP)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	pGP->m_P.lon = this->GetLongitude();	// 경도를 계산한다.
	pGP->m_P.lat = this->GetLatitude();		// 위도를 계산한다.
}

double CGStringGEOREF::GetLongitude()
{
	double lon;
	
	int index15deg;
	switch ( this->m_15DLon )
	{
	case 'A':	case 'B':	case 'C':	case 'D':	case 'E':
	case 'F':	case 'G':	case 'H':
		index15deg = this->m_15DLon - 'A';
		break;

	case 'J':	case 'K':	case 'L':	case 'M':	case 'N':
		index15deg = this->m_15DLon - 'A' - 1;
		break;

	case 'P':	case 'Q':	case 'R':	case 'S':	case 'T':
	case 'U':	case 'V':	case 'W':	case 'X':	case 'Y':
	case 'Z':
		index15deg = this->m_15DLon - 'A' - 2;
		break;
	}

	int index1deg;
	switch ( this->m_1DLon )
	{
	case 'A':	case 'B':	case 'C':	case 'D':	case 'E':
	case 'F':	case 'G':	case 'H':
		index1deg = this->m_1DLon - 'A';
		break;
	case 'J':	case 'K':	case 'L':	case 'M':	case 'N':
		index1deg = this->m_1DLon - 'A' - 1;
		break;
	case 'P':	case 'Q':
		index1deg = this->m_1DLon - 'A' - 2;
		break;
	}

	lon = (double)(index15deg * 15 + index1deg - 180.0);

	double inc = 0.0;
	inc += ((double) this->m_1MLon / 60.0);
	inc += m_60SLon / 60.0;

	if (0.0 <= lon)	// 동경
		lon += inc;
	else			// 서경
		lon = (::fabs(lon) + inc) * -1;

	return lon;
}

double CGStringGEOREF::GetLatitude()
{
	double lat;

	int index15deg;
	switch ( this->m_15DLat )
	{
	case 'A':	case 'B':	case 'C':	case 'D':	case 'E':
	case 'F':	case 'G':	case 'H':
		index15deg = this->m_15DLat - 'A';
		break;

	case 'J':	case 'K':	case 'L':	case 'M':
		index15deg = this->m_15DLat - 'A' - 1;
		break;
	}

	int index1deg;
	switch ( this->m_1DLat )
	{
	case 'A':	case 'B':	case 'C':	case 'D':	case 'E':
	case 'F':	case 'G':	case 'H':
		index1deg = this->m_1DLat - 'A';
		break;
	case 'J':	case 'K':	case 'L':	case 'M':	case 'N':
		index1deg = this->m_1DLat - 'A' - 1;
		break;
	case 'P':	case 'Q':
		index1deg = this->m_1DLat - 'A' - 2;
		break;
	}

	lat = (double)(index15deg * 15 + index1deg - 90.0);

	double inc = 0.0;
	inc += ((double) this->m_1MLat / 60.0);
	inc += m_60SLat / 60.0;

	if (0.0 <= lat)	// 북위
		lat += inc;
	else			// 남위
		lat = (::fabs(lat) + inc) * -1;

	return lat;
}

CString	CGStringGEOREF::GetString(int nUnit) const
{
	CString georefStr;

	// 만약 현재의 값이 Empty면 NULL 스트링을 리턴한다.
	if (!this->IsEmpty()) return georefStr;
	
	CString lon15DStr, lon1DStr, lonMinSecStr;
	CString	lat15DStr, lat1DStr, latMinSecStr;
	
	switch(nUnit)
	{
	case GUNIT_GEOREF_1 :
		lon15DStr.Format("%1c", this->m_15DLon);// 경도 15도 구역
		lon1DStr.Format("%1c", this->m_1DLon);	// 경도 1 도 구역	
		lonMinSecStr.Format("%02d", this->m_1MLon);

		lat15DStr.Format("%1c", this->m_15DLat);// 위도 15도 구역
		lat1DStr.Format("%1c", this->m_1DLat);	// 위도 1 도 구역
		latMinSecStr.Format("%02d", this->m_1MLat);
		break;
	case GUNIT_GEOREF_2 :
		lon15DStr.Format("%1c", this->m_15DLon);// 경도 15도 구역
		lon1DStr.Format("%1c", this->m_1DLon);	// 경도 1 도 구역	
		lonMinSecStr.Format("%01d", this->m_1MLon / 10);

		lat15DStr.Format("%1c", this->m_15DLat);// 위도 15도 구역
		lat1DStr.Format("%1c", this->m_1DLat);	// 위도 1 도 구역
		latMinSecStr.Format("%01d", this->m_1MLat / 10);
		break;
	case GUNIT_GEOREF_3 :
		lon15DStr.Format("%1c", this->m_15DLon);// 경도 15도 구역
		lon1DStr.Format("%1c", this->m_1DLon);	// 경도 1 도 구역	

		lat15DStr.Format("%1c", this->m_15DLat);// 위도 15도 구역
		lat1DStr.Format("%1c", this->m_1DLat);	// 위도 1 도 구역
		break;
	case GUNIT_GEOREF_4 :
		lon15DStr.Format("%1c", this->m_15DLon);// 경도 15도 구역

		lat15DStr.Format("%1c", this->m_15DLat);// 위도 15도 구역
		break;
	case GUNIT_GEOREF_0 :
	default :
		lon15DStr.Format("%1c", this->m_15DLon);// 경도 15도 구역
		lon1DStr.Format("%1c", this->m_1DLon);	// 경도 1 도 구역	
		lonMinSecStr.Format("%02d%01d", this->m_1MLon, (int) (this->m_60SLon * 10));

		lat15DStr.Format("%1c", this->m_15DLat);// 위도 15도 구역
		lat1DStr.Format("%1c", this->m_1DLat);	// 위도 1 도 구역
		latMinSecStr.Format("%02d%01d", this->m_1MLat, (int) (this->m_60SLat * 10));
		break;
	}
	
	georefStr = lon15DStr + lat15DStr + 
				lon1DStr + 	lat1DStr +
				lonMinSecStr + latMinSecStr;
	
	return georefStr;
}


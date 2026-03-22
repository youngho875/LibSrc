// GStringGP.cpp: implementation of the CGStringGP class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"

#include "GStringGeo.h"
#include "GStringGP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGStringGP::CGStringGP() : CGStringGeo(GSTYPE_GP)
{
	Empty();
}

CGStringGP::CGStringGP(CString str) : CGStringGeo(GSTYPE_GP)
{
	if ( !SetPoint(str) ) {
		Empty();
	}
}

CGStringGP::~CGStringGP()
{
}

const CGStringGP& CGStringGP::operator = (const CGStringGP &right)
{
	if (&right != this)
	{
		this->m_NorSou = right.m_NorSou;	
		this->m_degLon = right.m_degLon;
		this->m_minLon = right.m_minLon;
		this->m_secLon = right.m_secLon;

		this->m_EasWes = right.m_EasWes;	
		this->m_degLat = right.m_degLat;
		this->m_minLat = right.m_minLat;
		this->m_secLat = right.m_secLat;
	}
	return	*this;
}

void CGStringGP::Empty()
{
	m_NorSou = NORTH;   // NORTH : 북위, SOUTH : 남위	// VHEMISPHERE
	m_degLon = 0;		// 도	// UINT
	m_minLon = 61;		// 분	// UINT
	m_secLon = -1.0;	// 초	// double

	m_EasWes = EAST;   // EAST : 동경, WEST : 서경		// VHEMISPHERE
	m_degLat = 0;		// 도	// UINT
	m_minLat = 61;		// 분	// UINT
	m_secLat = -1.0;	// 초	// double
}

// 동경, 서경을 구분하고 맞으면 m_EasWes값을 설정하고 인덱스를 리턴한다.
BOOL CGStringGP::IsEmpty() const
{
	if (this->m_NorSou == NORTH)
		if (this->m_degLon == 0)
			if (this->m_minLon == 61)
				if (this->m_secLon == -1.0) return TRUE;
	return FALSE;
}

int CGStringGP::SetLonHemisphere(CString &str, BOOL bSet)
{
	int nEast, nWest;
	int nLonInx;

	nEast = str.Find("E");	// 동경을 검사한다.
	nWest = str.Find("W");	// 서경을 검사한다.
	if ( nEast == -1 )
	{
		if ( nWest == -1 ) {
			if ( bSet ) Empty();
			return -1;	// 동경, 서경을 위미하는 문자가 없음
		}
		nLonInx = nWest;	// 서경
		if ( bSet ) this->m_EasWes = WEST;
	}
	else
	{
		if ( nWest != -1 ) {
			if ( bSet ) Empty();
			return -1;	// 동경, 서경을 의미하는 문자가 두개 존재
		}
		nLonInx = nEast;	// 동경
		if ( bSet ) this->m_EasWes = EAST;
	}
	return nLonInx; // Zero-base Index
}

// 북위, 남위를 구분하고 맞으면 m_NorSou값을 설정하고 인덱스를 리턴한다.
int CGStringGP::SetLatHemisphere(CString &str, BOOL bSet)
{
	int nNorth, nSouth;
	int nLatInx;

	nNorth = str.Find("N");	// 북위를 결정한다.
	nSouth = str.Find("S");	// 남위를 결정한다.
	if ( nNorth == -1 )
	{
		if ( nSouth == -1 ) {
			if ( bSet ) Empty();
			return -1;	// 남위, 북위를 위미하는 문자가 없음
		}
		nLatInx = nSouth;	// 남위
		if ( bSet ) this->m_NorSou = SOUTH;
	}
	else
	{
		if ( nSouth != -1 ) {
			if ( bSet ) Empty();
			return -1;	// 남위, 북위를 의미하는 문자가 두개 존재
		}
		nLatInx = nNorth;	// 북위
		if ( bSet ) this->m_NorSou = NORTH;
	}
	return nLatInx;	// Zero-base Index
}

int	CGStringGP::IsValid(CString &str, BOOL bSet)
{
	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();	// Removes newline, space, and tab characters
	tmpStr.TrimRight();	// Removes newline, space, and tab characters

	// 지리좌표의 식별자가 있는가를 조사한다.
	if (tmpStr.Left(1) == "G")
		tmpStr = tmpStr.Right(tmpStr.GetLength() - 1);

	// 동경, 서경은 각각 E, W로 표기되어 있다.
	// 북위, 남위는 각각 N, S로 표기되어 있다ㅣ.
	// 동경, 서경은 도(degree)가 3자리로 표기되어 있다.
	//              분(minute)가 2자리도 표기되어 있다.
	//              초(second)가 2자리로 표기하며,
	//	                         소수점을 기입하고 이하 자리를 표기 한다.
	// 북위, 남위는 도(degree)가 자리로 표기되어 있다.
	//              분(minute)가 2자리도 표기되어 있다.
	//              초(second)가 2자리로 표기하며,
	//			                 소수점을 기입하고 이하 자리를 표기 한다.
	//
	// 표기방법 : dddmmss.sss[E|W]ddmmss.sss[N|S]
	// E,W나  N, S는 초단위 다음에 표기한다.

	int nLonInx, nLatInx;
	// 반구를 설정한다.
	nLonInx = SetLonHemisphere(tmpStr, bSet);
	nLatInx = SetLatHemisphere(tmpStr, bSet);

	// 동경, 서경, 북위, 남위를 구분하는 문자열을 조사
	if ( nLonInx == -1 || nLatInx == -1 ) return -1;	// 에러

	CString digitPartStr("");
	int digitCheck = -1;
	if ( nLonInx < nLatInx )	// 동경, 서경이 앞에 있는 경우
	{
		// 동경 또는 서경 숫자 부분
		digitPartStr = tmpStr.Left(nLonInx);
		// 경도를 설정한다.
		digitCheck = SetDegMinSec(digitPartStr, TRUE, bSet);
		if ( digitCheck <= 0 ) return -1;	// 입력된 문자 에러

		// 북위, 남위의 숫자 부분
		digitPartStr = tmpStr.Mid(nLonInx + 1, nLatInx - nLonInx - 1);
		// 위도를 설정한다.
		digitCheck = SetDegMinSec(digitPartStr, FALSE, bSet);
		if ( digitCheck <= 0 ) return -1;	// 입력된 문자 에러
	}
	else	// 북위, 남위가 앞에 있는 경우
	{
		// 북위, 남위의 숫자 부분
		digitPartStr = tmpStr.Left(nLatInx);
		// 위도를 설정한다.
		digitCheck = SetDegMinSec(digitPartStr, FALSE, bSet);
		if ( digitCheck <= 0 ) return -1;	// 입력된 문자 에러

		// 동경 또는 서경 숫자 부분
		digitPartStr = tmpStr.Mid(nLatInx + 1, nLonInx - nLatInx - 1);
		// 경도를 설정한다.
		digitCheck = SetDegMinSec(digitPartStr, TRUE, bSet);
		if ( digitCheck <= 0 ) return -1;	// 입력된 문자 에러
	}
	return 1;
}

// 숫자열을 검사하고, m_EasWes, m_NorSou값을 가지고 설정한다.
// 0 보다 작으면 에러
// bIsLon == TRUE  : 경도
// bIsLon == FALSE : 위도
int CGStringGP::SetDegMinSec(CString &str, BOOL bIsLon, BOOL bSet)
{
	int periodInx = str.Find(".");
	CString digitStr("");
	if ( periodInx == -1 ) // 문자열에 소숫점 이하의 자리수가 없는 경우
		digitStr = str;
	else	// 문자열에 소숫점이 있는 경우
		digitStr = str.Left(periodInx);	// 소숫점 이하의 문자열을 읽음
	
	if ( !IsDigit(digitStr) )
		return FALSE;	// 문자열에 숫자가 아닌 문자를 포함한 경우

	int length = digitStr.GetLength();
	CString tmpStr("");

	UINT nDeg, nMin;
	double dSec;
	if ( bIsLon )	// 경도
	{
		if ( length < MIN_LEN_LON || MAX_LEN_LON < length )
			return -1;	// 경도의 숫자 길이가 맞지 않음

		tmpStr = digitStr.Right(4);
		
		nDeg = (UINT) atoi(digitStr.Left(length-4));	// 도(degree)
		nMin = (UINT) atoi(tmpStr.Left(2));			// 분(minute)
		dSec = atof(tmpStr.Right(2));			// 초(second)

		// 설정된 값을 검사
		if ( IsInRangeLongitude( (BYTE) nDeg, (BYTE) nMin, (BYTE) dSec) == FALSE ) {
			if ( bSet ) Empty();
			return -1; // 범위 에러
		}
		if ( bSet ) {
			this->m_degLon = nDeg;	// 도(degree)
			this->m_minLon = nMin;	// 분(minute)
			this->m_secLon = dSec;	// 초(second)
		}
	}
	else // 위도
	{
		if ( length < MIN_LEN_LAT || MAX_LEN_LAT < length )
			return -1;	// 위도의 숫자 길이가 맞지 않음

		tmpStr = digitStr.Right(4);

		nDeg = (UINT) atoi(digitStr.Left(length-4));	// 도(degree)
		nMin = (UINT) atoi(tmpStr.Left(2));			// 분(minute)
		dSec = atof(tmpStr.Right(2));			// 초(second)

		// 설정된 값을 검사
		if ( IsInRangeLatitude( (BYTE) nDeg, (BYTE) nMin, (BYTE) dSec) == FALSE ) {
			if ( bSet )Empty();
			return -1; // 범위 에러
		}
		if ( bSet ) {
			this->m_degLat = nDeg;	// 도(degree)
			this->m_minLat = nMin;	// 분(minute)
			this->m_secLat = dSec;	// 초(second)
		}
	}

	// 문자열에 소숫점 이하의 자리수가 있는 경우, 초의 소수점 이하를 처리한다.
	if ( periodInx != -1 )
	{
		length = str.GetLength();
		digitStr = str.Right(length - periodInx - 1);	// 소숫점 이하 문자열
		if ( !IsDigit(digitStr) )
			return FALSE;	// 문자열에 숫자가 아닌 문자를 포함한 경우

		// 소숫점을 포함한 문자열로 다시 읽음
		digitStr = str.Right(length - periodInx);
		if ( bSet )
		{
			double tmpSec;
			if ( bIsLon )	// 경도
			{
				tmpSec = atof(digitStr);
				this->m_secLon += tmpSec;	// 초(second)
			}
			else	// 위도
			{
				tmpSec = atof(digitStr);
				this->m_secLat += tmpSec;	// 초(second)
			}
		}
	}
	return 1;
}


// 경도 값을 검사하는 함수
// FALSE : 도, 분, 초 범위 에러
// TRUE  : 정상적으로 수행
BOOL CGStringGP::IsInRangeLongitude(BYTE nDeg, BYTE nMin, BYTE nSec)
{
	if ( nDeg < MIN_RANGE || MAX_RANGE_LON < nDeg )
		return FALSE;	// 도 범위 에러
	if ( nMin < MIN_RANGE || MAX_RANGE_MIN < nMin )
		return FALSE;	// 분 범위 에러
	if ( nSec < MIN_RANGE || MAX_RANGE_SEC < nSec )
		return FALSE;	// 초 범위 에러
	return TRUE;
}

// 위도 값을 검사하는 함수
// FALSE : 도, 분, 초 범위 에러
// TRUE  : 정상적으로 수행
BOOL CGStringGP::IsInRangeLatitude(BYTE nDeg, BYTE nMin, BYTE nSec)
{
	if ( nDeg < MIN_RANGE || MAX_RANGE_LAT < nDeg )
		return FALSE;	// 도 범위 에러
	if ( nMin < MIN_RANGE || MAX_RANGE_MIN < nMin )
		return FALSE;	// 분 범위 에러
	if ( nSec < MIN_RANGE || MAX_RANGE_SEC < nSec )
		return FALSE;	// 초 범위 에러
	return TRUE;
}

BOOL CGStringGP::SetPoint(CString &str)
{
	if ( IsValid(str, TRUE) < 0 ) {
		this->Empty();
		return FALSE;
	}
	return TRUE;
}

BOOL CGStringGP::SetPoint(CGPointGP *pGP)
{
	double lon, lat;
	UINT nDeg, nMin;
	double dSec;

	lon = pGP->m_P.lon;		lat = pGP->m_P.lat;
	
	// 경도를 설정한다.
	if ( lon >= 0.0 )
		this->m_EasWes = EAST;	// 동경
	else {
		this->m_EasWes = WEST;	// 서경
		lon = lon * (-1);		// 음수를 양수로 바꾼다.
	}
	
	nDeg = (UINT) ( lon );		// 도(degree)의 정수값
	lon  = lon - (double) nDeg;	// 도(degree)의 소수점이하

	lon *= 60.0;
	nMin = (UINT) lon;			// 분(minute)의 정수값
	lon  = lon - (double) nMin;	// 분(minute)의 소수점이하
	
	dSec = lon * 60.0;			// 초(second)의 값(소수이하 포함)

	this->m_degLon = nDeg;	// 도
	this->m_minLon = nMin;	// 분
	this->m_secLon = dSec;	// 초

	// 위도를 설정한다.
	if ( lat >= 0.0 )
		this->m_NorSou = NORTH;	// 북위
	else {
		this->m_NorSou = SOUTH;	// 남위
		lat = lat * (-1);		// 음수를 양수로 바꾼다.
	}
	
	nDeg = (UINT) ( lat );		// 도(degree)의 정수값
	lat  = lat - (double) nDeg;	// 도(degree)의 소수점이하

	lat *= 60.0;
	nMin = (UINT) lat;			// 분(minute)의 정수값
	lat  = lat - (double) nMin;	// 분(minute)의 소수점이하
	
	dSec = lat * 60.0;			// 초(second)의 값(소수이하 포함)

	this->m_degLat = nDeg;	// 도
	this->m_minLat = nMin;	// 분
	this->m_secLat = dSec;	// 초

	return TRUE;
}

void CGStringGP::GetPoint(CGPointGP *pGP)
{
	double	lon, lat;
	lon = lat = 0.0;

	// 경도 값을 계산한다.
	lon =    (double) this->m_degLon;			// 도(degree)값
	lon += ( (double) this->m_minLon / 60.0 );	// 분(minute)
	lon += this->m_secLon / 3600.0;				// 초(second)값

	if ( this->m_EasWes == WEST ) lon *= (-1);	// 경도가 서경인 경우

	// 위도 값을 계산한다.
	lat =    (double) this->m_degLat;			// 도(degree)값
	lat += ( (double) this->m_minLat / 60.0 );	// 분(minute)
	lat += this->m_secLat / 3600.0;				// 초(second)값

	if ( this->m_NorSou == SOUTH ) lat *= (-1);	// 위도가 서경인 경우

	pGP->SetPoint(lon, lat);
}

CString CGStringGP::GetLongitude(int nDecPoint) const
{
	CString lonStr;
	CString degStr, minStr, secStr, hemiStr;

	// 경도를 문자열로 변환한다.
	switch ( m_EasWes ) {
	case EAST : hemiStr = "E"; break;
	case WEST : hemiStr = "W"; break;
	}

	degStr.Format("%03d",   this->m_degLon);
	minStr.Format("%02d",   this->m_minLon);
	secStr.Format("%06.3f", this->m_secLon);

	// 반올림 - 시작
	switch(nDecPoint)	
	{
	case 1 :	// 소수 둘째자리에서 반올림
		secStr.Format("%04.1f", this->m_secLon);	break;
	case 2 :
		secStr.Format("%05.2f", this->m_secLon);	break;
	case 3 :
		secStr.Format("%06.3f", this->m_secLon);	break;
	case 0 :
	default :	// 소수 첫째자리에서 반올림
		secStr.Format("%02.0f", this->m_secLon);	break;
	}
	// 반올림 - 마지막

	BOOL bAddMin, bAddDeg;
	bAddMin = bAddDeg = FALSE;

	if (secStr.Left(2) == "60")
	{
		secStr = "00.000";
		bAddMin = TRUE;
	}
	if (bAddMin)
	{
		minStr.Format("%02d",   this->m_minLon + 1);
		if (minStr == "60")
		{
			minStr = "00";
			bAddDeg = TRUE;
		}
	}
	if (bAddDeg)
		degStr.Format("%03d",   this->m_degLon + 1);

	switch(nDecPoint)	// 반올림을 막음 ==> 해지함
	{
	case 1 :
		secStr = secStr.Left(4);
		break;
	case 2 :
		secStr = secStr.Left(5);
		break;
	case 3 :
		secStr = secStr.Left(6);
		break;
	case 0 :
	default :
		secStr = secStr.Left(2);
		break;
	}

	lonStr = degStr + minStr + secStr + hemiStr;

	return lonStr;
}

CString CGStringGP::GetLatitude(int nDecPoint) const
{
	CString latStr;
	CString degStr, minStr, secStr, hemiStr;

	// 위도를 문자열로 변환한다.
	switch ( m_NorSou ) {
	case NORTH : hemiStr = "N"; break;
	case SOUTH : hemiStr = "S"; break;
	}

	degStr.Format("%02d",   this->m_degLat);
	minStr.Format("%02d",   this->m_minLat);
	secStr.Format("%06.3f", this->m_secLat);

	// 반올림 - 시작
	switch(nDecPoint)	
	{
	case 1 :	// 소수 둘째자리에서 반올림
		secStr.Format("%04.1f", this->m_secLat);	break;
	case 2 :
		secStr.Format("%05.2f", this->m_secLat);	break;
	case 3 :
		secStr.Format("%06.3f", this->m_secLat);	break;
	case 0 :
	default :	// 소수 첫째자리에서 반올림
		secStr.Format("%02.0f", this->m_secLat);	break;
	}
	// 반올림 - 마지막

	BOOL bAddMin, bAddDeg;
	bAddMin = bAddDeg = FALSE;

	if (secStr.Left(2) == "60")
	{
		secStr = "00.000";
		bAddMin = TRUE;
	}
	if (bAddMin)
	{
		minStr.Format("%02d",   this->m_minLat + 1);
		if (minStr == "60")
		{
			minStr = "00";
			bAddDeg = TRUE;
		}
	}
	if (bAddDeg)
		degStr.Format("%02d",   this->m_degLat + 1);

	switch(nDecPoint)	// 반올림을 막음 ==> 해지됨
	{
	case 1 :
		secStr = secStr.Left(4);
		break;
	case 2 :
		secStr = secStr.Left(5);
		break;
	case 3 :
		secStr = secStr.Left(6);
		break;
	case 0 :
	default :
		secStr = secStr.Left(2);
		break;
	}

	latStr = degStr + minStr + secStr + hemiStr;

	return latStr;
}

CString	CGStringGP::GetString(int nDecPoint) const
{
	CString lonStr, latStr;
	lonStr = this->GetLongitude(nDecPoint);
	latStr = this->GetLatitude(nDecPoint);

	return (lonStr + latStr);
}

BOOL CGStringGP::operator==(CGStringGP right) const
{
	CString gpLeftStr, gpRightStr;
	gpLeftStr = this->GetString();
	gpRightStr = right.GetString();

	if ( gpLeftStr != gpRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGStringGP::operator!=(CGStringGP right) const
{
	CString gpLeftStr, gpRightStr;
	gpLeftStr = this->GetString();
	gpRightStr = right.GetString();

	if ( gpLeftStr == gpRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGStringGP::Add(int nDeg, int nMin, double dSec, UINT nLonLat)
{
	CGStringGP bkStrGP = *this;

	BOOL bFlag = TRUE;
	if (nLonLat < GLONGITUDE || GBOTH < nLonLat) bFlag = FALSE;

	int nMinTemp = 0, nDegTemp = 0;

	// 1단계 이상을 더하거나 빼는 경우는 고려하지 않았음	***** 중요 *****
	if (nLonLat == GLONGITUDE)
	{
		this->m_secLon += dSec;	// 초를 더한다.
		if (this->m_secLon < 0.0)	// 초 단위가 0.0 보다 작을 경우
		{
			nMinTemp--;	// 1 분을 뺌
			this->m_secLon += 60.0;
		}
		else if (this->m_secLon >= 60.0) // 초가 60.0 보다 큰 경우
		{
			nMinTemp++;	// 1 분을 더함
			this->m_secLon -= 60.0;
		}

		nMinTemp += this->m_minLon;	// 분을 더한다.
		nMinTemp += nMin;

		if (nMinTemp < 0)	// 분이 0 보다 작은 경우
		{
			nDegTemp--;	// 1 도를 뺌
			this->m_minLon = nMinTemp + 60;
		}
		else if (nMinTemp >= 60)	// 분이 60 보다 큰 경우
		{
			nDegTemp++;	// 1 도를 더함
			this->m_minLon = nMinTemp - 60;
		}
		else
			this->m_minLon = nMinTemp;

		nDegTemp += this->m_degLon;
		nDegTemp += nDeg;	// 도를 더한다.

		if (nDegTemp < 0)	// 동경, 서경이 바뀐 경우
		{
			if (this->m_EasWes == EAST)
				this->m_EasWes = WEST;
			else
				this->m_EasWes = EAST;

			this->m_degLon = nDegTemp * (-1);
		}
		else
			this->m_degLon = nDegTemp;
	}

	// 1단계 이상을 더하거나 빼는 경우는 고려하지 않았음	***** 중요 *****
	else if (nLonLat == GLATITUDE)
	{
		this->m_secLat += dSec;	// 초를 더한다.
		if (this->m_secLat < 0.0)	// 초 단위가 0.0 보다 작을 경우
		{
			nMinTemp--;	// 1 분을 뺌
			this->m_secLat += 60.0;
		}
		else if (this->m_secLat >= 60.0) // 초가 60.0 보다 큰 경우
		{
			nMinTemp++;	// 1 분을 더함
			this->m_secLat -= 60.0;
		}

		nMinTemp += this->m_minLat;	// 분을 더한다.
		nMinTemp += nMin;

		if (nMinTemp < 0)	// 분이 0 보다 작은 경우
		{
			nDegTemp--;	// 1 도를 뺌
			this->m_minLat = nMinTemp + 60;
		}
		else if (nMinTemp >= 60)	// 분이 60 보다 큰 경우
		{
			nDegTemp++;	// 1 도를 더함
			this->m_minLat = nMinTemp - 60;
		}
		else
			this->m_minLat = nMinTemp;

		nDegTemp += this->m_degLat;
		nDegTemp += nDeg;	// 도를 더한다.

		if (nDegTemp < 0)	// 남위 북위가 바뀐 경우
		{
			if (this->m_NorSou == NORTH)
				this->m_NorSou = SOUTH;
			else
				this->m_NorSou = NORTH;

			this->m_degLat = nDegTemp * (-1);
		}
		else
			this->m_degLat = nDegTemp;
	}
	else if (nLonLat == GBOTH)
	{
		this->Add(nDeg, nMin, dSec, GLONGITUDE);
		this->Add(nDeg, nMin, dSec, GLATITUDE);
	}

	if (!bFlag) *this = bkStrGP;
	
	return bFlag;
}

BOOL CGStringGP::Add(int nUnit, UINT nPart, UINT nLonLat)
{
	BOOL bFlag = TRUE;
	if (nLonLat < GLONGITUDE || GBOTH < nLonLat) bFlag = FALSE;

	if (nPart == GDEG)
		this->Add(nUnit, 0, 0.0, nLonLat);
	else if (nPart == GMIN)
		this->Add(0, nUnit, 0.0, nLonLat);
	else if (nPart == GSEC)
		this->Add(0, 0,   nUnit, nLonLat);
	else
		bFlag = FALSE;
	
	return bFlag;	
}
void CGStringGP::Clear(int nUnit, UINT nPart, UINT nLonLat)
{
	CGStringGP bkStrGP = *this;
	if ((nLonLat < GLONGITUDE || GBOTH < nLonLat)  || (nPart < GDEG || GSEC < nPart))
	{
		*this = bkStrGP;
		return;
	}
	
	if (nLonLat == GLONGITUDE)
	{
		switch(nPart)
		{
		case GDEG :
			this->m_degLon /= nUnit;
			this->m_degLon *= nUnit;
			this->m_minLon = 0;
			this->m_secLon = 0.0;
			break;
		case GMIN :
			this->m_minLon /= nUnit;
			this->m_minLon *= nUnit;
			this->m_secLon = 0.0;
			break;
		case GSEC :
			this->m_secLon = (double) ((int) this->m_secLon / nUnit) * nUnit;
			break;
		}
	}
	else if (nLonLat == GLATITUDE)
	{
		switch(nPart)
		{
		case GDEG :
			this->m_degLat /= nUnit;
			this->m_degLat *= nUnit;
			this->m_minLat = 0;
			this->m_secLat = 0.0;
			break;
		case GMIN :
			this->m_minLat /= nUnit;
			this->m_minLat *= nUnit;
			this->m_secLat = 0.0;
			break;
		case GSEC :
			this->m_secLat = (double) ((int) this->m_secLat / nUnit) * nUnit;
			break;
		}
	}
	else if (nLonLat == GBOTH)
	{
		this->Clear(nUnit, nPart, GLONGITUDE);
		this->Clear(nUnit, nPart, GLATITUDE);
	}
}
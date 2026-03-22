// GStringMGRS.cpp : implementation file
//

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoUTM.h"
#include "GGeoGP.h"

#include "GStringGeo.h"
#include "GStringMGRS.h"

#include "GEllipsoid.h"
#include "GGeoTrans.h"

#include <math.h>	// fmod()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGStringMGRS

CGStringMGRS::CGStringMGRS() : CGStringGeo(GSTYPE_MGRS)
{
	this->Empty();
}

CGStringMGRS::~CGStringMGRS()
{
}

CGStringMGRS::CGStringMGRS(CString str, int eInx) : CGStringGeo(GSTYPE_MGRS)
{
	if ( !this->SetPoint(str, eInx) ) {
		this->Empty();
	}
}

// MGRS좌표를 값의 의미를 갖지 않도록 만든다.
void CGStringMGRS::Empty()
{
	// BYTE	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	m_zone = 0;	
	
	// BYTE	// 위도 : 80도S - 84도N 구간을 8도 간격으로 구분하는 문자
	m_dgn = 'A';

	m_xch = 'A';// 100,000 Meter MGRS Square의 East  Designator
	m_x = -1.0;	// 100,000 Meter를 더 세분하여 나눈 East  Meter

	m_ych = 'A';// 100,000 Meter MGRS Square의 North Designator
	m_y = -1.0;	// 100,000 Meter를 더 세분하여 나눈 North Meter

	m_nUnit = 0;
	m_ellipse = EWGE;	// default : WGS 84
}

BOOL CGStringMGRS::IsEmpty() const
{
	if (this->m_zone == 0)
		if (this->m_dgn == 'A')
			if (this->m_xch == 'A')
				if (this->m_ych == 'A')
					if (this->m_x == -1.0)
						if (this->m_y == -1.0) return TRUE;
	return FALSE;
}

const CGStringMGRS &CGStringMGRS::operator=(const CGStringMGRS &right)
{
	if (&right != this)
	{
		this->m_zone = right.m_zone;	
		this->m_dgn  = right.m_dgn;

		this->m_xch  = right.m_xch;
		this->m_x    = right.m_x;
		this->m_ych  = right.m_ych;
		this->m_y    = right.m_y;

		this->m_nUnit   = right.m_nUnit;
		this->m_ellipse = right.m_ellipse;
	}
	return	*this;
}

/////////////////////////////////////////////////////////////////////////////
// CGStringMGRS operations

BOOL CGStringMGRS::IsValidDesignator(BYTE dgn)
{
	dgn = (BYTE) toupper(dgn);

	if ( dgn < 'C' || 'X' < dgn ) return FALSE;
	if ( dgn == 'I' || dgn == 'O') return FALSE;
	return TRUE;
}

BOOL CGStringMGRS::IsValid100KmChar(BYTE ch, int nAxis, int nZone)
{
	if ( !( 0 < nZone && nZone <= 60 ) ) return FALSE;

	BYTE chMin, chMax;
	int nMod = nZone % 3;

	ch = (BYTE) toupper(ch);
	switch (nAxis)
	{
	case MGRS_EASTING :
		{
			if (nMod == 1)
			{
				chMin = 'A';
				chMax = 'H';
			}
			else if (nMod == 2)
			{
				chMin = 'J';
				chMax = 'R';
			}
			else
			{
				chMin = 'S';
				chMax = 'Z';
			}
		}
		break;
	case MGRS_NORTHING :
		chMin = 'A';
		chMax = 'V';
		break;
	default :
		ASSERT(FALSE);
		return FALSE;
	}
	if ( chMin > ch || ch > chMax ) return FALSE;
	if ( ch == 'I' || ch == 'O') return FALSE;
	return TRUE;
}

int CGStringMGRS::IsValid(CString &str, int eInx, BOOL bSet)
{
	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();	// Removes newline, space, and tab characters
	tmpStr.TrimRight();	// Removes newline, space, and tab characters

	// MGRS 좌표의 식별자가 있는가를 조사한다.
	if (tmpStr.Left(1) == "M")
		tmpStr = tmpStr.Right(tmpStr.GetLength() - 1);

	// 유효한 MGRS 문자열인가를 검사하고,
	// 유효한 문자열이면 숫자 표시 단계를 리턴하고,
	// 유효하지 않은 문자열이면 0을 리턴한다.

	// ZZYXXxxxxxyyyyy
	//
	//	ZZ : zone 번호
	//	Y  : 위도 구역 식별자
	//	XX : 경도 구역 식별자
	//	xxxxx : 기준으로부터 경도 방향으로 거리, 1 ~ 5 자리수
	//	yyyyy : 기준으로부터 위도 방향으로 거리. 1 ~ 5 자리수
	// % xxxxx와 yyyyy는 자리수가 같아야 한다.

	int length = tmpStr.GetLength();
	int strInx = 1;

	int	inx;
	int	add;

	// 문자열의 첫 글자가 숫자가 아니면 에러로 FALSE를 리턴한다.
	if ( length < strInx ) return -1;
	if ( !isdigit(tmpStr[0]) ) return FALSE;

	if ( length < ++strInx ) return -1;
	if ( isdigit(tmpStr[1]) ) {	// zone 값이 두자리
		inx = 2;
		add = 0;
	}
	else {			// zone 값이 한자리
		inx = 1;
		add = 100;
		strInx--;
	}

	BYTE	zone;	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	BYTE	dgn;	// // 위도 : 80도S - 84도N 구간을 8도 간격으로 구분하는 문자
	BYTE	xch;	// 100,000 Meter MGRS Square의 East  Designator
	double	x;		// 100,000 Meter를 더 세분하여 나눈 East  Meter
	BYTE	ych;	// 100,000 Meter MGRS Square의 North Designator
	double	y;		// 100,000 Meter를 더 세분하여 나눈 North Meter

	// Zone의 범위 검사
	CString		strZone;
	strZone = tmpStr.Left(inx);	// 구역(zone)의 값을 읽어온다.
	
	zone = (BYTE) atoi( (LPCTSTR)strZone );
	// Zone값이 0과 60사이 값이 아니면 에러로 FALSE를 리턴한다.
	if( !( 0 < zone && zone <= 60 ) ) return 0;
	
	//
	// check designator
	if ( length < ++strInx ) return -1;
	if ( !IsValidDesignator(tmpStr[inx++]) ) return 0;
	else dgn = tmpStr[inx - 1];	// 위도 식별자

	// easting character, northing character check
	if ( length < ++strInx ) return -1;
	if ( !IsValid100KmChar(tmpStr[inx++], MGRS_EASTING, zone) ) return 0;
	else xch = tmpStr[inx - 1];	// 100km 동경 구역

	if ( length < ++strInx ) return -1;
	if ( !IsValid100KmChar(tmpStr[inx++], MGRS_NORTHING, zone) ) return 0;
	else ych = tmpStr[inx - 1];	// 100km 북향 구역
	
	CString		strDigit;
	CString		strX, strY;
	int		nLength;

	nLength = tmpStr.GetLength() - inx;	// 숫좌열의 길이를 계산함
	if ( nLength > GUNIT_MGRS_LAST || nLength < GUNIT_MGRS_FIRST || (nLength % 2) != 0) {
		// 숫자의 갯수가 맞지 않음.
		return 0;
	}

	// 숫자열에 숫가 아닌 문자가 들어 있는지 검사한다.
	strDigit = tmpStr.Right(nLength);
	for (inx = 0; inx < nLength; inx++)
	{
		if ( !isdigit(strDigit[inx]) ) {
			return 0;
		}
	}
	// 이상으로 표현방법에 대한 검사는 끝났다.

	// 이제는 실제 좌표가 맞는가를 검사한다.
	x = ::atof(strDigit.Left(nLength / 2));
	y = ::atof(strDigit.Right(nLength / 2));
	x = x * (100000 * ::pow(0.1, nLength / 2));
	y = y * (100000 * ::pow(0.1, nLength / 2));

	CGStringMGRS mgrsTmpStr;

	mgrsTmpStr.m_ellipse = eInx;
	mgrsTmpStr.m_nUnit = (BYTE) nLength;
	mgrsTmpStr.m_zone = zone;
	mgrsTmpStr.m_dgn = dgn;
	mgrsTmpStr.m_xch = xch;
	mgrsTmpStr.m_ych = ych;
	mgrsTmpStr.m_x = x;
	mgrsTmpStr.m_y = y;
	
	CGGeoTrans trans(eInx);
	CGPointUTM utmPoint;
	CGPointGP gpPoint;
	mgrsTmpStr.GetPoint(&utmPoint, eInx);	// MGRS를 UTM으로 변환
	trans.GeoConvert(&gpPoint, &utmPoint, 1);	// UTM을 지리좌표로 변환
	trans.GeoConvert(&utmPoint, &gpPoint, 1);	// 지리좌표를 UTM으로 변환
	
	if (utmPoint.m_P.dgn != mgrsTmpStr.m_dgn) return -1;
	else if (bSet)
		*this = mgrsTmpStr;

	return 1;
}

BOOL CGStringMGRS::SetPoint(CString &str, int eInx)
{
	if ( IsValid(str, eInx, TRUE) <= 0 ) {
		this->Empty();
		return FALSE;
	}
	return TRUE;
}

BOOL CGStringMGRS::SetPoint(CGPointUTM *pUtm, int eInx)
{
	// set ellipsoid
	m_ellipse = eInx;

	this->m_zone = pUtm->m_P.zone;	// 범위 : 1 - 60 (경도 6도 간격)
	this->m_dgn  = pUtm->m_P.dgn;	// 위도 : 80도S - 84도N 구간을 8도 간격으로 구분하는 문자 

	BYTE EcharArray[24] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
							'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R',
							'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

	int index;
	index = (int) (pUtm->m_P.x / 100000); // atoi( eastingstr.Left(1) );
	switch( index )
	{
	case  0: break;
	case  9: index = index - 2; break;
	default: index = index - 1; break;
	}
	
	// 100,000 Meter MGRS Square의 easting character
	switch ( (pUtm->m_P.zone % 3) )
	{
	case 1:	                    break;	// zone 번호를 3으로 나눠 나머지가 1 인경우 
	case 2:	index = index +  8;	break;	// zone 번호를 3으로 나눠 나머지가 2 인경우 
	case 0:	index = index + 16;	break;	// zone 번호를 3으로 나눠 나머지가 0 인경우 
	}
	this->m_xch = EcharArray[index];

	// 100,000 Meter를 더 세분하여 나눈 easting meter
	this->m_x = fmod(pUtm->m_P.x, 100000.0);  // atol( eastingstr.Right(5) );
	

	BYTE WGS84NcharArrayOddZone[20]
					= { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
						'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V' };
	BYTE WGS84NcharArrayEvenZone[20]
					= { 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
						'R', 'S', 'T', 'U', 'V', 'A', 'B', 'C', 'D', 'E' };
	BYTE BesselNcharArrayOddZone[20]
					= { 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
						'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K' };
	BYTE BesselNcharArrayEvenZone[20]
					= { 'R', 'S', 'T', 'U', 'V', 'A', 'B', 'C', 'D', 'E',
						'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q' };

	double northing;
	if ( pUtm->m_P.y >= 0.0 ) northing = pUtm->m_P.y;	// North
	else northing = pUtm->m_P.y + UTM_FALSE_NORTHING;		// South

	index = (int) (northing / 100000);
	index = ( index % 20 );

	// 100,000 Meter MGRS Square의 northing character
	switch ( m_ellipse )
	{
	case EBES :	// Bessel 1841
		this->m_ych = ( (pUtm->m_P.zone % 2) == 1 )
					? BesselNcharArrayOddZone[index]
					: BesselNcharArrayEvenZone[index];
		break;
	case EWGE :	// WGS 84
	default :
		this->m_ych = ( (pUtm->m_P.zone % 2) == 1 )
					? WGS84NcharArrayOddZone[index]
					: WGS84NcharArrayEvenZone[index];
		break;
	}
	// 100,000 Meter를 더 세분하여 나눈 northing meter
	this->m_y = fmod(northing, 100000.0);

	this->m_nUnit = GUNIT_MGRS_10;

	return TRUE;
}

CString CGStringMGRS::GetString(int nUnit) const	// 0, 2, 4, 6, 8 10, 12
{
	ASSERT((GUNIT_MGRS_FIRST <= nUnit && nUnit <= GUNIT_MGRS_LAST
									  && (nUnit % 2) == 0) || nUnit == -1);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!((GUNIT_MGRS_FIRST <= nUnit && nUnit <= GUNIT_MGRS_LAST
									  && (nUnit % 2) == 0) || nUnit == -1)) return _T("");

	CString		mgrsStr;

	// 기존에 지정되어 있는 갯수를 사용한다.
	if (nUnit == -1) nUnit = this->m_nUnit;

	CString zoneStr, eastStr, northStr;

	zoneStr.Format("%02d", this->m_zone);
	eastStr.Format("%08.1f",  this->m_x * 10);
	northStr.Format("%08.1f", this->m_y * 10);

	switch(nUnit)
	{
	case GUNIT_MGRS_12 : //	12	// 52S CJ 712431 091941	: 0.1  미터 단위(12단계)
		eastStr.Format("%06.0f",  this->m_x * 10);	// 소수 둘째자리서 반올림
		northStr.Format("%06.0f", this->m_y * 10);
		eastStr  = eastStr.Left(6);
		northStr = northStr.Left(6);
		break;
	case GUNIT_MGRS_8 : //	8	// 52S CJ 7124 0919		: 10   미터 단위(8 단계)
		eastStr  = eastStr.Left(4);
		northStr = northStr.Left(4);
		break;
	case GUNIT_MGRS_6 : //	6	// 52S CJ 712 091		: 100  미터 단위(6 단계)
		eastStr  = eastStr.Left(3);
		northStr = northStr.Left(3);
		break;
	case GUNIT_MGRS_4 : //	4	// 52S CJ 71 09			: 1000 미터 단위(4 단계)
		eastStr  = eastStr.Left(2);
		northStr = northStr.Left(2);
		break;
	case GUNIT_MGRS_2 : //	2	// 52S CJ 7 0			: 1만  미터 단위(2 단계)
		eastStr  = eastStr.Left(1);
		northStr = northStr.Left(1);
		break;
	case GUNIT_MGRS_0 : //	0	// 52S CJ				: 10만 미터 단위(0 단계)
		eastStr  = eastStr.Left(0);
		northStr = northStr.Left(0);
		break;
	case GUNIT_MGRS_10 : //	10	// 52S CJ 71243 09194	: 1    미터 단위(10단계)
	default:
		eastStr.Format("%05.0f",  this->m_x);	// 소수 첫째자리서 반올림
		northStr.Format("%05.0f", this->m_y);
		eastStr  = eastStr.Left(5);
		northStr = northStr.Left(5);
		break;
	}

	mgrsStr.Format("%s%c%c%c%s%s",
			zoneStr, this->m_dgn, this->m_xch, this->m_ych,	eastStr, northStr);

	return mgrsStr;
}

CString	CGStringMGRS::Get100kmSquare() const
{
	CString str = this->GetString(GUNIT_MGRS_0);
	return str.Right(2);
}

void CGStringMGRS::GetPoint(CGPointUTM *pUtm, int eInx)
{
	// 설정된 타원체의 값이 다르면 Epmty() 실행하고 종료한다.
	if (m_ellipse != eInx) {
		Empty();
		return;
	}

	pUtm->m_P.zone = this->m_zone;
	pUtm->m_P.dgn  = this->m_dgn; // ( dgn >= 'N' ) ? NORTH : SOUTH;
	
	BYTE EcharArray[24] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
							'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R',
							'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	int index = 0;
	for ( ; ( EcharArray[index] != this->m_xch ) && ( index < 24 ); index++ );
	if ( index < 24 )
		index = ( index % 8 ) + 1;	// UTM좌표중 easting의 10만 단위의 숫자 
	else
	{
		pUtm->m_P.zone = 0;
		return;
	}
	pUtm->m_P.x = ( (double)index * 100000.0 ) + (double)this->m_x; // mgrs->eastmeter;

	switch (this->m_dgn)
	{
	case 'C': index =  9;	break;
	case 'D': index = 18;	break;
	case 'E': index = 27;	break;
	case 'F': index = 35;	break;
	case 'G': index = 44;	break;

	case 'H': index = 53;	break;
	case 'J': index = 62;	break;
	case 'K': index = 71;	break;
	case 'L': index = 80;	break;
	case 'M': index = 89;	break;

	case 'N': index =  0;	break;
	case 'P': index =  6;	break;
	case 'Q': index = 15;	break;
	case 'R': index = 24;	break;
	case 'S': index = 33;	break;

	case 'T': index = 42;	break;
	case 'U': index = 51;	break;
	case 'V': index = 60;	break;
	case 'W': index = 69;	break;
	case 'X': index = 77;	break;
	default:
		pUtm->m_P.zone = 0;
		return;
	}

	BYTE WGS84NcharArrayOddZone[20]
						= { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
							'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V' };
	BYTE WGS84NcharArrayEvenZone[20]
						= { 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
							'R', 'S', 'T', 'U', 'V', 'A', 'B', 'C', 'D', 'E' };
	BYTE BesselNcharArrayOddZone[20]
						= { 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
							'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K' };
	BYTE BesselNcharArrayEvenZone[20]
						= { 'R', 'S', 'T', 'U', 'V', 'A', 'B', 'C', 'D', 'E',
							'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q' };

	switch ( m_ellipse )
	{
	case EBES :
		switch ( this->m_zone % 2 )
		{
		case 1:	
			for ( ; ( BesselNcharArrayOddZone[(index%20)] != this->m_ych ); index++);
			break;
		case 0:
			for ( ; ( BesselNcharArrayEvenZone[(index%20)] != this->m_ych ); index++ );
			break;
		}
		break;
	case EWGE :
	default :
		switch ( this->m_zone % 2 )
		{
		case 1:	
			for ( ; ( WGS84NcharArrayOddZone[(index%20)] != this->m_ych ); index++ );
			break;
		case 0:
			for ( ; ( WGS84NcharArrayEvenZone[(index%20)] != this->m_ych ); index++ );
			break;
		}
		break;
	}

	pUtm->m_P.y = ( (double)(index) * 100000.0 ) + (double)this->m_y;
	if ( this->m_dgn < 'N' ) pUtm->m_P.y -= UTM_FALSE_NORTHING;
}

BOOL CGStringMGRS::operator==(CGStringMGRS right) const
{
	// 타원체가 다른 경우 
	if ( this->m_ellipse != right.m_ellipse ) return FALSE;

	CString mgrsLeftStr, mgrsRightStr;
	mgrsLeftStr = this->GetString(GUNIT_MGRS_10);
	mgrsRightStr = right.GetString(GUNIT_MGRS_10);

	if ( mgrsLeftStr != mgrsRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGStringMGRS::operator!=(CGStringMGRS right) const
{
	// 타원체가 다른 경우 
	if ( this->m_ellipse == right.m_ellipse ) return FALSE;

	CString mgrsLeftStr, mgrsRightStr;
	mgrsLeftStr = this->GetString(GUNIT_MGRS_10);
	mgrsRightStr = right.GetString(GUNIT_MGRS_10);

	if ( mgrsLeftStr == mgrsRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

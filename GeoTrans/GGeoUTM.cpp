// GGeoUTM.cpp: implementation of the CGGeoUTM class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"
#include "GGeoAbst.h"
#include "GGeoUTM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGSizeUTM
CGSizeUTM::CGSizeUTM() : CGSizeGeo(GPTYPE_UTM)
{
	m_S.cx = 0.0;
	m_S.cy = 0.0;
	m_S.zone = 0;
	m_S.zone = '?';
}

CGSizeUTM::CGSizeUTM(double cx, double cy, char zone, char dgn) : CGSizeGeo(GPTYPE_UTM)
{
	m_S.cx = cx;
	m_S.cy = cy;
	m_S.zone = zone;
	m_S.zone = dgn;
}

CGSizeUTM::~CGSizeUTM()
{
}

void CGSizeUTM::SetSize(double cx, double cy)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	m_S.cx = cx;
	m_S.cy = cy;
}

void CGSizeUTM::SetSize(CGSizeGeo *pSize)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	ASSERT(pSize->m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize->m_nPointType==GPTYPE_UTM)) return;

	CGSizeUTM	*pUTM = (CGSizeUTM *) pSize;
	m_S.cx = pUTM->m_S.cx;
	m_S.cy = pUTM->m_S.cy;
	m_S.zone = pUTM->m_S.zone;
	m_S.dgn = pUTM->m_S.dgn;
}

void CGSizeUTM::SetSize(SIZEUTM *pSize)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	m_S.cx = pSize->cx;
	m_S.cy = pSize->cy;
	m_S.zone = pSize->zone;
	m_S.dgn = pSize->dgn;
}

void CGSizeUTM::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	ASSERT(pOffset->m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset->m_nPointType==GPTYPE_UTM)) return;

	CGSizeUTM	*pUTM = (CGSizeUTM *) pOffset;
	m_S.cx += pUTM->m_S.cx;
	m_S.cy += pUTM->m_S.cy;
}

void CGSizeUTM::Offset(SIZEUTM *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_S.cx += pOffset->cx;
	m_S.cy += pOffset->cy;
}

const CGSizeUTM &CGSizeUTM::operator=(const CGSizeUTM &right)
{
	if(&right!=this) {
		this->m_S.cx = right.m_S.cx;
		this->m_S.cy = right.m_S.cy;
		this->m_S.zone = right.m_S.zone;
		this->m_S.dgn = right.m_S.dgn;
	}
	return	*this;
}

/////////////////////////////////////////////////////////////////////////////
// CGPointUTM
CGPointUTM::CGPointUTM() : CGPointGeo(GPTYPE_UTM)
{
	this->Empty();
}

CGPointUTM::CGPointUTM(double x, double y, char zone, char dgn) : CGPointGeo(GPTYPE_UTM)
{
	m_P.x = x;
	m_P.y = y;
	m_P.zone = zone;
	m_P.dgn = dgn;
}

CGPointUTM::~CGPointUTM()
{
}

void CGPointUTM::Empty()
{
	this->m_P.x = 0.0;
	this->m_P.y = 0.0;
	this->m_P.zone = 0;
	this->m_P.dgn = '?';
}

BOOL CGPointUTM::IsEmpty() const
{
	if (this->m_P.x == 0.0)
		if (this->m_P.y == 0.0)
			if (this->m_P.zone == 0)
				if (this->m_P.dgn == '?') return TRUE;

	return FALSE;
}

void CGPointUTM::SetPoint(CGPointGeo *pPoint)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	ASSERT(pPoint->m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint->m_nPointType==GPTYPE_UTM)) return;

	CGPointUTM	*pUTM = (CGPointUTM *) pPoint;

	m_P.x = pUTM->m_P.x;
	m_P.y = pUTM->m_P.y;
	m_P.zone = pUTM->m_P.zone;
	m_P.dgn = pUTM->m_P.dgn;
};

void CGPointUTM::SetPoint(POINTUTM *pPoint)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	m_P.x = pPoint->x;
	m_P.y = pPoint->y;
	m_P.zone = pPoint->zone;
	m_P.dgn = pPoint->dgn;
};

void CGPointUTM::SetPoint(char zone, char dgn,
				  double easting, double northing, VERHEMISPHERE hemisphere)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	m_P.zone = zone;
	m_P.dgn = dgn;
	m_P.x = easting;
	m_P.y = northing;
	if ( hemisphere == SOUTH ) m_P.y -= UTM_FALSE_NORTHING;
};

void CGPointUTM::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	ASSERT(pOffset->m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset->m_nPointType==GPTYPE_UTM)) return;

	CGSizeUTM	*pUTM = (CGSizeUTM *) pOffset;
	m_P.x += pUTM->m_S.cx;
	m_P.y += pUTM->m_S.cy;
};

void CGPointUTM::Offset(SIZEUTM *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_P.x += pOffset->cx;
	m_P.y += pOffset->cy;
};

const CGPointUTM &CGPointUTM::operator=(const CGPointUTM &right)
{
	if(&right!=this) {
		this->m_P.x = right.m_P.x;
		this->m_P.y = right.m_P.y;
		this->m_P.zone = right.m_P.zone;
		this->m_P.dgn = right.m_P.dgn;
	}
	return	*this;
}

int CGPointUTM::IsValid(CString &str, BOOL bSet)
{
	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();	// Removes newline, space, and tab characters

	if (str.GetLength() <= 3) return -1;

	// GK 좌표의 식별자가 있는가를 조사한다.
	if (tmpStr.Left(1) == "U" || tmpStr.Left(1) == "T")
		tmpStr = tmpStr.Right(tmpStr.GetLength() - 1);

	int length = tmpStr.GetLength();
	if (length < 6) return -1;	// UTM 좌표의 최소 길이

	// UTM zone을 검사한다.
	int zone = ::atoi(tmpStr.Left(2));
	if (zone < 1 || zone > 60) return -1;

	// 위도 식별자(designator)를 검사한다.
	char dgn = tmpStr[2];
	if ( dgn < 'C'  || 'X' < dgn ) return -1;
	if ( dgn == 'I' || dgn == 'O') return -1;

	// 모두 숫자열로 되어 있는가를 검사한다.
	CString digitStr = tmpStr.Right(length - 3);	// zone가 designator 제외
	if (!this->IsDigit(digitStr)) 
		return -1;	// 숫자열 이외의 문자가 포함된 에러

	// UTM 좌표의 숫자 자리수 에러 검사, 최소길이 : 3, 최대길이 : 15
	int digitLength = digitStr.GetLength();
	if ((digitLength % 2) == 0) return -1;
	if (digitLength < 3 || digitLength > 15) return -1;

	if (bSet)
	{
		int eastStrLength = digitLength / 2;
		int northStrLength = digitLength - eastStrLength;
		double easting, northing;
		easting  = ::atof(digitStr.Left(eastStrLength));
		northing = ::atof(digitStr.Right(northStrLength));

		int nUnit = eastStrLength - 1;

		double Multiple = 100000;
		for (int i = 0; i < nUnit; i++) Multiple /= 10.0;
		
		this->m_P.zone = (char) zone;
		this->m_P.dgn = dgn;
		this->m_P.x = easting * Multiple;
		this->m_P.y = (dgn >= 'N') ? (northing * Multiple) : (northing * Multiple - UTM_FALSE_NORTHING);
	}
	return 1;
}

CString CGPointUTM::GetString(int nDecPoint) const
{
	ASSERT(0 <= nDecPoint && nDecPoint <= 3);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(0 <= nDecPoint && nDecPoint <= 3)) return _T("");

	double northing = 0.0;
	char nChar;
	if ( this->m_P.y >= 0.0 ) {	// North
		northing = this->m_P.y;
		nChar = 'N';
	}
	else {	// South
		northing = this->m_P.y + UTM_FALSE_NORTHING;
		nChar = 'S';
	}

	CString eastStr, northStr;

	eastStr.Format("%011.4f",  this->m_P.x);
	northStr.Format("%012.4f", northing);

	// 반올림 - 시작
	switch (nDecPoint)
	{
	case 1:	// 소수 둘째자리에서 반올림
		eastStr.Format("%08.1f",  this->m_P.x);
		northStr.Format("%09.1f", northing);	break;
	case 2:
		eastStr.Format("%09.2f",  this->m_P.x);
		northStr.Format("%010.2f", northing);	break;
	case 3:
		eastStr.Format("%010.3f",  this->m_P.x);
		northStr.Format("%011.3f", northing);	break;
	case 0:
	default:	// 소수 첫째자리에서 반올림
		eastStr.Format("%06.0f",  this->m_P.x);
		northStr.Format("%07.0f", northing);	break;
	}
	// 반올림 - 끝

	CString utmStr;
	if (nDecPoint == 0)
	{
		utmStr.Format("%02d%1c,%s,%s,%1c",
				this->m_P.zone, this->m_P.dgn, eastStr.Left(6), northStr.Left(7), nChar);
	}
	else
	{
		utmStr.Format("%02d%1c,%s,%s,%1c",
				this->m_P.zone, this->m_P.dgn, eastStr.Left(6+1+nDecPoint), northStr.Left(7+1+nDecPoint), nChar);
	}

	return utmStr;
}

BOOL CGPointUTM::operator==(CGPointUTM& right) const
{
	CString utmLeftStr, utmRightStr;
	utmLeftStr = this->GetString();
	utmRightStr = right.GetString();

	if ( utmLeftStr != utmRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

BOOL CGPointUTM::operator!=(CGPointUTM& right) const
{
	CString utmLeftStr, utmRightStr;
	utmLeftStr = this->GetString();
	utmRightStr = right.GetString();

	if ( utmLeftStr == utmRightStr ) return FALSE;	// 값이 다른 경우

	return TRUE;
}

void CGPointUTM::Offset(double x, double y, int nUnit)
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

/////////////////////////////////////////////////////////////////////////////
// CGRectUTM
CGRectUTM::CGRectUTM() : CGRectGeo(GPTYPE_UTM)
{
	m_R.left = 0.0;
	m_R.top = 0.0;
	m_R.right = 0.0;
	m_R.bottom = 0.0;
	m_R.zone = 0;
	m_R.dgn = '?';
}

CGRectUTM::CGRectUTM(double left, double top, double right, double bottom,
				char zone, char dgn) : CGRectGeo(GPTYPE_UTM)
{
	m_R.left = left;
	m_R.top = top;
	m_R.right = right;
	m_R.bottom = bottom;
	m_R.zone = zone;
	m_R.dgn = dgn;
}

CGRectUTM::~CGRectUTM()
{
}

void CGRectUTM::SetRect(CGRectGeo *pRect)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	CGRectUTM	*pUTM = (CGRectUTM *) pRect;

	m_R.left = pUTM->m_R.left;
	m_R.top = pUTM->m_R.top;
	m_R.right = pUTM->m_R.right;
	m_R.bottom = pUTM->m_R.bottom;
	m_R.zone = pUTM->m_R.zone;
	m_R.dgn = pUTM->m_R.dgn;
}

void CGRectUTM::SetRect(RECTUTM *pRect)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	m_R.left = pRect->left;
	m_R.top = pRect->top;
	m_R.right = pRect->right;
	m_R.bottom = pRect->bottom;
	m_R.zone = pRect->zone;
	m_R.dgn = pRect->dgn;
}

void CGRectUTM::Offset(CGSizeGeo *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	CGSizeUTM	*pUTM = (CGSizeUTM *) pOffset;

	m_R.left += pUTM->m_S.cx;
	m_R.top += pUTM->m_S.cy;
	m_R.right += pUTM->m_S.cx;
	m_R.bottom += pUTM->m_S.cy;
}

void CGRectUTM::Offset(SIZEUTM *pOffset)
{
	ASSERT(m_nPointType==GPTYPE_UTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_nPointType==GPTYPE_UTM)) return;

	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;

	m_R.left += pOffset->cx;
	m_R.top += pOffset->cy;
	m_R.right += pOffset->cx;
	m_R.bottom += pOffset->cy;
}

double CGRectUTM::Width()
{
	return m_R.right - m_R.left;
}

double CGRectUTM::Height()
{
	return m_R.top - m_R.bottom;
}


const CGRectUTM &CGRectUTM::operator=(const CGRectUTM &right)
{
	if(&right!=this) {
		this->m_R.left = right.m_R.left;
		this->m_R.top = right.m_R.top;
		this->m_R.right = right.m_R.right;
		this->m_R.bottom = right.m_R.bottom;
		this->m_R.zone = right.m_R.zone;
		this->m_R.dgn = right.m_R.dgn;
	}
	return	*this;
}


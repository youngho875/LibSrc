// GStringNE.cpp: implementation of the CGStringNE class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"

#include "GStringGeo.h"
#include "GStringGP.h"

#include "GStringNE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static int g_nMaxN = 0;	// N 좌표의 개수
static int g_nMaxE = 0;	// E 좌표의 개수
static NETABLE	*g_pNeTable = NULL;
static NETABLE	g_NeTable[500][GMAX_MONTH];
static CString	g_sBaseGP;

CGStringNE::CGStringNE() : CGStringGeo(GSTYPE_NE)
{
	this->Empty();
	
	this->InitializeNeTable();	// NE 파일에서 읽어서 환자표를 초기화한다.
}

void CGStringNE::InitializeNeTable()
{
	CString			tm_sAppDate;	// 적용일
	CString			tm_sBaseGP;		// 기준지리돠표

	CStringArray	tm_sArrN[12];	// N 좌표
	CStringArray	tm_sArrE[12];	// E 좌표

	CGStringGP		gpTemp;

	if (g_pNeTable == NULL)
	{
		this->m_bRead = ReadNeTableFormFile(&tm_sAppDate, &tm_sBaseGP, &g_nMaxN, &g_nMaxE, tm_sArrN, tm_sArrE);
		g_pNeTable = &g_NeTable[0][0];

		g_sBaseGP = tm_sBaseGP;
		int nCnt, nMonth;
		NETABLE	*pTempNeTable = g_pNeTable;
		int nMax;
		nMax = (g_nMaxN > g_nMaxE) ? g_nMaxN : g_nMaxE;

		if (this->m_bRead)
		{
			for (nCnt = 0; nCnt < nMax; nCnt++)
			{
				for (nMonth = 0; nMonth < GMAX_MONTH; nMonth++)
				{
					::strcpy(pTempNeTable->Nchar, tm_sArrN[nMonth].GetAt(nCnt));
					::strcpy(pTempNeTable->Echar, tm_sArrE[nMonth].GetAt(nCnt));
					++pTempNeTable;
				}
			}
		}
	}
	else 
	{
		this->m_bRead = TRUE;
	}

	gpTemp.SetPoint(g_sBaseGP);
	gpTemp.GetPoint(&m_gpBasis);
}

CGStringNE::~CGStringNE()
{
}

NETABLE* CGStringNE::GetNeTable()
{
	if (this->m_bRead == FALSE)
		return NULL;
//	return &__neTable__[0][0];	// ** 변경됨 **
//	return &m_NeTable[0][0];
	return g_pNeTable;
}

void CGStringNE::Empty()
{
	CGStringGP		gpTemp;
	gpTemp.SetPoint(g_sBaseGP);
	gpTemp.GetPoint(&m_gpBasis);

	CTime t = CTime::GetCurrentTime();		// NE좌표의 기준시각
	this->m_nMonth = t.GetMonth();

	this->m_bRead = FALSE;

	this->m_Narea = 0;
	this->m_Nsec  = 0;
	this->m_Earea = 0;
	this->m_Esec  = 0;

	this->m_expNE = GEXP_NE_POS;

	m_mile = 0;
	m_Ncount = 0;
	m_Ecount = 0;
}

BOOL CGStringNE::IsEmpty() const
{
	if (this->m_Narea == 0)
		if (this->m_Nsec == 0)
			if (this->m_Earea == 0)
				if (this->m_Esec == 0) return TRUE;
	return FALSE;
}

CGPointGP CGStringNE::GetBasisGP() const
{
	return this->m_gpBasis;
}

CGPointGP CGStringNE::SetBasisGP(CGPointGP gpBasis)
{
	CGPointGP prevBasisGP(this->m_gpBasis);
	this->m_gpBasis = gpBasis;
	return prevBasisGP;
}

const CGStringNE& CGStringNE::operator = (const CGStringNE &right)
{
	if (&right != this)
	{
		this->m_gpBasis = right.m_gpBasis;
		this->m_nMonth = right.m_nMonth;

		this->m_Narea = right.m_Narea;
		this->m_Nsec  = right.m_Nsec;
		this->m_Earea = right.m_Earea;
		this->m_Esec  = right.m_Esec;
	}
	return	*this;
}

int	CGStringNE::IsValid(CString &str, BOOL bSet)
{
	if (this->m_bRead == FALSE)
		return -1;

	CString tmpStr(str);
	tmpStr.MakeUpper();	// 대문자로 변환한다.
	tmpStr.TrimLeft();		// Removes newline, space, and tab characters
	tmpStr.TrimRight();	// Removes newline, space, and tab characters

	int nValidPos = this->IsValidPos(tmpStr, bSet);

	return nValidPos;
}

int	CGStringNE::IsValidPos(CString &str, BOOL bSet)
{
	if (this->m_bRead == FALSE)
		return -1;

	int length = str.GetLength();

	// 위치표시 길이가 맞는가를 검사한다.(11자리, 5자리)
	if (!(length == 11 || length == 5)) return -1;

	if (str.Find('-') == -1) return -2;// 위치표기법이 틀림. N과 E좌표의 구분이 없음
	
	CString strNarea, strNsec, strEarea, strEsec;
	if (length == 11)
	{
		strNarea = str.Left(2);
		strNsec  = str.Mid(2, 3);
		strEarea = str.Mid(6, 2);
		strEsec  = str.Right(3);
	}
	else if (length == 5)
	{
		strNarea = str.Left(2);
		strNsec  = "000";
		strEarea = str.Right(2);
		strEsec  = "000";
	}

	if (!IsAlpha(strNarea)) return -3;	// N좌표 10분 구간에 영문자 이외의 문자가 포함
	if (!IsDigit(strNsec)) return -4;	// N좌표 10분 미만에 숫자 이외의 문자열 포함
	if (!IsAlpha(strEarea)) return -5;	// E좌표 10분 구간에 영문자 이외의 문자가 포함
	if (!IsDigit(strEsec)) return -6;	// E좌표 10분 미만에 숫자 이외의 문자열 포함

	if (strNsec[1] < '0' || '6' < strNsec[1]) return -7;	// N좌표의 숫자열의 초 범위 에러
	if (strEsec[1] < '0' || '6' < strEsec[1]) return -8;	// E좌표의 숫자열의 초 범위 에러

	int month = this->m_nMonth;

	int Narea, Earea;
	Narea = this->FindNEtable(strNarea, TRUE, month);
	if (Narea <= 0) return -9;	// N좌표 문자열이 NE환자표에 없음

	Earea = this->FindNEtable(strEarea, FALSE, month);
	if (Earea <= 0) return -10;	// E좌표 문자열이 NE환자표에 없음

	if (bSet)
	{
		this->m_Narea = (BYTE) Narea;
		this->m_Nsec  = ::atoi(strNsec);
		this->m_Earea = (BYTE) Earea;
		this->m_Esec  = ::atoi(strEsec);

		this->m_expNE = GEXP_NE_POS;
	}

	return GEXP_NE_POS;
}

int	CGStringNE::IsValidArea(CString &str, BOOL bSet)
{
	if (this->m_bRead == FALSE)
		return -1;

	int length = str.GetLength();

	if (length < 4) return -1; // 구역을 나타내는 문자열의 길이가 충분하지 않음

	int areaType = 0;
	if (length == 4)
		areaType = GEXP_NE_AREA_BASE;	// 기본구역
	else if (str[2] != '-')
		areaType = GEXP_NE_AREA_RADIUS;	// 임의구역(반경)
	else if (str[2] != '(')
		areaType = GEXP_NE_AREA_RECT;	// 임의구역(사각영역)
	else
		return -1;	// NE좌표의 구역 문자열이 아님

	CString strNarea, strEarea;
	CString strNcnt, strEcnt, strMile;
	int indexFirst, indexLast;

	strNarea = str.Left(2);
	switch (areaType)
	{
	case GEXP_NE_AREA_BASE :	// 기본구역
		strEarea = str.Right(2);
		break;
	case GEXP_NE_AREA_RADIUS :	// 임의구역(반경), 구분자는 '-'
		if (length < 7) return -1; // 구역을 나타내는 문자열의 길이가 충분하지 않음
		if (str[5] != '-') return -1;

		strEarea = str.Mid(3, 2);
		strMile = str.Right(length - 6);
		break;
	case GEXP_NE_AREA_RECT :	// 임의구역(사각영역), 구분자는 '('
		if (str[length - 1] != ')') return -1;	// 마지막 문자 검사
		indexFirst = str.Find(')');
		if (indexFirst <= 4 || indexFirst == length - 1) return -1;
		indexLast = str.ReverseFind('(');
		if (indexLast != indexFirst + 3) return -1;
		
		strEarea = str.Mid(indexFirst, 2);
		strNcnt = str.Mid(3, indexFirst - 3);
		strEcnt = str.Mid(indexLast + 1, length - indexLast - 2);
		break;
	}

	if (!IsAlpha(strNarea)) return -3;	// N좌표 10분 구간에 영문자 이외의 문자가 포함
	if (!IsAlpha(strEarea)) return -4;	// E좌표 10분 구간에 영문자 이외의 문자가 포함
	switch (areaType)
	{
	case GEXP_NE_AREA_RADIUS : // 임의구역(반경)
		if (!IsDigit(strMile)) return -5;
		break;
	case GEXP_NE_AREA_RECT :	// 임의구역(사각영역)
		if (!IsDigit(strNcnt)) return -6;
		if (!IsDigit(strEcnt)) return -7;
		break;
	}

	int month = this->m_nMonth;

	int Narea, Earea;
	Narea = this->FindNEtable(strNarea, TRUE, month);
	if (Narea <= 0) return -8;	// N좌표 문자열이 NE환자표에 없음

	Earea = this->FindNEtable(strEarea, FALSE, month);
	if (Earea <= 0) return -9;	// E좌표 문자열이 NE환자표에 없음

	if (bSet)
	{
		this->m_Narea = (BYTE) Narea;
		this->m_Nsec  = 0;
		this->m_Earea = (BYTE) Earea;
		this->m_Esec  = 0;

		this->m_expNE = areaType;
		
		switch (areaType)
		{
		case GEXP_NE_AREA_RADIUS : // 임의구역(반경)
			this->m_mile = ::atoi(strMile);
			break;
		case GEXP_NE_AREA_RECT :	// 임의구역(사각영역)
			this->m_Ncount = ::atoi(strNcnt);
			this->m_Ecount = ::atoi(strEcnt);
			break;
		}
	}
	return areaType;
}

int	CGStringNE::IsValidRefLine(CString &str, BOOL bSet)
{
	if (this->m_bRead == FALSE)
		return -1;

	str = str;
	bSet = bSet;
	return GEXP_NE_REFLINE;
}

// NE환자표에서 해당 문자열을 검색하여 구역번호를 리턴하는 함수
int CGStringNE::FindNEtable(CString str, BOOL bN, int month)
{
	if (this->m_bRead == FALSE)
		return -1;

	// 파리미터 검사
	if (str.GetLength() != 2) return -1;	// NE 환자 문자열 길이가 맞지 않음
	if (!IsAlpha(str)) return -2;	// NE 환자 문자열에 영문자 이외의 문자를 포함

	if (month < 0 || 12 < month) return -3; // 월의 범위를 벗어남
	if (month == 0) month = this->m_nMonth;

	BOOL bFlag = FALSE;
	int i, maxAreaNum;
//	maxAreaNum = (bN) ? GMAX_NE_N : GMAX_NE_E;
	maxAreaNum = (bN) ? g_nMaxN : g_nMaxE;

	NETABLE *pTempNeTable = g_pNeTable;
	NETABLE *pPrevNeTable = g_pNeTable;
	
	for (i = 0; i < maxAreaNum; i++)
	{	// 테이블은 0 에서 11 까지의 인덱스를 사용한다.

		pTempNeTable = pPrevNeTable;
		pTempNeTable += (month - 1);

		if (bN)	// N 좌표인 경우
		{
//			if (str == __neTable__[i][month - 1].Nchar)	// ** 변경됨 **
//			if (str == this->m_NeTable[i][month - 1].Nchar)
//			if (str == this->m_pNeTable[i][month - 1]->Nchar)
			if (str == pTempNeTable->Nchar)
			{
				bFlag = TRUE;
				break;
			}
			else
				pPrevNeTable += GMAX_MONTH;
		}
		else	// E 좌표인 경우
		{
//			if (str == __neTable__[i][month - 1].Echar)	// ** 변경됨 **
//			if (str == this->m_NeTable[i][month - 1].Echar)
//			if (str == this->m_pNeTable[i][month - 1]->Echar)
			if (str == pTempNeTable->Echar)
			{
				bFlag = TRUE;
				break;
			}
			else
				pPrevNeTable += GMAX_MONTH;
		}
	}
	i++;

	if (!bFlag) return -4;	// NE환자표에서 문자열을 찾지 못함
	else return i;
}

BOOL CGStringNE::SetPoint(CGPointGP *pGP)
{
	if (this->m_bRead == FALSE)
		return -1;

	if (pGP == NULL)
		return FALSE;

	double tmpht = 999.0;
	if (pGP->m_P.ht == 998.0)
	{	// 좌표범위 검사를 위한 영역 검사
		pGP->m_P.ht = tmpht;

		if( pGP->m_P.lon >= m_gpBasis.m_P.lon && pGP->m_P.lon <= (m_gpBasis.m_P.lon + (10.0 * (g_nMaxE) / 60) - 0.0000001) &&
			pGP->m_P.lat >= m_gpBasis.m_P.lat && pGP->m_P.lat <= (m_gpBasis.m_P.lat + (10.0 * (g_nMaxN) / 60) - 0.0000001) )
			return TRUE;
		else 
			return FALSE;
	}
	else if (pGP->m_P.ht == 997.0)
	{	// 문자를 도시하기 위한 영역 검사
		pGP->m_P.ht = tmpht;

		if( pGP->m_P.lon >= m_gpBasis.m_P.lon && pGP->m_P.lon <= (m_gpBasis.m_P.lon + (10.0 * (g_nMaxE) / 60) - 0.0000001) &&
			pGP->m_P.lat >= m_gpBasis.m_P.lat && pGP->m_P.lat <= (m_gpBasis.m_P.lat + (10.0 * (g_nMaxN) / 60) - 0.0000001) )
			return TRUE;
		else 
			return FALSE;
	}
	else if (pGP->m_P.ht == 996.0)
	{	// 위도를 도시하기 위한 경도 범위 검사
		pGP->m_P.ht = tmpht;

		if( pGP->m_P.lon >= (m_gpBasis.m_P.lon - 0.00001) && 
			pGP->m_P.lon <= (m_gpBasis.m_P.lon + (10.0 * (g_nMaxE) / 60) + 0.00001) )
			return TRUE;
		else 
			return FALSE;
	}
	else if (pGP->m_P.ht == 995.0)
	{	// 위도를 도시하기 위한 최저, 최고의 위도 값 결정
		pGP->m_P.ht = tmpht;

		if (pGP->m_P.lat >= (m_gpBasis.m_P.lat + (10.0 * (g_nMaxN) / 60)))
		{
			pGP->m_P.lat = m_gpBasis.m_P.lat + (10.0 * (g_nMaxN) / 60);
			return TRUE;
		}
		if (pGP->m_P.lat <= m_gpBasis.m_P.lat)
		{
			pGP->m_P.lat = m_gpBasis.m_P.lat;
			return TRUE;
		}
		return FALSE;
	}
	else if (pGP->m_P.ht == 994.0)
	{	// 경도를 도시하기 위한 위도 범위 검사
		pGP->m_P.ht = tmpht;

		if( pGP->m_P.lat >= (m_gpBasis.m_P.lat - 0.00001) && 
			pGP->m_P.lat <= (m_gpBasis.m_P.lat + (10.0 * (g_nMaxN) / 60) + 0.00001) )
			return TRUE;
		else 
			return FALSE;
	}
	else if (pGP->m_P.ht == 993.0)
	{	// 경도를 도시하기 위한 최저, 최고의 경도 값 결정
		pGP->m_P.ht = tmpht;

		if (pGP->m_P.lon >= (m_gpBasis.m_P.lon + (10.0 * (g_nMaxE) / 60)))
		{
			pGP->m_P.lon = m_gpBasis.m_P.lon + (10.0 * (g_nMaxE) / 60);
			return TRUE;
		}
		if (pGP->m_P.lon <= m_gpBasis.m_P.lon)
		{
			pGP->m_P.lon = m_gpBasis.m_P.lon;
			return TRUE;
		}
		return FALSE;
	}

	// NE좌표의 영역을 벗어났는가를 검사한다.
	CGSizeGP gpSize((10.0 * (g_nMaxE) / 60), (10.0 * (g_nMaxN) / 60));	// N 좌표의 범위 7.5 도, E 좌표의 범위 10.0 도

	// 영역 검사을 위해 추가 ( 상단, 오른쪽 1구역씩 추가 )
	gpSize.m_S.clat -= 0.0000001;
	gpSize.m_S.clon -= 0.0000001;

	CGRectGP gpRect(this->m_gpBasis, gpSize);
	gpRect.NormalizeRect();

	if ((pGP->m_P.lon < gpRect.m_R.left) || (gpRect.m_R.right < pGP->m_P.lon) )
	{
		this->Empty();
		return FALSE;	// NE 좌표의 영역을 벗어난 경우
	}
	if ((pGP->m_P.lat < gpRect.m_R.bottom) || (gpRect.m_R.top < pGP->m_P.lat) )
	{
		this->Empty();
		return FALSE;	// NE 좌표의 영역을 벗어난 경우
	}

	CGStringGP strBaseGP, strGP;
	strBaseGP.SetPoint(&this->m_gpBasis);
	strGP.SetPoint(pGP);

	CString gpBasisStr, gpStr;
	gpBasisStr = strBaseGP.GetString(0);// 소숫점 없음
	gpStr = strGP.GetString(0);			// 소숫점 없음  예) 1273053E385520N

	int	nCurPos, nBasic;

	nCurPos = ( ::atoi( gpStr.Mid(8,2) ) * 6 ) + ( ::atoi( gpStr.Mid(10,1) ) );
	nBasic  = ( ::atoi( gpBasisStr.Mid(8,2) ) * 6 ) + ( ::atoi( gpBasisStr.Mid(10,1) ) );
	this->m_Narea	= (BYTE) ( nCurPos - nBasic + 1 );		// BYTE
	this->m_Nsec	= (UINT) ::atoi( gpStr.Mid(11, 3) );	// UINT

	nCurPos = ( ::atoi( gpStr.Mid(0,3) ) * 6 ) + ( ::atoi( gpStr.Mid(3,1) ) );
	nBasic  = ( ::atoi( gpBasisStr.Mid(0,3) ) * 6 ) + ( ::atoi( gpBasisStr.Mid(3,1) ) );
	this->m_Earea	= (BYTE) ( nCurPos - nBasic + 1 );		// BYTE
	this->m_Esec	= (UINT) ::atoi( gpStr.Mid(4, 3) );		// UINT

	if (this->m_Narea < 1 || g_nMaxN < this->m_Narea)
	{
		this->Empty();
		return FALSE;	// NE 좌표의 영역을 벗어난 경우
	}
	if (this->m_Earea < 1 || g_nMaxE < this->m_Earea)
	{
		this->Empty();
		return FALSE;	// NE 좌표의 영역을 벗어난 경우
	}

	return TRUE;
}

BOOL CGStringNE::SetPoint(CString &str)
{
	if (this->m_bRead == FALSE)
		return FALSE;

	if (IsValid(str, TRUE) <= 0)
	{
		this->Empty();
		return FALSE;
	}

	return TRUE;
}

void CGStringNE::GetPoint(CGPointGP *pGP)
{
	if (this->m_bRead == FALSE)
		return;

	if (this->IsEmpty())
	{
		pGP->Empty();
		return;
	}

	CGStringGP strBaseGP;
	strBaseGP.SetPoint(&this->m_gpBasis);

	CString gpBasisStr;
	gpBasisStr = strBaseGP.GetString(0);// 소숫점 없음	예) 1273053E385520N

	int	nCurPos, nBasis;

	// 위도 계산
//	nCurPos = ( ::atoi( gpStr.Mid(8,2) ) * 6 ) + ( ::atoi( gpStr.Mid(10,1) ) );
	nBasis  = ( ::atoi( gpBasisStr.Mid(8,2) ) * 6 ) + ( ::atoi( gpBasisStr.Mid(10,1) ) );
//	this->m_Narea	= (BYTE) ( nCurPos - nBasis + 1 );		// BYTE
	nCurPos = (int) this->m_Narea + nBasis - 1;
//	this->m_Nsec	= (UINT) ::atoi( gpStr.Mid(11, 3) );	// UINT
	CString strLat;
	strLat.Format("%02d%01d%03dN", (nCurPos / 6), (nCurPos % 6), this->m_Nsec);

	// 경도 계산
//	nCurPos = ( ::atoi( gpStr.Mid(0,3) ) * 6 ) + ( ::atoi( gpStr.Mid(3,1) ) );
	nBasis  = ( ::atoi( gpBasisStr.Mid(0,3) ) * 6 ) + ( ::atoi( gpBasisStr.Mid(3,1) ) );
//	this->m_Earea	= (BYTE) ( nCurPos - nBasis + 1 );		// BYTE
	nCurPos = (int) this->m_Earea + nBasis - 1;
//	this->m_Esec	= (UINT) ::atoi( gpStr.Mid(4, 3) );		// UINT
	CString strLon;
	strLon.Format("%03d%01d%03dE", (nCurPos / 6), (nCurPos % 6), this->m_Esec);

	CGStringGP tmpStrGP(strLon + strLat);
	if (tmpStrGP.IsEmpty())
		pGP->Empty();
	else
		tmpStrGP.GetPoint(pGP);
}

CString CGStringNE::GetString()
{
	if (this->m_bRead == FALSE)
		return "";

	// NE 좌표가 설정되지 않은 경우는 널 문자열을 리턴한다.
	if (this->IsEmpty()) return CString("");

	int month = this->m_nMonth;
	CString NareaStr, NsecStr, EareaStr, EsecStr;
	CString retStr;

	NETABLE	*pTempNeTable = NULL;
	
	pTempNeTable = g_pNeTable;
//	NareaStr = (CString) __neTable__[m_Narea - 1][month - 1].Nchar;	// ** 변경됨 **
//	NareaStr = (CString) this->m_NeTable[m_Narea - 1][month - 1].Nchar;
//	NareaStr = (CString) this->m_pNeTable[m_Narea - 1][month - 1]->Nchar;
	pTempNeTable += (GMAX_MONTH * (m_Narea - 1));
	pTempNeTable += (month-1);
	NareaStr = (CString) pTempNeTable->Nchar;
	NsecStr.Format("%03d", this->m_Nsec);

	pTempNeTable = g_pNeTable;
//	EareaStr = (CString) __neTable__[m_Earea - 1][month - 1].Echar;	// ** 변경됨 **
//	EareaStr = (CString) this->m_NeTable[m_Earea - 1][month - 1].Echar;
//	EareaStr = (CString) this->m_pNeTable[m_Earea - 1][month - 1]->Echar;
	pTempNeTable += (GMAX_MONTH * (m_Earea - 1));
	pTempNeTable += (month-1);
	EareaStr = (CString) pTempNeTable->Echar;
	EsecStr.Format("%03d", this->m_Esec);

	retStr = NareaStr + NsecStr + "-" + EareaStr + EsecStr;

	return retStr;
}

int CGStringNE::SetMonth(int nMonth)
{
	if (this->m_bRead == FALSE)
		return -1;

	int nPreMonth = this->m_nMonth;

	if (1 <= nMonth && nMonth <= 12)
		this->m_nMonth = nMonth;
	else
	{	// 현재 월로 설정한다.
		CTime t = CTime::GetCurrentTime();		// NE좌표의 기준시각
		this->m_nMonth = t.GetMonth();
	}		

	return nPreMonth;
}

BOOL CGStringNE::ReadNeTableFormFile(CString *psAppDate, CString *psBaseGP, int *pnCntN, int *pnCntE, CStringArray *psArrN, CStringArray *psArrE)
{
	CStdioFile fIn;
	BOOL	bOpen = FALSE;
/*
	CGEnvVar env("SGIS_FILE_BASE");
	CString strNe;
	strNe = env.GetValue() + "\\data\\mapinfo\\Netable.nec";
	bOpen = fIn.Open(strNe, CFile::modeRead | CFile::shareDenyNone);

	if (!bOpen)
	{
		TRACE0("netable.nec 파일 열기에 실패하였습니다.(파일 없음)");
		return FALSE;
	}

	BOOL bRead = FALSE;
	BOOL bErr = FALSE;

	bRead = fIn.ReadString(*psAppDate);

	if (psAppDate->GetLength() == 48)
	{
		*psBaseGP = psAppDate->Mid(24, 15);		// 기준지리돠표
		*pnCntN = ::atoi(psAppDate->Mid(43, 2));	// N 좌표의 갯수
		*pnCntE = ::atoi(psAppDate->Mid(46, 2));	// E 좌표의 갯수
		*psAppDate = psAppDate->Mid(5, 12);
	}
	else if (psAppDate->GetLength() == 50)
	{
		*psBaseGP = psAppDate->Mid(24, 15);		// 기준지리돠표
		*pnCntN = ::atoi(psAppDate->Mid(43, 3));	// N 좌표의 갯수
		*pnCntE = ::atoi(psAppDate->Mid(47, 3));	// E 좌표의 갯수
		*psAppDate = psAppDate->Mid(5, 12);
	}
	else	// if (psAppDate->GetLength() != 48 || psAppDate->GetLength() != 50)
	{
		TRACE0("netable.nec 파일 열기에 실패하였습니다.(파일포멧 다름)");
		return FALSE;
	}

	if (psAppDate->GetLength() == 12)
	{
		CString strTmp;
		strTmp.Empty();
		int i = 0;
		
		bRead = fIn.ReadString(strTmp);
		while (bRead && !bErr)
		{
			if (strTmp.GetLength() == 48 || strTmp.GetLength() == 51)
			{
				// N 좌표 추가
				if (strTmp.GetLength() == 51)
					strTmp = strTmp.Mid(3);

				for (i = 0; i < 12; i++)
				{
					psArrN[i].Add (strTmp.Mid(i * 4,     2));
					psArrE[i].Add (strTmp.Mid(i * 4 + 2, 2));
				}
			}
			else
			{
				bErr = TRUE;
				continue;
			}
			
			strTmp.Empty();
			bRead = fIn.ReadString(strTmp);
		}
	}

	fIn.Close();

	if (!bRead && !bErr)
		return TRUE;

	psAppDate->Empty();			// 적용일
	for (int i = 0; i < 12; i++)
	{
		psArrN[i].RemoveAll();	// N 좌표
		psArrE[i].RemoveAll();	// E 좌표
	}
*/

	return FALSE;
}

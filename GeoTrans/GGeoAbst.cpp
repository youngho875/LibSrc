// GGeoAbst.cpp: implementation of the CGGeoAbst class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoAbst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGPointGeo

CGPointGeo::CGPointGeo(int nPointType) : m_nPointType(nPointType)
{
}

CGPointGeo::~CGPointGeo()
{
}

BOOL CGPointGeo::IsDigit(CString &str, BOOL bRealNum)
{
	int length = str.GetLength();
	int count = 0;

	for ( int i = 0; i < length; i++ )
	{
		// 문자열이 모두 숫자가 아니면 FALSE를 리턴한다.
		if ( !isdigit(str[i]) )
		{
			if (!bRealNum) return FALSE;	// 정수만을 검사

			if (str[i] == '.') count++;	// 소수점이 있나를 검사
			else return FALSE;
			if (count > 1) return FALSE; // 소수점이 두개 이상인 경우
		}
	}

	return TRUE;
}

void CGPointGeo::SetPoint(CString* ptStr, int nType, int nSeparator)
{
	ptStr = ptStr;
	nType = nType;
	nSeparator = nSeparator;
}

void CGPointGeo::GetPoint(CString* ptStr, int nType, int nSeparator)
{
	ptStr = ptStr;
	nType = nType;
	nSeparator = nSeparator;
}

/////////////////////////////////////////////////////////////////////////////
// CGSizeGeo

CGSizeGeo::CGSizeGeo(int nPointType) : m_nPointType(nPointType)
{
}

CGSizeGeo::~CGSizeGeo()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGRectGeo

CGRectGeo::CGRectGeo(int nPointType) : m_nPointType(nPointType)
{
}

CGRectGeo::~CGRectGeo()
{
}


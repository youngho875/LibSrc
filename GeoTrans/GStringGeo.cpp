// GStringGeo.cpp: implementation of the CGStringGeo class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GStringGeo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGStringGeo::CGStringGeo(int nPointType) : m_nPointType(nPointType)
{
}

CGStringGeo::~CGStringGeo()
{
}

BOOL CGStringGeo::IsDigit(CString &str)
{
	int length = str.GetLength();
	if (length == 0) return FALSE;

	for ( int i = 0; i < length; i++ ) {
		if ( !isdigit(str[i]) ) FALSE ;	// 문자열이 모두 숫자가 아니면 FALSE를 리턴한다.
	}
	return TRUE;
}

// 문자열이 모두 스트링이면 TRUE를 그렇지 않으면 FALSE를 리턴한다.
BOOL CGStringGeo::IsAlpha(CString &str)
{
	int length = str.GetLength();
	if (length == 0) return FALSE;

	for ( int i = 0; i < length; i++ ) {
		if ( !isalpha(str[i]) ) FALSE ;	// 문자열이 모두 숫자가 아니면 FALSE를 리턴한다.
	}
	return TRUE;
}


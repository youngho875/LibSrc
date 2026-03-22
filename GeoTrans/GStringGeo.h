// GStringGeo.h: interface for the CGStringGeo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

/**
* GEOREF 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringGeo
{
protected:
	CGStringGeo(int nPointType);

public:
	virtual ~CGStringGeo();
	virtual void	Empty() { ASSERT(FALSE); };
	virtual BOOL	IsEmpty() const { ASSERT(FALSE); return FALSE;};

protected:
	virtual BOOL IsDigit(CString &str);
	virtual BOOL IsAlpha(CString &str);

// Attributes
public:
	const int	m_nPointType;	// point type

};

#undef	AFX_DATA
#define	AFX_DATA



// GGeoAbst.h: interface for the CGPointGeo, CGSizeGeo, CGRectGeo class.
//
//////////////////////////////////////////////////////////////////////
#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CGPointGeo

class CGPointGeo;
class CGSizeGeo;
class CGRectGeo;

/**
* GEOREF 좌표계 point 관리 class
* @author 유영호
* @version 1.00
* @since 2024.07.
**/
class AFX_DATA CGPointGeo
{
protected:
	CGPointGeo(int nPointType);	// protected constructor for abstract class
public:
	virtual ~CGPointGeo();

// Attributes
public:
	const int	m_nPointType;	// point type

// operator
public:

// Operations
public:
	virtual void	Offset(CGSizeGeo *) {ASSERT(FALSE);};

	virtual void SetPoint(CGPointGeo *) {ASSERT(FALSE);};
	virtual void SetPoint(CString* ptStr, int nType=GPTYPE_GP, int nSeparator=SEPARATOR_BLANK);
	virtual void GetPoint(CString* ptStr, int nType=GPTYPE_GP, int nSeparator=SEPARATOR_BLANK);

protected:
	virtual BOOL IsDigit(CString &str, BOOL bRealNum = FALSE);
};

/////////////////////////////////////////////////////////////////////////////
// CGSizeGeo

/**
* Geo좌표 size 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGSizeGeo
{
protected:
	CGSizeGeo(int nPointType);	// protected constructor for abstract class

// Attributes
public:
	const int	m_nPointType;	// point type

// operator
public:
//	virtual const CGSizeGeo& operator = (const CGSizeGeo &right) = NULL;
	virtual void	SetSize(double, double) {ASSERT(FALSE);};
	virtual void	SetSize(CGSizeGeo *) {ASSERT(FALSE);};
	virtual void	Offset(CGPointGeo *) {ASSERT(FALSE);};

// Operations
public:
	virtual ~CGSizeGeo();
};

/////////////////////////////////////////////////////////////////////////////
// CGRectGeo

/**
* GEOREF 좌표계 영역 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class CGRectGeo
{
protected:
	AFX_DATA CGRectGeo(int nPointType);	// protected constructor for abstract class

// Attributes
public:
	const int	m_nPointType;	// point type

// operator
public:
//	AFX_DATA virtual const CGRectGeo& operator = (const CGRectGeo &right) = NULL;
	AFX_DATA virtual void	SetRect(CGPointGeo *, CGPointGeo *)	{ASSERT(FALSE);};
	AFX_DATA virtual void	SetRect(CGRectGeo *) {ASSERT(FALSE);};

	AFX_DATA virtual void	Offset(CGPointGeo *) {ASSERT(FALSE);};
	AFX_DATA virtual double	Width() {ASSERT(FALSE); return 0.0;};
	AFX_DATA virtual double	Height() {ASSERT(FALSE); return 0.0;};

// Operations
public:
	AFX_DATA virtual ~CGRectGeo();
};



#undef	AFX_DATA
#define	AFX_DATA



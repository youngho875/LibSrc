// GGeoUTM.h: interface for the CGGeoUTM class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class CGSizeUTM;
class CGPointUTM;
class CGRectUTM;

/////////////////////////////////////////////////////////////////////////////
// CGSizeUTM

/**
* UTM좌표 size 관리 class
* @author 유영호
* @version 1.0
* @since 2005.07.
**/
class AFX_DATA CGSizeUTM : public CGSizeGeo
{
public:
	CGSizeUTM();
	CGSizeUTM(double cx, double cy, char zone, char dgn = '?');
	virtual ~CGSizeUTM();

// Attributes
public:
	SIZEUTM		m_S;

// Operations
public:
	virtual void	SetSize(double cx, double cy);
	virtual void	SetSize(CGSizeGeo *);
	virtual void	SetSize(SIZEUTM *);
	virtual void	Offset(CGSizeGeo *);
	virtual void	Offset(SIZEUTM *);

// Operators
public:
	const CGSizeUTM& operator = (const CGSizeUTM &right);
};

/////////////////////////////////////////////////////////////////////////////
// CGPointUTM

/**
* UTM 좌표계 point 관리 class
* @author 유영호
* @version 1.0
* @since 2005.07.
**/
class AFX_DATA CGPointUTM : public CGPointGeo
{
public:
	CGPointUTM();
	CGPointUTM(double x, double y, char zone, char dgn = '?');
	virtual ~CGPointUTM();

// Attributes
public:
	POINTUTM	m_P;

// Operations
public:
	void Empty();
	BOOL IsEmpty() const;

	virtual void	SetPoint(CGPointGeo *);
	virtual void	SetPoint(POINTUTM *);
	virtual void	SetPoint(char zone, char dgn, double easting,
					double northing, VERHEMISPHERE hemisphere = NORTH);
	virtual void	Offset(CGSizeGeo *);
	virtual void	Offset(SIZEUTM *);

	BOOL operator==(CGPointUTM& right) const;
	BOOL operator!=(CGPointUTM& right) const;

	void Offset(double x, double y, int nUnit = GUNIT_M);

// Operators
public:
	const CGPointUTM& operator = (const CGPointUTM &right);

	CString	GetString(int nDecPoint = 0) const;
	int		IsValid(CString &str, BOOL bSet = FALSE);

};

/////////////////////////////////////////////////////////////////////////////
// CGRectUTM

/**
* UTM 좌표계 영역 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGRectUTM : public CGRectGeo
{
public:
	CGRectUTM();
	CGRectUTM(double left, double top, double right, double bottom,
			char zone, char dgn = '?');
	virtual ~CGRectUTM();

// Attributes
public:
	RECTUTM		m_R;

// Operations
public:
	virtual void	SetRect(CGRectGeo *);
	virtual void	SetRect(RECTUTM *);
	virtual void	Offset(CGSizeGeo *);
	virtual void	Offset(SIZEUTM *);
	virtual double	Width();
	virtual double	Height();

// Operators
public:
	const CGRectUTM& operator = (const CGRectUTM &right);
};


#undef	AFX_DATA
#define	AFX_DATA



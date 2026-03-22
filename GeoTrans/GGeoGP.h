// GGeoGP.h: interface for the CGGeoGP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class CGPointGeo;
class CGSizeGeo;
class CGRectGeo;

class CGPointGP;
class CGSizeGP;
class CGRectGP;

/////////////////////////////////////////////////////////////////////////////
// CGSizeGP

/**
* GP좌표 size 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class CGSizeGP : public CGSizeGeo
{
public:
	AFX_DATA CGSizeGP();
	AFX_DATA CGSizeGP(double clon, double clat);
	AFX_DATA virtual ~CGSizeGP();

// Attributes
public:
	SIZEGP		m_S;

// Operations
public:
	AFX_DATA virtual void	SetSize(double clon, double clat);
	AFX_DATA virtual void	SetSize(CGSizeGeo *);
	AFX_DATA virtual void	SetSize(SIZEGP *);
	AFX_DATA virtual void	Offset(CGSizeGeo *);	// 삭제할 것임
	AFX_DATA virtual void	Offset(SIZEGP *);		// 삭제할 것임

// Operators
public:
	AFX_DATA const CGSizeGP& operator = (const CGSizeGP &right);

	AFX_DATA BOOL operator==(CGSizeGP& sizeGP) const;
	AFX_DATA BOOL operator!=(CGSizeGP& sizeGP) const;

	AFX_DATA void operator+=(CGSizeGP sizeGP);
	AFX_DATA void operator-=(CGSizeGP sizeGP);

	AFX_DATA CGSizeGP  operator+(CGSizeGP sizeGP) const;
	AFX_DATA CGPointGP operator+(CGPointGP gp) const;
	AFX_DATA CGRectGP  operator+(const CGRectGP rectGP) const;

	AFX_DATA CGSizeGP  operator-(CGSizeGP sizeGP) const;
	AFX_DATA CGPointGP operator-(CGPointGP gp) const;

	AFX_DATA CGRectGP operator-(const CGRectGP rectGP) const;
	AFX_DATA CGSizeGP operator-() const;

};

/////////////////////////////////////////////////////////////////////////////
// CGPointGP

/**
* GP 좌표계 point 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/

//#include "GGeoObjStruct.h"

class CGPointGP : public CGPointGeo
{
public:
	AFX_DATA CGPointGP();
	AFX_DATA CGPointGP(double lon, double lat, double height = 0.0);
	AFX_DATA CGPointGP(CGPointGP *initGP);
	AFX_DATA CGPointGP(POINTGP initGP);
	AFX_DATA CGPointGP(CGSizeGP initSizeGP);
	AFX_DATA CGPointGP(SIZEGP initSizeGP);

	AFX_DATA virtual ~CGPointGP();

// Attributes
public:
	POINTGP		m_P;

// Operations
public:
	AFX_DATA virtual void Offset(CGSizeGeo *);
	AFX_DATA virtual void Offset(SIZEGP *);
	AFX_DATA virtual void Offset(double lon, double lat, int nUnit = GUNIT_DEG);
//	void		Offset(double longitudeOffset, double latitudeOffset);
//	void		Offset(POINTGP gp);
//	void		Offset(CGPointGP *gp);

	AFX_DATA BOOL operator==(CGPointGP& gp) const;
	AFX_DATA BOOL operator!=(CGPointGP& gp) const;
	AFX_DATA BOOL operator==(POINTGP gp) const;
	AFX_DATA BOOL operator!=(POINTGP gp) const;

	AFX_DATA virtual void SetPoint(CGPointGeo *pGeo);

	AFX_DATA void SetPoint(CGPointGP *pGP);
	AFX_DATA void SetPoint(POINTGP *gp);
	AFX_DATA void SetPoint(double lon, double lat, double height = 0.0);
	AFX_DATA virtual void SetPoint(CString* ptStr, int nType=GPTYPE_GP, int nSeparator=SEPARATOR_BLANK);
	AFX_DATA virtual void GetPoint(CString* ptStr, int nType=GPTYPE_GP, int nSeparator=SEPARATOR_BLANK);

	AFX_DATA CString GetString(int nDecPoint = -1) const;
	AFX_DATA void Empty();
	AFX_DATA BOOL IsEmpty() const;

protected:
	AFX_DATA CString GetLongitude(int nDecPoint = -1) const;
	AFX_DATA CString GetLatitude(int nDecPoint = -1) const;

// Operators
public:
	AFX_DATA const CGPointGP& operator = (const CGPointGP &right);
	// Operators Returning CGPointGp Values 
	AFX_DATA void operator+=(const CGPointGP gp);
//	void		operator+=(POINTGP gp);
//	void		operator+=(SIZEGP sizeGp);

//	void		operator-=(POINTGP gp);
//	void		operator-=(SIZEGP sizeGp);

//	CGRectGp	operator+(const RECTGP* lpRectGp) const;
//	CGPointGp	operator+(POINTGP gp) const;
//	CGPointGp	operator+(SIZEGP sizeGp) const;

//	CGSizeGp	operator-(POINTGP gp) const;
//	CGPointGp	operator-(SIZEGP sizeGp) const;
//	CGRectGp	operator-(const RECTGP* lpRectGp) const;
//	CGPointGp	operator-() const;
	
};


/////////////////////////////////////////////////////////////////////////////
// CGRectGP

/**
* GP 좌표계 영역 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGRectGP : public CGRectGeo
{
public:
	CGRectGP();
	CGRectGP(double left, double top, double right, double bottom, double height = 0.0);
	CGRectGP(const CGRectGP& rectGP);
	CGRectGP(CGPointGP gp, CGSizeGP sizeGP);
	CGRectGP(CGPointGP topLeft, CGPointGP bottomRight);
	virtual ~CGRectGP();

// Attributes
public:
	RECTGP		m_R;

// Operations
public:
	virtual double	Width() const;
	virtual double	Height() const;
	virtual CGSizeGP Size() const;
	virtual CGPointGP TopLeft() const;
	virtual const CGPointGP TopLeft();
	virtual CGPointGP BottomRight() const;
	virtual const CGPointGP BottomRight();
	virtual CGPointGP CenterPoint() const;
	
	virtual BOOL IsRectEmpty() const;
	virtual BOOL IsRectNull() const;
	
	BOOL PtInRect(CGPointGP& gp) const;
	BOOL PtInRectWidth(CGPointGP& gp) const;
	BOOL PtInRectHeight(CGPointGP& gp) const;

	virtual void SetRect(CGPointGeo *pTopLeft, CGPointGeo *pBottomRight);
	void SetRect(double lon1, double lat1, double lon2, double lat2);
	void SetRect(CGPointGP& topLeft, CGPointGP& bottomRight);

	virtual void SetRectEmpty();
	
	void CopyRect(CGRectGP& rectGP);
	BOOL EqualRect(const CGRectGP& rectGP) const;

	void InflateRect(double lon, double lat);
	void InflateRect(CGSizeGP& sizeGP);
	void InflateRect(CGRectGP& rectGP);
	void InflateRect(double l, double t, double r, double b);

	void DeflateRect(double lon, double lat);
	void DeflateRect(CGSizeGP& sizeGP);
	void DeflateRect(CGRectGP& rectGP);
	void DeflateRect(double l, double t, double r, double b);

	void NormalizeRect();

	void OffsetRect(double lon, double lat);
	void OffsetRect(CGPointGP& gp);
	void OffsetRect(CGSizeGP& sizeGP);

	const CGRectGP& operator = (const CGRectGP &right);

	BOOL operator ==(const CGRectGP& rectGP) const;
	BOOL operator !=(const CGRectGP& rectGP) const;

	virtual void	SetRect(CGRectGeo *);
	virtual void	SetRect(RECTGP *);
	virtual void	Offset(CGSizeGeo *);
	virtual void	Offset(SIZEGP *);

};

typedef CArray<CGRectGP, CGRectGP> CGRectGPArray;

#undef	AFX_DATA
#define	AFX_DATA



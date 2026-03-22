// GRgn.h: interface for the CGRgn class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEODC_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define GRGN_FIRST				100
#define GRGN_NULL				100
#define GRGN_RECT				101
#define GRGN_RECT_INDIRECT		102
#define GRGN_ELLIPTIC			103
#define GRGN_ELLIPTIC_INDIRECT	104
#define GRGN_POLYGON			105
#define GRGN_POLYPOLYGON		106
#define GRGN_ROUND_RECT			107
#define GRGN_SET_RECT			108
#define GRGN_SET_RECT_RECT		109
#define GRGN_LAST				109


class CGDCGeo;
class CGRectGP;
class CGPointGP;

/**
* CGDCGeo 관리 class
* @author 유영호
* @version 1.0
* @since 2004.07.
**/
class AFX_DATA CGRgn
{

public:
//	static CRgn* PASCAL FromHandle(HRGN hRgn);
//	operator HRGN() const;

	// Constructors
	CGRgn(CGDCGeo *pDCGeo = NULL);
	virtual ~CGRgn();

	BOOL CreateRectRgn(double lon1, double lat1, double lon2, double lat2);
	BOOL CreateRectRgnIndirect(CGRectGP *pRectGP);
	BOOL CreateEllipticRgn(double lon1, double lat1, double lon2, double lat2);
	BOOL CreateEllipticRgnIndirect(CGRectGP *pRectGP);
	BOOL CreatePolygonRgn(CGPointGP *pGPs, int nCount, int nMode);

#ifndef _MAC
//	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode);
#endif
	 BOOL CreateRoundRectRgn(double lon1, double lat1, double lon2, double lat2, double clon, double clat);
//#ifndef _MAC
//	BOOL CreateFromPath(CDC* pDC);
//	BOOL CreateFromData(const XFORM* lpXForm, int nCount, const RGNDATA* pRgnData);
//#endif

	// Operations
	void SetRectRgn(double lon1, double lat1, double lon2, double lat2);
	void SetRectRgn(CGRectGP *pRectGP);
	int CombineRgn(CGRgn* pRgn1, CGRgn* pRgn2, int nCombineMode);
	int CopyRgn(CGRgn* pRgnSrc);
	BOOL EqualRgn(CGRgn* pRgn);
	int OffsetRgn(double lon, double lat);
	int OffsetRgn(CGPointGP gp);
	int GetRgnBox(CGRectGP *pRectGP);
	BOOL PtInRegion(double lon, double lat) const;
	BOOL PtInRegion(CGPointGP gp) const;
	BOOL RectInRegion(CGRectGP *pRectGP) const;
//#ifndef _MAC
//	int GetRegionData(LPRGNDATA lpRgnData, int nCount) const;
//#endif

	void SetDC(CGDCGeo *pDCGeo);

	void RestoreDC();
	void SetGRgnDC();

	BOOL Create();

public:
	CRgn	*m_pRgn;

protected:
	CGDCGeo		*m_pDCGeo;

	// CGRgn type
	UINT	m_nRgnType;

	// 좌표값을 유지
	CGPointGP	*m_pGPs;
	int			m_nCount;
	int			m_nMode;	// CreatePolygon 함수만 적용
	// Device Context
	int		m_MapMode;

	CPoint	m_WinOrg;
	CSize	m_WinExt;

	CPoint	m_ViewOrg;
	CSize	m_ViewExt;

	void SaveDC();
};

#undef	AFX_DATA
#define	AFX_DATA


//	static CRgn* PASCAL FromHandle(HRGN hRgn);
//	operator HRGN() const;
//	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode);
//	BOOL CreateFromPath(CDC* pDC);
//	BOOL CreateFromData(const XFORM* lpXForm, int nCount, const RGNDATA* pRgnData);
//	int GetRegionData(LPRGNDATA lpRgnData, int nCount) const;

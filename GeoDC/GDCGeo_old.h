#pragma once

#include "../../inc/includeGeoTrans.h"
#include "MapCore.h"

#undef AFX_DATA
#ifdef GEODC_IMPL
#define AFX_DATA __declspec(dllexport)
#else
#define AFX_DATA __declspec(dllimport)
#endif

class CGRgn;

/**
* CDC extension class (manages map coordinates, scales, ratios, etc.)
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGDCGeo : public CDC
{
public:
    CGDCGeo();
    CGDCGeo(HWND hWnd);
    CGDCGeo(CDC* pDC);
    DECLARE_DYNCREATE(CGDCGeo)

protected:
    CGGeoObject* m_pGeoObj;
    CGPointGeo* m_pWinOrg;      // window origin
    CGSizeGeo* m_pWinExt;      // window extent
    int* m_pMapScale;     // map scale
    double* m_pZoomRatio;    // zoom ratio

protected:
    HWND m_hWnd; // Member to store the HWND

public:
    CMapCore* m_pCoordMgr;     // Pointer to coordinate manager
    // Creating geometric objects
    CGPointGeo* CreatePoint(int nPoint = 1);
    CGSizeGeo* CreateSize();
    CGRectGeo* CreateRect();

    // Set coordinate manager instance
    void SetCoordMgr(CMapCore* pCoordMgr);

    // Initialization
    BOOL CreateCompatibleDC(CGDCGeo* pDCGeo);
    void MemReset();

    // Mapping Functions
    int GetMapMode() const;
    CGGeoObject* SetMapMode(CGGeoObject* pGeoObj);
    CGGeoObject* GetGeoObject() const;

    // Window Origin
    void GetWindowOrg(CGPointGeo* pRet);
    void SetWindowOrg(CGPointGeo* pNew, CGPointGeo* pRet = NULL);
    void OffsetWindowOrg(CGSizeGeo* pOffset, CGPointGeo* pRet = NULL);

    // Window extent
    void GetWindowExt(CGSizeGeo* pRet);
    void SetWindowExt(CGSizeGeo* pExt, CGSizeGeo* pRet = NULL);
    void ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom,
        CGSizeGeo* pRet = NULL);

    // Type-Safe Selection Helpers
    virtual CGdiObject* SelectObject(CGdiObject* pGdiObj);
    virtual CPen* SelectObject(CPen* pPen);
    virtual CBrush* SelectObject(CBrush* pBrush);
    virtual CFont* SelectObject(CFont* pFont);
    virtual CBitmap* SelectObject(CBitmap* pBitmap);
    virtual int SelectObject(CRgn* pRgn);
    int SelectObject(CGRgn* pRgn);

    // Region Functions
    BOOL FillRgn(CRgn* pRgn, CBrush* pBrush);
    BOOL FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight);
    BOOL InvertRgn(CRgn* pRgn);
    BOOL PaintRgn(CRgn* pRgn);

    BOOL FillRgn(CGRgn* pRgn, CBrush* pBrush);
    BOOL FrameRgn(CGRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight);
    BOOL InvertRgn(CGRgn* pRgn);
    BOOL PaintRgn(CGRgn* pRgn);

    // Coordinate Functions
    void DPtoGeoP(CGPointGeo* pPoints, LPPOINT lpPoints, int nCount = 1);
    void DPtoGeoP(CGSizeGeo* pSizeGeo, LPSIZE lpSize);
    void DPtoGeoP(CGRectGeo* pRectGeo, LPRECT lpRect);

    void LPtoGeoP(CGPointGeo* pPoints, LPPOINT lpPoints, int nCount = 1);
    void LPtoGeoP(CGSizeGeo* pSizeGeo, LPSIZE lpSize);
    void LPtoGeoP(CGRectGeo* pRectGeo, LPRECT lpRect);

    void GeoPtoDP(LPPOINT lpPoints, CGPointGeo* pPoints, int nCount = 1);
    void GeoPtoDP(LPSIZE lpSize, CGSizeGeo* pSizeGeo);
    void GeoPtoDP(LPRECT lpRect, CGRectGeo* pRectGeo);

    void GeoPtoLP(LPPOINT lpPoints, CGPointGeo* pPoints, int nCount = 1);
    void GeoPtoLP(LPSIZE lpSize, CGSizeGeo* pSizeGeo);
    void GeoPtoLP(LPRECT lpRect, CGRectGeo* pRectGeo);

    // Special Coordinate Functions (helpful for dealing with metafiles and OLE)
    void GeoPtoHIMETRIC(LPSIZE lpSize, CGSizeGeo* pSize);
    void HIMETRICtoGeoP(CGSizeGeo* pSize, LPSIZE lpSize);

    // Clipping Functions
    UINT SetBoundsRect(CGRectGeo* pRect, UINT flags);
    UINT GetBoundsRect(CGRectGeo* pRect, UINT flags);
    int GetClipBox(CGRectGeo* pRect);

    virtual int SelectClipRgn(CRgn* pRgn);
    int SelectClipRgn(CRgn* pRgn, int nMode);

    virtual int SelectClipRgn(CGRgn* pRgn);
    int SelectClipRgn(CGRgn* pRgn, int nMode);

    virtual int ExcludeClipRect(double lon1, double lat1, double lon2, double lat2);
    virtual int ExcludeClipRect(CGPointGeo* pPoint1, CGPointGeo* pPoint2);
    virtual int ExcludeClipRect(CGRectGeo* pRect);

    int ExcludeUpdateRgn(CWnd* pWnd);

    virtual int IntersectClipRect(double lon1, double lat1, double lon2, double lat2);
    virtual int IntersectClipRect(CGPointGeo* pPoint1, CGPointGeo* pPoint2);
    virtual int IntersectClipRect(CGRectGeo* pRect);

    virtual int OffsetClipRgn(double clon, double clat);
    virtual int OffsetClipRgn(CGSizeGP* pSizeGP);

    virtual BOOL PtVisible(double lon, double lat);
    virtual BOOL PtVisible(CGPointGeo* pPoint);
    virtual BOOL RectVisible(CGRectGeo* pRectGeo);

    // Line-Output Functions
    void GetCurrentPosition(CGPointGeo* pPoint);
    void MoveTo(CGPointGeo* pMove, CGPointGeo* pPrev = NULL);
    BOOL LineTo(CGPointGeo* pLine, BOOL bGrid = FALSE);
    BOOL Arc(CGPointGeo* pP1, CGPointGeo* pP2, CGPointGeo* pP3, CGPointGeo* pP4, BOOL bGrid = FALSE);
    BOOL Arc(CGRectGeo* pRect, CGPointGeo* pStart, CGPointGeo* pEnd, BOOL bGrid = FALSE);
    BOOL ArcTo(CGPointGeo* pP1, CGPointGeo* pP2, CGPointGeo* pP3, CGPointGeo* pP4, BOOL bGrid = FALSE);
    BOOL ArcTo(CGRectGeo* pRect, CGPointGeo* pStart, CGPointGeo* pEnd, BOOL bGrid = FALSE);
    BOOL AngleArc(CGPointGeo* pCenter, double dRadius, float fStartAngle, float fSweepAngle, BOOL bGrid = FALSE);
    BOOL AngleArc2(CGPointGeo* pCenter, CPoint ptRad, float fStartAngle, float fSweepAngle, BOOL bGrid = FALSE);
    BOOL PolyDraw(CGPointGeo* pPoints, const BYTE* bTypes, int nCount, BOOL bGrid = FALSE);
    BOOL Polyline(CGPointGeo* pPoints, int nCount, BOOL bGrid = FALSE);
    BOOL PolyPolyline(CGPointGeo* pPoints, const DWORD* lpPolyPoints, int nCount, BOOL bGrid = FALSE);
    BOOL PolylineTo(CGPointGeo* pPoints, int nCount, BOOL bGrid = FALSE);
    BOOL PolyBezier(CGPointGeo* pPoints, int nCount, BOOL bGrid = FALSE);
    BOOL PolyBezierTo(CGPointGeo* pPoints, int nCount, BOOL bGrid = FALSE);

    // Simple Drawing Functions
    void FillRect(CGRectGeo* pRect, CBrush* pBrush, BOOL bGrid = FALSE);
    void FrameRect(CGRectGeo* pRect, CBrush* pBrush, BOOL bGrid = FALSE);
    void InvertRect(CGRectGeo* pRect, BOOL bGrid = FALSE);
    BOOL DrawIcon(CGPointGeo* pPoint, HICON hIcon);
    BOOL DrawIcon(CPoint* pPoint, HICON hIcon);
    void DrawDragRect(CGRectGeo* pRect, CGSizeGeo* pSize, CGRectGeo* pRectLast, CGSizeGeo* pSizeLast, CBrush* pBrush = NULL, CBrush* pBrushLast = NULL, BOOL bGrid = FALSE);
    void FillSolidRect(CGRectGeo* pRect, COLORREF clr, BOOL bGrid = FALSE);
    void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr);
    //	void Draw3dRect(CGPointGeo *pOrg, CGSizeGeo *pSize, COLORREF clrTopLeft, COLORREF clrBottomRight, BOOL bGrid = FALSE);
    void Draw3dRect(CGRectGeo* pRect, COLORREF clrTopLeft, COLORREF clrBottomRight, BOOL bGrid = FALSE);
    BOOL DrawEdge(CGRectGeo* pRect, UINT nEdge, UINT nFlags, BOOL bGrid = FALSE);
    BOOL DrawFrameControl(CGRectGeo* pRect, UINT nType, UINT nState, BOOL bGrid = FALSE);

    // Ellipse and Polygon Functions
    BOOL Chord(CGRectGeo* pRect, CGPointGeo* pStart, CGPointGeo* pEnd, BOOL bGrid = FALSE);
    BOOL Chord(CGPointGeo* pP1, CGPointGeo* pP2, CGPointGeo* pP3, CGPointGeo* pP4, BOOL bGrid = FALSE);
    void DrawFocusRect(CGRectGeo* pRect);
    BOOL Ellipse(CGRectGeo* pRect, BOOL bGrid = FALSE);
    BOOL Pie(CGRectGeo* pRect, CGPointGeo* pStart, CGPointGeo* pEnd, BOOL bGrid = FALSE);
    BOOL Pie(CGPointGeo* pP1, CGPointGeo* pP2, CGPointGeo* pP3, CGPointGeo* pP4, BOOL bGrid = FALSE);
    BOOL Polygon(CGPointGeo* pPoints, int nCount, BOOL bGrid = FALSE);
    BOOL PolyPolygon(CGPointGeo* pPoints, LPINT lpPolyCounts, int nCount, BOOL bGrid = FALSE);
    BOOL Rectangle(CGRectGeo* pRect, BOOL bGrid = FALSE);
    BOOL Rectangle(CGPointGeo* pP1, CGPointGeo* pP2, BOOL bGrid = FALSE);
    BOOL Rectangle(double lon1, double lat1, double lon2, double lat2, BOOL bGrid = FALSE);
    BOOL RoundRect(CGRectGeo* pRect, CGPointGeo* pPoint, BOOL bGrid = FALSE);
    //	BOOL RoundRect(CGPointGeo *pP1, CGPointGeo *pP2, CGPointGeo *pP3, BOOL bGrid = FALSE);

// Bitmap Functions
    BOOL PatBlt(double lon, double lat,
        double clon, double clat, DWORD dwRop);
    BOOL PatBlt(CGPointGeo* pLeftTop, CGSizeGeo* pSizeGeo,
        DWORD dwRop);

    BOOL BitBlt(double lon, double lat, double clon, double clat,
        CGDCGeo* pSrcDCGeo,
        double lonSrc, double latSrc, DWORD dwRop);
    BOOL BitBlt(CGPointGeo* pLeftTop, CGSizeGeo* pSizeGeo,
        CGDCGeo* pSrcDCGeo,
        CGPointGeo* pSrcLeftTop, DWORD dwRop);

    BOOL StretchBlt(double lon, double lat, double clon, double clat,
        CGDCGeo* pSrcDCGeo,
        double lonSrc, double latSrc,
        double clonSrc, double clatSrc, DWORD dwRop);
    BOOL StretchBlt(CGPointGeo* pLeftTop, CGSizeGeo* pSizeGeo,
        CGDCGeo* pSrcDCGeo,
        CGPointGeo* pSrcLeftTop, CGSizeGeo* pSrcSizeGeo,
        DWORD dwRop);

    COLORREF GetPixel(CGPointGeo* pPoint);
    COLORREF SetPixel(CGPointGeo* pPoint, COLORREF crColor);
    BOOL SetPixelV(CGPointGeo* pPoint, COLORREF crColor);
    BOOL FloodFill(CGPointGeo* pPoint, COLORREF crColor);
    BOOL ExtFloodFill(CGPointGeo* pPoint, COLORREF crColor, UINT nFillType);

    BOOL MaskBlt(double lon, double lat, double clon, double clat,
        CGDCGeo* pStrDCGeo, double lonSrc, double latSrc,
        CBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop);
    BOOL MaskBlt(CGPointGeo* pLeftTop, CGSizeGeo* pSizeGeo,
        CGDCGeo* pStrDCGeo, CGPointGeo* pSrcLeftTop,
        CBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop);

    BOOL PlgBlt(CGPointGeo* pPoints,
        CGDCGeo* pSrcDCGeo,
        double lonSrc, double latSrc, double clonSrc, double clatSrc,
        CBitmap& maskBitmap, int xMask, int yMask);
    BOOL PlgBlt(CGPointGeo* pPoints, CGDCGeo* pSrcDCGeo,
        CGPointGeo* pSrcLeftTop, CGSizeGeo* pSrcSizeGeo,
        CBitmap& maskBitmap, int xMask, int yMask);

    int StretchDIBits(double lonDest, double latDest, double clonDest, double clatDest,
        double lonSrc, double latSrc, double clonSrc, double clatSrc,
        CONST VOID* lpBits,		// address of bitmap bits 
        CONST BITMAPINFO* lpBitsInfo,// address of bitmap data 
        UINT iUsage,			// usage 
        DWORD dwRop);		// raster operation code
    int StretchDIBits(CGPointGeo* pDestPointGeo, CGSizeGeo* pDestSizeGeo,
        CGPointGeo* pSrcPointGeo, CGSizeGeo* pSrcSizeGeo,
        CONST VOID* lpBits,		// address of bitmap bits 
        CONST BITMAPINFO* lpBitsInfo,// address of bitmap data 
        UINT iUsage,			// usage 
        DWORD dwRop);		// raster operation code
    int StretchDIBits(CGPointGeo* pDestPointGeo, CGSizeGeo* pDestSizeGeo,
        int xSrc, int ySrc, int cxSrc, int cySrc,
        CONST VOID* lpBits,				// address of bitmap bits 
        CONST BITMAPINFO* lpBitsInfo,	// address of bitmap data 
        UINT iUsage,						// usage 
        DWORD dwRop);					// raster operation code


// Text Functions
    BOOL TextOut(CGPointGeo* pPoint, const CString& str);
    BOOL TextOut(CPoint* pPoint, const CString& str);
    BOOL ExtTextOut(CGPointGeo* pPoint, UINT nOptions, CGRectGeo* pRect, const CString& str, LPINT lpDxWidths);
    BOOL ExtTextOut(CGPointGeo* pPoint, UINT nOptions, CGRectGeo* pRect, LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths);
    void TabbedTextOut(CGPointGeo* pPoint, LPCTSTR lpszString, int nCount,
        int nTabPositions, LPINT lpnTabStopPosition, int nTabOrigin, CGSizeGeo* pSize);
    int DrawText(const CString& str, CGRectGeo* pRect, UINT nFormat);
    int DrawText(LPCTSTR lpszString, int nCount, CGRectGeo* pRect, UINT nFormat);
    void GetTextExtent(LPCTSTR lpszString, int nCount, CGSizeGeo* pSize);
    void GetTextExtent(const CString& str, CGSizeGeo* pSize);
    void GetOutputTextExtent(LPCTSTR lpszString, int nCount, CGSizeGeo* pSize);
    void GetOutputTextExtent(const CString& str, CGSizeGeo* pSize);
    void GetOutputTabbedTextExtent(const CString& str, int nCount, int nTabPositions, LPINT lpnTabStopPositions, CGSizeGeo* pSize);
    void GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, CGSizeGeo* pSize);
    void GetTabbedTextExtent(const CString& str, int nTabPositions, LPINT lpnTabStopPositions, CGSizeGeo* pSize);
    void GetTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, CGSizeGeo* pSize);
    void GetOutputTabbedTextExtent(const CString& str, int nTabPositions, LPINT lpnTabStopPositions, CGSizeGeo* pSize);
    //	virtual BOOL GrayString(CBrush* pBrush, BOOL ( CALLBACK EXPORT* lpfnOutput )( HDC, LPARAM, int ), LPARAM lpData, int nCount, CGPointGeo *pPoint, CGSizeGeo *pSize);

    CGSizeGP LPSZtoGeoPSZ(CPoint* pLPOrg, CSize* pLPSize);
    CSize    GeoPSZtoLPSZ(CGPointGeo* pPointGeoOrg, CGSizeGeo* pSizeGeo);

    CGSizeGP GetPixelSizeGP();

    // Path Function
    int GetPath(CGPointGeo* pGeoPoints, LPBYTE lpTypes, int nCount);

private:
    CSize	 GPSZtoLPSZ(CGPointGP gpOrg, CGSizeGP gpSize);
    CGSizeGP LPSZtoGPSZ(CPoint lpOrg, CSize lpSize);

    // Implementation
public:
    //AFX_DATA BOOL DrawImageList(CImageList& ImageList);
    void SetRealScaleGeoDC();
    int SetMapScale(int nMapScale);
    int GetMapScale() const;
    double SetZoomRatio(double dZoom = 1.0);
    double GetZoomRatio() const;
    virtual ~CGDCGeo();

    int GetEllipsoid() const;

#ifdef _DEBUG
    AFX_DATA virtual void AssertValid() const;
    AFX_DATA virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CGDCGeo Paint Management Class

class AFX_DATA CGPaintDCGeo : public CGDCGeo
{
    DECLARE_DYNAMIC(CGPaintDCGeo)

    // Constructors
public:
    CGPaintDCGeo(CWnd* pWnd);   // BeginPaint
    CGPaintDCGeo(HWND hWnd, HDC hDC = NULL);

    // Attributes
protected:
    HWND m_hWnd;  // Store window handle for the painting operation

public:
    PAINTSTRUCT m_ps;       // Actual paint struct

// Implementation
public:
    virtual ~CGPaintDCGeo();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};

/**
* Class for Client DC Management
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGClientDCGeo : public CGDCGeo
{
    DECLARE_DYNAMIC(CGClientDCGeo)

    // Constructors
public:
    CGClientDCGeo(CWnd* pWnd);

    // Attributes
protected:
    HWND m_hWnd; // Store window handle for the client DC

// Implementation
public:
    virtual ~CGClientDCGeo();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};

/**
* Class for Window DC Management
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGWindowDCGeo : public CGDCGeo
{
    DECLARE_DYNAMIC(CGWindowDCGeo)

    // Constructors
public:
    CGWindowDCGeo(CWnd* pWnd);

    // Attributes
protected:
    HWND m_hWnd; // Store window handle for window DC

// Implementation
public:
    virtual ~CGWindowDCGeo();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};

#undef AFX_DATA
#define AFX_DATA
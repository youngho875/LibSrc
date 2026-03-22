// GRgn.h: interface for the CGRgn class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#undef AFX_DATA
#ifdef GEODC_IMPL
#define AFX_DATA __declspec(dllexport)
#else
#define AFX_DATA __declspec(dllimport)
#endif

// Region type definitions
#define GRGN_FIRST                   100
#define GRGN_NULL                    100
#define GRGN_RECT                    101
#define GRGN_RECT_INDIRECT           102
#define GRGN_ELLIPTIC                103
#define GRGN_ELLIPTIC_INDIRECT       104
#define GRGN_POLYGON                 105
#define GRGN_POLYPOLYGON             106
#define GRGN_ROUND_RECT              107
#define GRGN_SET_RECT                108
#define GRGN_SET_RECT_RECT           109
#define GRGN_LAST                    109

class CGDCGeo;    // Forward declaration
class CGRectGP;   // Forward declaration
class CGPointGP;  // Forward declaration

/**
* CGDCGeo management class
* @author À¯¿µÈ£
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGRgn
{
public:
    // Constructors
    CGRgn(CGDCGeo* pDCGeo = nullptr);
    virtual ~CGRgn();

    // Region creation functions
    BOOL CreateRectRgn(double lon1, double lat1, double lon2, double lat2);
    BOOL CreateRectRgnIndirect(CGRectGP* pRectGP);
    BOOL CreateEllipticRgn(double lon1, double lat1, double lon2, double lat2);
    BOOL CreateEllipticRgnIndirect(CGRectGP* pRectGP);
    BOOL CreatePolygonRgn(CGPointGP* pGPs, int nCount, int nMode);
    BOOL CreateRoundRectRgn(double lon1, double lat1, double lon2, double lat2, double clon, double clat);

    // Operations
    void SetRectRgn(double lon1, double lat1, double lon2, double lat2);
    void SetRectRgn(CGRectGP* pRectGP);
    int CombineRgn(CGRgn* pRgn1, CGRgn* pRgn2, int nCombineMode);
    int CopyRgn(CGRgn* pRgn);
    BOOL EqualRgn(CGRgn* pRgn);
    int OffsetRgn(double lon, double lat);
    int OffsetRgn(CGPointGP gp);
    int GetRgnBox(CGRectGP* pRectGP);
    BOOL PtInRegion(double lon, double lat) const;
    BOOL PtInRegion(CGPointGP gp) const;
    BOOL RectInRegion(CGRectGP* pRectGP) const;

    // Device Context Management
    void SetDC(CGDCGeo* pDCGeo);
    void RestoreDC();
    void SetGRgnDC();

    BOOL Create(); // Create the region

public:
    CRgn* m_pRgn;              // Pointer to the region

protected:
    CGDCGeo* m_pDCGeo;        // Pointer to the associated device context

    // Region type
    UINT m_nRgnType;          // Type of the region

    // Points and counts for polygons
    CGPointGP* m_pGPs;        // Array of points for the region
    int m_nCount;             // Number of points
    int m_nMode;              // Mode for polygon creation

    // Device Context Details
    int m_MapMode;            // Mapping mode in use
    CPoint m_WinOrg;          // Window origin
    CSize m_WinExt;           // Window extent
    CPoint m_ViewOrg;         // View origin
    CSize m_ViewExt;          // View extent

    void SaveDC();            // Save the device context
};

#undef AFX_DATA
#define AFX_DATA

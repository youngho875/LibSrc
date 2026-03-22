// GRgn.cpp: implementation of the CGRgn class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GDCGeo.h"
#include "GRgn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGRgn::CGRgn(CGDCGeo* pDCGeo)
    :   m_pDCGeo(pDCGeo),
        m_nRgnType(GRGN_NULL), // Initialize region type
        m_pRgn(nullptr),       // Initialize region pointer to nullptr
        m_pGPs(nullptr),       // Initialize points to nullptr
        m_nCount(0),          // Initialize point count to 0
        m_nMode(0)            // Initialize mode
{
    if (pDCGeo) 
    {
        SaveDC(); // Save current DC settings
    }
}

CGRgn::~CGRgn()
{
    if (m_pGPs) delete[] m_pGPs; // Clean up point array
    if (m_pRgn) 
    {
        m_pRgn->DeleteObject(); // Delete the region object if not null
        delete m_pRgn;          // Clean up region pointer
    }
}

void CGRgn::SetDC(CGDCGeo* pDCGeo)
{
    m_pDCGeo = pDCGeo;

    m_nRgnType = GRGN_NULL;    // Reset region type
    m_pRgn = nullptr;          // Reset region pointer
    m_pGPs = nullptr;          // Reset points
    m_nCount = 0;              // Reset point count
    m_nMode = 0;               // Reset mode

    if (pDCGeo) SaveDC();      // Save DC if available
}

void CGRgn::SaveDC()
{
    m_MapMode = m_pDCGeo->CDC::GetMapMode(); // Save the current map mode
    m_WinOrg = m_pDCGeo->CDC::GetWindowOrg(); // Save window origin
    m_ViewOrg = m_pDCGeo->CDC::GetViewportOrg(); // Save view origin
    m_WinExt = m_pDCGeo->CDC::GetWindowExt(); // Save window extent
    m_ViewExt = m_pDCGeo->CDC::GetViewportExt(); // Save view extent
}

void CGRgn::RestoreDC()
{
    m_pDCGeo->CDC::SetMapMode(m_MapMode); // Restore map mode
    m_pDCGeo->CDC::SetWindowOrg(m_WinOrg); // Restore window origin
    m_pDCGeo->CDC::SetViewportOrg(m_ViewOrg); // Restore view origin
    m_pDCGeo->CDC::SetWindowExt(m_WinExt); // Restore window extent
    m_pDCGeo->CDC::SetViewportExt(m_ViewExt); // Restore view extent
}

void CGRgn::SetGRgnDC()
{
    CPoint dpWinOrg = m_WinOrg; // Initialize device context window origin
    CDC* pDC = (CDC*)m_pDCGeo;

    pDC->LPtoDP(&dpWinOrg); // Convert logical to device units

    pDC->SetMapMode(MM_TEXT); // Set mapping mode to text
    pDC->SetWindowOrg(dpWinOrg); // Set window origin
    pDC->SetViewportOrg(m_ViewOrg); // Set viewport origin
}

BOOL CGRgn::CreateRectRgn(double lon1, double lat1, double lon2, double lat2)
{
    m_pGPs = new CGPointGP[2]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check if memory allocation was successful

    m_pGPs[0].SetPoint(lon1, lat1); // Set points for the rectangle
    m_pGPs[1].SetPoint(lon2, lat2);

    m_nRgnType = GRGN_RECT; // Set region type to rectangle
    m_nCount = 2; // Set number of points

    return Create(); // Create the region
}

BOOL CGRgn::CreateRectRgnIndirect(CGRectGP* pRectGP)
{
    m_pGPs = new CGPointGP[2]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check memory allocation

    m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top); // Set points from rect
    m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.bottom);

    m_nRgnType = GRGN_RECT_INDIRECT; // Set region type to indirect rectangle
    m_nCount = 2; // Set point count

    return Create(); // Create the region
}

BOOL CGRgn::CreateEllipticRgn(double lon1, double lat1, double lon2, double lat2)
{
    m_pGPs = new CGPointGP[2]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check memory allocation

    m_pGPs[0].SetPoint(lon1, lat1); // Set points for the ellipse
    m_pGPs[1].SetPoint(lon2, lat2);

    m_nRgnType = GRGN_ELLIPTIC; // Set region type to elliptic
    m_nCount = 2; // Set point count

    return Create(); // Create the region
}

BOOL CGRgn::CreateEllipticRgnIndirect(CGRectGP* pRectGP)
{
    m_pGPs = new CGPointGP[2]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check memory allocation

    m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top); // Set points from rect
    m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.bottom);

    m_nRgnType = GRGN_ELLIPTIC_INDIRECT; // Set region type to indirect elliptic
    m_nCount = 2; // Set point count

    return Create(); // Create the region
}

BOOL CGRgn::CreatePolygonRgn(CGPointGP* pGPs, int nCount, int nMode)
{
    m_pGPs = new CGPointGP[nCount]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check memory allocation

    for (int i = 0; i < nCount; i++) // Set points from passed array
        m_pGPs[i].SetPoint(&pGPs[i]);

    m_nRgnType = GRGN_POLYGON; // Set region type to polygon
    m_nCount = nCount; // Set point count
    m_nMode = nMode; // Set mode

    return Create(); // Create the region
}

BOOL CGRgn::CreateRoundRectRgn(double lon1, double lat1, double lon2, double lat2, double clon, double clat)
{
    m_pGPs = new CGPointGP[3]; // Allocate memory for points
    if (!m_pGPs) return FALSE; // Check memory allocation

    m_pGPs[0].SetPoint(lon1, lat1); // Set points for the round rectangle
    m_pGPs[1].SetPoint(lon2, lat2);
    m_pGPs[2].SetPoint(clon, clat);

    m_nRgnType = GRGN_ROUND_RECT; // Set region type to round rectangle
    m_nCount = 3; // Set point count

    return Create(); // Create the region
}

BOOL CGRgn::Create()
{
    if (m_pGPs == nullptr || m_nCount == 0 || m_nRgnType == GRGN_NULL || m_pDCGeo == nullptr)
        return FALSE; // Check necessary conditions

    SaveDC(); // Save the current DC settings

    CPoint* pDPs = new CPoint[m_nCount]; // Allocate points in device coordinates
    if (!pDPs) return FALSE; // Check memory allocation

    m_pDCGeo->GeoPtoDP(pDPs, m_pGPs, m_nCount); // Convert geographic points to device points

    // Handle different region types
    if (m_nRgnType == GRGN_ROUND_RECT) {
        CGSizeGP sizeGP(m_pGPs[2].m_P.lon, m_pGPs[2].m_P.lat); // Get round rect size
        CSize sizeDP;
        m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP); // Convert to device size
        pDPs[2] = sizeDP; // Set radius for round rectangle
    }

    SetGRgnDC(); // Set the device context for the region

    if (m_pRgn) {
        m_pRgn->DeleteObject(); // Delete previous region if exists
        delete m_pRgn;
    }

    m_pRgn = new CRgn; // Allocate a new region
    if (!m_pRgn) return FALSE; // Check memory allocation

    // Create the region based on its type
    switch (m_nRgnType) {
    case GRGN_RECT:
        return m_pRgn->CreateRectRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
    case GRGN_RECT_INDIRECT:
        return m_pRgn->CreateRectRgnIndirect(&CRect(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y));
    case GRGN_ELLIPTIC:
        return m_pRgn->CreateEllipticRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
    case GRGN_ELLIPTIC_INDIRECT:
        return m_pRgn->CreateEllipticRgnIndirect(&CRect(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y));
    case GRGN_POLYGON:
        return m_pRgn->CreatePolygonRgn(pDPs, m_nCount, m_nMode);
    case GRGN_ROUND_RECT:
        return m_pRgn->CreateRoundRectRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y, pDPs[2].x, pDPs[2].y);
    default:
        return FALSE; // Handle any unrecognized region type
    }

    if (pDPs) delete[] pDPs; // Clean up dynamically allocated memory
    RestoreDC(); // Restore previous DC settings

    return TRUE; // Indicate success if reached here
}

void CGRgn::SetRectRgn(double lon1, double lat1, double lon2, double lat2)
{
    m_pGPs = new CGPointGP[2]; // Allocate for two points
    if (!m_pGPs) return; // Check for allocation success

    m_pGPs[0].SetPoint(lon1, lat1); // Set rectangle points
    m_pGPs[1].SetPoint(lon2, lat2);

    m_nRgnType = GRGN_SET_RECT; // Set region type
    m_nCount = 2;               // Point count is 2

    Create(); // Create the region
}

void CGRgn::SetRectRgn(CGRectGP* pRectGP)
{
    m_pGPs = new CGPointGP[2]; // Allocate memory for two points
    if (!m_pGPs) return; // Check memory allocation

    m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top); // Set rectangle points
    m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.bottom);

    m_nRgnType = GRGN_SET_RECT_RECT; // Set region type
    m_nCount = 2;                     // Point count is 2

    Create(); // Create the region
}

int CGRgn::CombineRgn(CGRgn* pRgn1, CGRgn* pRgn2, int nCombineMode)
{
    int nRetVal = ERROR;

    if (pRgn1->m_pDCGeo == pRgn2->m_pDCGeo) // Ensure both regions belong to the same DC
    {
        this->m_pDCGeo = pRgn1->m_pDCGeo; // Set the DC to one of the regions

        // Set up region properties
        this->m_MapMode = pRgn1->m_MapMode;
        this->m_WinOrg = pRgn1->m_WinOrg;
        this->m_WinExt = pRgn1->m_WinExt;
        this->m_ViewOrg = pRgn1->m_ViewOrg;
        this->m_ViewExt = pRgn1->m_ViewExt;

        this->m_pRgn = new CRgn; // Create a new region
        if (!this->m_pRgn) // Check allocation
            return -1;

        nRetVal = this->m_pRgn->CombineRgn(pRgn1->m_pRgn, pRgn2->m_pRgn, nCombineMode); // Combine regions
    }
    return nRetVal; // Return result
}

int CGRgn::CopyRgn(CGRgn* pRgnSrc)
{
    this->m_pDCGeo = pRgnSrc->m_pDCGeo; // Set the DC to the source region

    // Device Context settings
    this->m_MapMode = pRgnSrc->m_MapMode;
    this->m_WinOrg = pRgnSrc->m_WinOrg;
    this->m_WinExt = pRgnSrc->m_WinExt;
    this->m_ViewOrg = pRgnSrc->m_ViewOrg;
    this->m_ViewExt = pRgnSrc->m_ViewExt;

    // Copy region properties
    this->m_nRgnType = pRgnSrc->m_nRgnType; // Set region type
    this->m_nCount = pRgnSrc->m_nCount;     // Set point count
    this->m_nMode = pRgnSrc->m_nMode;       // Set mode

    this->m_pGPs = new CGPointGP[this->m_nCount]; // Allocate memory for points
    if (!this->m_pGPs) // Check allocation success
        return FALSE;

    for (int i = 0; i < this->m_nCount; i++)
        this->m_pGPs[i].SetPoint(&pRgnSrc->m_pGPs[i]); // Copy points

    Create(); // Create the region

    return this->m_pRgn->CopyRgn(pRgnSrc->m_pRgn); // Copy the source region
}

BOOL CGRgn::EqualRgn(CGRgn* pRgn)
{
    BOOL bRetVal = FALSE;

    if (this->m_pRgn != NULL) // Check if this region is initialized
        if (this->m_pDCGeo == pRgn->m_pDCGeo) // Compare with the source region's DC
            bRetVal = this->m_pRgn->EqualRgn(pRgn->m_pRgn); // Check if regions are equal

    return bRetVal; // Return result
}

int CGRgn::OffsetRgn(double lon, double lat)
{
    int nRetVal = ERROR;

    if (this->m_pDCGeo && this->m_pGPs) // Ensure DC and points are valid
    {
        for (int i = 0; i < this->m_nCount; i++) {
            this->m_pGPs[i].Offset(lon, lat); // Offset points
        }

        CGSizeGP sizeGP(lon, lat);
        CSize sizeDP;
        this->m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP); // Convert size to device points

        nRetVal = this->m_pRgn->OffsetRgn(sizeDP.cx, sizeDP.cy); // Offset region
        Create(); // Create region with new offsets
    }
    return nRetVal; // Return result
}

int CGRgn::OffsetRgn(CGPointGP gp)
{
    int nRetVal = ERROR;

    if (this->m_pDCGeo && this->m_pGPs) // Ensure DC and points are valid
    {
        for (int i = 0; i < this->m_nCount; i++) {
            this->m_pGPs[i].Offset(gp.m_P.lon, gp.m_P.lat); // Offset points
        }

        CGSizeGP sizeGP(gp.m_P.lon, gp.m_P.lat);
        CSize sizeDP;
        this->m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP); // Convert size to device points

        nRetVal = this->m_pRgn->OffsetRgn(sizeDP.cx, sizeDP.cy); // Offset region
        Create(); // Create region with new offsets
    }
    return nRetVal; // Return result
}

int CGRgn::GetRgnBox(CGRectGP* pRectGP)
{
    int nRetVal = ERROR;

    if (this->m_pRgn != NULL) // Ensure region is initialized
    {
        CRect rect;
        nRetVal = this->m_pRgn->GetRgnBox(&rect); // Get region bounding box
        this->m_pDCGeo->DPtoGeoP(pRectGP, rect); // Convert bounding box to geo coordinates
    }
    return nRetVal; // Return result
}

BOOL CGRgn::PtInRegion(double lon, double lat) const
{
    BOOL bRetVal = FALSE;

    if (this->m_pDCGeo && this->m_pRgn) // Ensure DC and region are valid
    {
        CGPointGP GP(lon, lat); // Create point in geographic coordinates
        CPoint DP;
        this->m_pDCGeo->GeoPtoDP(&DP, &GP, 1); // Convert to device points
        bRetVal = this->m_pRgn->PtInRegion(DP.x, DP.y); // Check
        bRetVal = this->m_pRgn->PtInRegion(DP.x, DP.y); // Check if the point is in the region
    }
    return bRetVal; // Return the result
}

BOOL CGRgn::PtInRegion(CGPointGP gp) const
{
    BOOL bRetVal = FALSE;

    if (this->m_pDCGeo && this->m_pRgn) // Ensure DC and region are valid
    {
        CPoint DP;
        this->m_pDCGeo->GeoPtoDP(&DP, &gp, 1); // Convert point from geo to device coordinates
        bRetVal = this->m_pRgn->PtInRegion(DP); // Check if the point is in the region
    }
    return bRetVal; // Return the result
}

BOOL CGRgn::RectInRegion(CGRectGP* pRectGP) const
{
    BOOL bRetVal = FALSE;

    if (this->m_pDCGeo && this->m_pRgn) // Ensure DC and region are valid
    {
        CRect rectDP;
        this->m_pDCGeo->GeoPtoDP(&rectDP, pRectGP); // Convert geographic rectangle to device coordinates
        bRetVal = this->m_pRgn->RectInRegion(rectDP); // Check if the rectangle is in the region
    }
    return bRetVal; // Return the result
}

// Additional methods for further functionality can go here
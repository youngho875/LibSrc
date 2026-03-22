// GDCGeo.cpp : implementation file
//

#include "pch.h"
#include "GRgn.h"
#include "GDCGeo.h"
#include "math.h"
#include "Coord.h"
//#include "CoordMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGDCGeo, CDC)
IMPLEMENT_DYNAMIC(CGClientDCGeo, CGDCGeo)
IMPLEMENT_DYNAMIC(CGWindowDCGeo, CGDCGeo)
IMPLEMENT_DYNAMIC(CGPaintDCGeo, CGDCGeo)

/////////////////////////////////////////////////////////////////////////////
// CGDCGeo


CGDCGeo::CGDCGeo()
    : m_hWnd(NULL), m_pCoordMgr(nullptr) // Initialize member variables
{
    // Additional initialization
}

CGDCGeo::CGDCGeo(HWND hWnd)
    : m_hWnd(hWnd)
{
    m_pGeoObj = NULL;
    m_pWinOrg = NULL;
    m_pWinExt = NULL;

    m_pMapScale = NULL;
    m_pZoomRatio = NULL;
    m_pCoordMgr = new CMapCore(hWnd); // Creating a new instance of CMapCore
}

CGDCGeo::CGDCGeo(CDC* pDC)
    : m_hWnd(NULL) // Initialize m_hWnd from CDC
{
    if (pDC)
    {
        CDC::Attach(pDC->m_hDC);
    }

    m_pGeoObj = NULL;
    m_pWinOrg = NULL;
    m_pWinExt = NULL;

    m_pMapScale = NULL;
    m_pZoomRatio = NULL;
    m_pCoordMgr = new CMapCore(m_hWnd); // Creating a new instance of CMapCore
}

CGDCGeo::~CGDCGeo()
{
    // m_pGeoObj는 다른 곳에서 생성하는 것이므로 delete 하지 않는다.
    if (m_pGeoObj != NULL) { m_pGeoObj = NULL; }
    if (m_pWinOrg != NULL) { delete m_pWinOrg; m_pWinOrg = NULL; }
    if (m_pWinExt != NULL) { delete m_pWinExt; m_pWinExt = NULL; }
    if (m_pMapScale != NULL) { delete m_pMapScale; m_pMapScale = NULL; }
    if (m_pZoomRatio != NULL) { delete m_pZoomRatio; m_pZoomRatio = NULL; }
    delete m_pCoordMgr; // Release the CMapCore instance
}

/////////////////////////////////////////////////////////////////////////////
// CGDCGeo diagnostics

#ifdef _DEBUG
void CGDCGeo::AssertValid() const
{
	CDC::AssertValid();
}

void CGDCGeo::Dump(CDumpContext& dc) const
{
	CDC::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGDCGeo attributes

/////////////////////////////////////////////////////////////////////////////
// CGDCGeo operations

// Set the coordinate manager instance
void CGDCGeo::SetCoordMgr(CMapCore* pCoordMgr)
{
    // Set the instance in the coordinate manager
	// If you're replacing an existing instance, clean it up
	if (m_pCoordMgr) {
		delete m_pCoordMgr;
	}

	m_pCoordMgr = pCoordMgr; // Set the new instance
}

// Creating geometric object
CGPointGeo* CGDCGeo::CreatePoint(int nPoint)
{
    ASSERT(m_pGeoObj); // Ensure mapping mode is set
    if (!m_pGeoObj) return NULL;

    return (CGPointGeo*)m_pGeoObj->Point(nPoint);
}

CGSizeGeo* CGDCGeo::CreateSize()
{
    ASSERT(m_pGeoObj); // Ensure mapping mode is set
    if (!m_pGeoObj) return NULL;

    return (CGSizeGeo*)m_pGeoObj->Size();
}

CGRectGeo* CGDCGeo::CreateRect()
{
    ASSERT(m_pGeoObj); // Ensure mapping mode is set
    if (!m_pGeoObj) return NULL;

    return (CGRectGeo*)m_pGeoObj->Rect();
}

/////////////////////////////////////////////////////////////////////////////
// Initializations

BOOL CGDCGeo::CreateCompatibleDC(CGDCGeo* pDCGeo)
{
/*	ASSERT(pDCGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDCGeo)) return FALSE;

	ASSERT(pDCGeo->m_pGeoObj);
	if (!(pDCGeo->m_pGeoObj)) return FALSE;

	this->m_pGeoObj = pDCGeo->m_pGeoObj;

	if (pDCGeo->m_pWinOrg) {
		if (!this->m_pWinOrg) {
			this->m_pWinOrg = this->CreatePoint();
		}
		CGPointGP *pPointGP = (CGPointGP*) this->m_pWinOrg;
		pPointGP->SetPoint(pDCGeo->m_pWinOrg);	// window origin
	}
	
	if (pDCGeo->m_pWinExt) {
		if (!this->m_pWinExt) {
			this->m_pWinExt = this->CreateSize();
		}
		CGSizeGP *pSizeGP = (CGSizeGP*) this->m_pWinExt;
		pSizeGP->SetSize(pDCGeo->m_pWinExt);	// window extent
	}

	if (pDCGeo->m_pMapScale) {
		if (!this->m_pMapScale)
		{
			this->m_pMapScale = new (int);
		}
		*this->m_pMapScale = *(pDCGeo->m_pMapScale);	// 지도축척
	}

	if (pDCGeo->m_pZoomRatio) {
		if (!this->m_pZoomRatio)
		{
			this->m_pZoomRatio = new (double);
		}
		*this->m_pZoomRatio = *(pDCGeo->m_pZoomRatio);	// 확대/축소 비율
	}
*/
	return CDC::CreateCompatibleDC((CDC*)pDCGeo);
}

/////////////////////////////////////////////////////////////////////////////
// Mapping Functions
int CGDCGeo::GetMapMode() const
{
	int curMapMode;
	if (this->m_pGeoObj != NULL) // 지도를 위한 mapping mode 가 설정되어 있는 경우
		curMapMode = m_pGeoObj->GetMapMode();
	// MFC에서 기본적으로 사용하는 mapping mode 를 사용한 경우
	else curMapMode = CDC::GetMapMode();

	return curMapMode;		
}

CGGeoObject *CGDCGeo::SetMapMode(CGGeoObject *pGeoObj)
{
	ASSERT(pGeoObj);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGeoObj)) return FALSE;

	CGGeoObject	*pPreObj = this->m_pGeoObj;

	// CGDCGeo 에 매핑에 관련된 값이 이미 설정되어 있는 경우
	int	nMapMode = pGeoObj->GetMapMode();

	// MFC mapping mode
	if(MM_TEXT <= nMapMode && nMapMode <= MM_ANISOTROPIC) {
		CDC::SetMapMode(nMapMode);
		m_pGeoObj = pGeoObj;
		return pPreObj;
	}

	// GMM_GP, GMM_UTM
	CDC::SetMapMode(MM_ISOTROPIC);
	m_pGeoObj = pGeoObj;

	return pPreObj;	// 이전에 설정되어 있던 객체를 돌려준다.
}

CGGeoObject* CGDCGeo::GetGeoObject() const
{
	return this->m_pGeoObj;
}
/////////////////////////////////////////////////////////////////////////////
// Window Origin
void CGDCGeo::GetWindowOrg(CGPointGeo *pRet)
{
	ASSERT(pRet);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRet)) return;

	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사.
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	ASSERT(pRet->m_nPointType == this->m_pGeoObj->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRet->m_nPointType == this->m_pGeoObj->m_nPointType)) return;

	pRet->SetPoint(m_pWinOrg);
}

void CGDCGeo::SetWindowOrg(CGPointGeo *pNew, CGPointGeo *pRet)
{
	ASSERT(pNew);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pNew)) return;
	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사.
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	if (pRet != NULL)
	{
		ASSERT(pRet->m_nPointType==m_pGeoObj->m_nPointType);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(pRet->m_nPointType==m_pGeoObj->m_nPointType)) return;
		pRet->SetPoint(m_pWinOrg);
	}

	if (this->m_pWinOrg == NULL) this->m_pWinOrg = this->m_pGeoObj->Point();
	this->m_pWinOrg->SetPoint(pNew);

	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, this->m_pWinOrg);
	CDC::SetWindowOrg(point);

}

void CGDCGeo::OffsetWindowOrg(CGSizeGeo *pOffset, CGPointGeo *pRet)
{
	ASSERT(pOffset);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset)) return;
	ASSERT(m_pGeoObj);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;
	ASSERT(pOffset->m_nPointType == m_pGeoObj->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pOffset->m_nPointType == m_pGeoObj->m_nPointType)) return;

	if(pRet!=NULL) {
		ASSERT(pRet->m_nPointType == this->m_pGeoObj->m_nPointType);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(pRet->m_nPointType == this->m_pGeoObj->m_nPointType)) return;
		pRet->SetPoint(this->m_pWinOrg);
	}

	this->m_pWinOrg->Offset(pOffset);

	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, this->m_pWinOrg);
	CDC::SetWindowOrg(point);
}

/////////////////////////////////////////////////////////////////////////////
// Window extent
void CGDCGeo::GetWindowExt(CGSizeGeo *pRet)
{
	ASSERT(pRet);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRet)) return;

	ASSERT(m_pGeoObj); // mapping mode가 설정되어 있는지 검사
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	// 좌표 형태가 같은가를 검사
	ASSERT(pRet->m_nPointType == this->m_pGeoObj->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRet->m_nPointType == this->m_pGeoObj->m_nPointType)) return;

	pRet->SetSize(this->m_pWinExt);
}

void CGDCGeo::SetWindowExt(CGSizeGeo *pExt, CGSizeGeo *pRet)
{

	ASSERT(pExt);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pExt)) return;
	ASSERT(m_pGeoObj); // mapping mode가 설정되어 있는지 검사
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	// 좌표 형태가 같은가를 검사
	ASSERT(pExt->m_nPointType == this->m_pGeoObj->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pExt->m_nPointType == this->m_pGeoObj->m_nPointType)) return;

	if (this->m_pWinExt == NULL) this->m_pWinExt = this->m_pGeoObj->Size();
	if(pRet != NULL) {
		// 좌표 형태가 같은가를 검사
		ASSERT(pRet->m_nPointType == this->m_pGeoObj->m_nPointType);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(pRet->m_nPointType == this->m_pGeoObj->m_nPointType)) return;

		// 리턴될 윈도우 범위를 세팅한다. 
		pRet->SetSize(this->m_pWinExt);
	}

	// 윈도우 범위(Window Extent)의 값을 설정한다.
	this->m_pWinExt->SetSize(pExt);

	CSize	size;

	// 지도좌표로 논리좌표의 범위를 계산
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	size = this->GeoPSZtoLPSZ(this->m_pWinOrg, this->m_pWinExt);
	CDC::SetWindowExt(size.cx, size.cy);
}

// 테스트 중
void CGDCGeo::ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom,
						CGSizeGeo *pRet)
{
	if(pRet!=NULL) {
		// 좌표 현태가 같은가를 검사
		ASSERT(pRet->m_nPointType==m_pGeoObj->m_nPointType);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(pRet->m_nPointType==m_pGeoObj->m_nPointType)) return;

		pRet->SetSize(m_pWinExt);
	}

	CDC::ScaleWindowExt(xNum, xDenom, yNum, yDenom);
	CSize	newExt = CDC::GetWindowExt();

	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(m_pWinExt, &newExt);
}

/////////////////////////////////////////////////////////////////////////////
// Coordinate Functions
void CGDCGeo::DPtoGeoP(CGPointGeo *pPoints, LPPOINT lpPoints, int nCount)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return;
	ASSERT(lpPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPoints)) return;

	CPoint	*pTmpLP = new CPoint[nCount];
	if (!(pTmpLP))	// 메모리가 할당되지 않음
		return;

	for ( int i = 0; i < nCount; i++ )
		pTmpLP[i] = lpPoints[i];

	// 장치좌표 ==> 논리좌표
	::DPtoLP(m_hDC, (LPPOINT) pTmpLP, nCount);

	// 논리좌표 ==> 지도좌표
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(pPoints, pTmpLP, nCount);

	delete[] pTmpLP;
}

void CGDCGeo::DPtoGeoP(CGSizeGeo *pSize, LPSIZE lpSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;

	CSize tmpSize = *lpSize;
	// 장치좌표 크기 ==> 논리좌표 크기
	CDC::DPtoLP(&tmpSize);

	// 논지좌표 크기 ==> 지도좌표 크기
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(pSize, &tmpSize);
}

void CGDCGeo::DPtoGeoP(CGRectGeo *pRect, LPRECT lpRect)
{
	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;

	CRect tmpRect = *lpRect;
	// 장치좌표 영역 ==> 논리좌표 영역
	CDC::DPtoLP(&tmpRect);
	// 논리좌표 영역 ==> 지도좌표 영역
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(pRect, &tmpRect);
}

void CGDCGeo::GeoPtoDP(LPPOINT lpPoints, CGPointGeo *pPoints, int nCount)
{
	ASSERT(lpPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPoints)) return;
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return;

	// 지도좌표 ==> 논리좌표
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(lpPoints, pPoints, nCount);
	// 논리좌표 ==> 장치좌표
	::LPtoDP(m_hDC, (LPPOINT) lpPoints, nCount);
}

void CGDCGeo::GeoPtoDP(LPSIZE lpSize, CGSizeGeo *pSize)
{
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	// 지도좌표 크기 ==> 논리좌표 크기
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(lpSize, pSize);
	// 논리좌표 크기 ==> 장치좌표 크기
	CDC::LPtoDP(lpSize);
}

void CGDCGeo::GeoPtoDP(LPRECT lpRect, CGRectGeo *pRect)
{
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;
	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	// 지도좌표 영역 ==> 논리좌표 영역
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(lpRect, pRect);
	// 논리좌표 영역 ==> 장치좌표 영역
	CDC::LPtoDP(lpRect);
}

void CGDCGeo::LPtoGeoP(CGPointGeo *pPoints, LPPOINT lpPoints, int nCount)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return;
	ASSERT(lpPoints);

    CCoord coord;
	m_pCoordMgr = CMapCore::GetInstance(m_hWnd);	//GetInstance(hWnd);
    double tmp_lat, tmp_long;

	CGPointGP	*pTokGPs = NULL;
	if (nCount >= 0)
		pTokGPs = new CGPointGP[nCount]; 

	for (int i = 0; nCount > i; i++)
	{
		m_pCoordMgr->ScreenXY2WorldXY(lpPoints[i].x, lpPoints[i].y,&tmp_lat, &tmp_long);
		coord.x_y_to_degrees_lat_long(tmp_lat, tmp_long, &pTokGPs[i].m_P.lat, &pTokGPs[i].m_P.lon);
		pPoints[i].SetPoint(&((CGPointGeo *) pTokGPs)[i]);
	}
}

// 윈도우 원점을 고려해야 한다.
// 기본 값으로 윈도우 원점(Window Origin) 값은 화면 중앙
// 만약 윈도우 원점을 화면 왼쪽 상단으로 하지 않은 경우는 심한 오차가 발생가능
void CGDCGeo::LPtoGeoP(CGSizeGeo *pSizeGeo, LPSIZE lpSize)
{
	ASSERT(pSizeGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo)) return;
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;


	// ***** 중요 *****
	// 지도좌표의 크기로 변환을 위해서는 윈도우 원점(Window Origin) 값이 필요
	// 논리좌표 크기 ==> 지도좌표 크기

	// 우선 지리좌표 값만을 사용한다.	***** 중요 *****
	CGSizeGP *pSizeGP = (CGSizeGP*) pSizeGeo;


	// 윈도우 원점이 화면 외쪽 상단일 경우	***** 중요 *****
	// 논리좌표 값은 x 값은 오른쪽으로 증가
	//               y 값은 위쪽으로   증가
	CPoint LpTopLeft, LpBottomRight;
	// 왼쪽 상단 논리좌표 값을 계산한다.
	LpTopLeft.x = 0;
	LpTopLeft.y = 0;
	// 오른쪽 하단 논리좌표 값을 계산한다.
	LpBottomRight.x = lpSize->cx;
	LpBottomRight.y = lpSize->cy;
	
	// 우선 지리좌표 값만을 사용한다.		***** 중요 *****
	CGPointGP gpTopLeft, gpBottomRight;	
	// 논리좌표를 지도좌표로 변환한다.
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&gpTopLeft, &LpTopLeft);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&gpBottomRight, &LpBottomRight);

	// 지도좌표 크기를 계산한다.
	// 지도좌표 값은 lon 값은 오른쪽으로 증가
	//               lat 값은 위쪽으로   증가
	// 경도의 크기를 계산한다. : Right에서 Left값을 뺀다
	pSizeGP->m_S.clon = gpBottomRight.m_P.lon - gpTopLeft.m_P.lon;
//	// 위도의 크기를 계산한다. : Top에서 Bottom값을 뺀다
//	pSizeGP->m_S.clat = gpTopLeft.m_P.lat - gpBottomRight.m_P.lat;
	// 위도의 크기를 계산한다. : Bottom에서 Top값을 뺀다
	pSizeGP->m_S.clat = gpBottomRight.m_P.lat - gpTopLeft.m_P.lat;

}


void CGDCGeo::LPtoGeoP(CGRectGeo *pRectGeo, LPRECT lpRect)
{
	ASSERT(pRectGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRectGeo)) return;
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;
	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;
	ASSERT(m_pGeoObj->m_nPointType == pRectGeo->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj->m_nPointType == pRectGeo->m_nPointType)) return;

	// 우선 지리좌표 값만을 사용한다.	***** 중요 *****
	CGRectGP *pRectGP = (CGRectGP*) pRectGeo;

	CPoint LpTopLeft(lpRect->left, lpRect->top);
	CPoint LpBottomRight(lpRect->right, lpRect->bottom);

	// 논리좌표 ==> 지도좌표
	CGPointGP gpTopLeft, gpBottomRight;
/*	// 왼쪽 상단의 논리좌표를 지도좌표로 변환
	m_pGeoObj->Inverse(&gpTopLeft, &LpTopLeft);
	// 오른쪽 하단의 논리좌표를 지도좌표로 변환
	m_pGeoObj->Inverse(&gpBottomRight, &LpBottomRight);
*/
	// 왼쪽 상단의 논리좌표를 지도좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&gpTopLeft, &LpTopLeft, 1);
	// 오른쪽 하단의 논리좌표를 지도좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&gpBottomRight, &LpBottomRight, 1);

	pRectGP->SetRect(gpTopLeft, gpBottomRight);
}

void CGDCGeo::GeoPtoLP(LPPOINT lpPoints, CGPointGeo *pPoints, int nCount)
{
	ASSERT(lpPoints);
	if (!(lpPoints)) return;
	ASSERT(pPoints);
	if (!(pPoints)) return;
    
    CCoord coord;
	m_pCoordMgr = CMapCore::GetInstance(m_hWnd);	//GetInstance(hWnd);
    double tmp_lat, tmp_long;
    
	
	CGPointGP	*pTokGPs = NULL;
	if (nCount >= 0)
		pTokGPs = new CGPointGP[nCount];
	for (int i = 0; nCount > i; i++)
	{
		pTokGPs[i].SetPoint(&((CGPointGP *) pPoints)[i]);
		coord.degrees_lat_long_to_x_y(pTokGPs[i].m_P.lat,pTokGPs[i].m_P.lon,&tmp_lat, &tmp_long);
		m_pCoordMgr->WorldXY2ScreenXY(tmp_lat, tmp_long, &lpPoints[i].x, &lpPoints[i].y);
	}
	delete [] pTokGPs;
}

// 지도좌표의 크기를 논리좌표의 크기로 변환하는 함수
void CGDCGeo::GeoPtoLP(LPSIZE lpSize, CGSizeGeo *pSizeGeo)
{
 	ASSERT(lpSize);
	if (!(lpSize)) return;
	ASSERT(pSizeGeo);
	if (!(pSizeGeo)) return;   

	//double tmp_lat, tmp_long;
	CCoord coord;
	CGPointGP gpTopLeft, gpBottomRight;
	CGSizeGP *pSizeGP = (CGSizeGP*) pSizeGeo;
	CPoint LpTopLeft, LpBottomRight;

	
	gpTopLeft.m_P.lat = 37.0;
    gpTopLeft.m_P.lon = 127.0;

	// 오른쪽 하단 지도좌표 값을 계산한다.
	gpBottomRight.m_P.lon = 127.0 + pSizeGP->m_S.clon;
	gpBottomRight.m_P.lat = 37.0 + pSizeGP->m_S.clat;
	
	// 왼쪽 상단의 지도좌표를 논리좌표로 변환한다.
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpTopLeft, &gpTopLeft);
	// 오른쪽 하단의 지도좌표를 논리좌표로 변환한다.
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpBottomRight, &gpBottomRight);

	// 논리좌표 크기를 계산한다.
	// 논리좌표 값은 x 값은 오른쪽으로 증가
	//               y 값은 위쪽으로   증가
	// x 의 크기를 계산한다. : Right에서 Left값을 뺀다
	lpSize->cx = LpBottomRight.x - LpTopLeft.x;
//	// y 의 크기를 계산한다. : Top에서 Bottom을 뺀다
//	lpSize->cy = LpTopLeft.y - LpBottomRight.y;
	// y 의 크기를 계산한다. : Bottom에서 Top을 뺀다
	lpSize->cy = LpBottomRight.y - LpTopLeft.y;	
}

void CGDCGeo::GeoPtoLP(LPRECT lpRect, CGRectGeo *pRectGeo)
{
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;
	ASSERT(pRectGeo);
	// Release에서 Down되지 않도록 if문으로 처리

	// 우선 지리좌표 값만을 사용한다.	***** 중요 *****
	CGRectGP *pRectGP = (CGRectGP*) pRectGeo;

	CGPointGP gpTopLeft(pRectGP->m_R.left, pRectGP->m_R.top);
	CGPointGP gpBottomRight(pRectGP->m_R.right, pRectGP->m_R.bottom);

	// 지도좌표 ==> 논리좌표
	CPoint LpTopLeft, LpBottomRight;
/*	// 왼쪽 상단의 지도좌표를 논리좌표로 변환
	m_pGeoObj->Forward(&LpTopLeft, &gpTopLeft);
	// 오른쪽 하단의 지도좌표를 논리좌표로 변환
	m_pGeoObj->Forward(&LpBottomRight, &gpBottomRight);
*/
	// 왼쪽 상단의 지도좌표를 논리좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpTopLeft, &gpTopLeft);
	// 오른쪽 하단의 지도좌표를 논리좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpBottomRight, &gpBottomRight);

	lpRect->left = LpTopLeft.x;
	lpRect->top  = LpTopLeft.y;
	lpRect->right  = LpBottomRight.x;
	lpRect->bottom = LpBottomRight.y;
}

/*
void CGDCGeo::GeoPtoLP(LPPOINT lpPoints, CGPointGeo *pPoints, int nCount)
{
	ASSERT(lpPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPoints)) return;
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return;
	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사.
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	// 타원체가 Bessel인 경우 Bessel로 변환한다.
	if (this->m_pGeoObj->GetEllipsoid() == EWGE)
	{
		// 지도좌표 ==> 논리좌표
		m_pGeoObj->Forward(lpPoints, pPoints, nCount);
	}
	else
	{
		CGPointGP	*pTokGPs = NULL;
		if (nCount >= 0)
			pTokGPs = new CGPointGP[nCount];
		for (int i = 0; nCount > i; i++)
			pTokGPs[i].SetPoint(&((CGPointGP *) pPoints)[i]);
		
		if (pTokGPs)
			::DTWgsToTok(pTokGPs, pTokGPs, nCount);

		// 지도좌표 ==> 논리좌표
		m_pGeoObj->Forward(lpPoints, pTokGPs, nCount);

		delete [] pTokGPs;
	}
}

// 지도좌표의 크기를 논리좌표의 크기로 변환하는 함수
void CGDCGeo::GeoPtoLP(LPSIZE lpSize, CGSizeGeo *pSizeGeo)
{
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;

	ASSERT(pSizeGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo)) return;

	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사.
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;

	ASSERT(m_pGeoObj->m_nPointType == pSizeGeo->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj->m_nPointType == pSizeGeo->m_nPointType)) return;

	// ***** 중요 *****
	// 지도좌표의 크기로 변환을 위해서는 윈도우 원점(Window Origin) 값이 필요
	// 지도좌표 크기 ==> 논리좌표 크기
	// m_pGeoObj->Forward(lpSize, pSize);

	// 우선 지리좌표 값만을 사용한다.	***** 중요 *****
	CGSizeGP *pSizeGP = (CGSizeGP*) pSizeGeo;

	CGPointGP gpWinOrg;	// 지리좌표 원점
	this->GetWindowOrg(&gpWinOrg);

	// 윈도우 원점이 화면 왼쪽 상단일 경우	***** 중요 *****
	// 지도좌표 값은 lon 값은 오른쪽으로 증가
	//               lat 값은 위쪽으로   증가
	CGPointGP gpTopLeft, gpBottomRight;
	// 왼쪽 상단 지도좌표 값을 계산한다.
	gpTopLeft.m_P.lon = gpWinOrg.m_P.lon;
	gpTopLeft.m_P.lat = gpWinOrg.m_P.lat;
	// 오른쪽 하단 지도좌표 값을 계산한다.
	gpBottomRight.m_P.lon = gpWinOrg.m_P.lon + pSizeGP->m_S.clon;
	gpBottomRight.m_P.lat = gpWinOrg.m_P.lat + pSizeGP->m_S.clat;

	CPoint LpTopLeft, LpBottomRight;

	// 왼쪽 상단의 지도좌표를 논리좌표로 변환한다.
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpTopLeft, &gpTopLeft);
	// 오른쪽 하단의 지도좌표를 논리좌표로 변환한다.
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpBottomRight, &gpBottomRight);

	// 논리좌표 크기를 계산한다.
	// 논리좌표 값은 x 값은 오른쪽으로 증가
	//               y 값은 위쪽으로   증가
	// x 의 크기를 계산한다. : Right에서 Left값을 뺀다
	lpSize->cx = LpBottomRight.x - LpTopLeft.x;
//	// y 의 크기를 계산한다. : Top에서 Bottom을 뺀다
//	lpSize->cy = LpTopLeft.y - LpBottomRight.y;
	// y 의 크기를 계산한다. : Bottom에서 Top을 뺀다
	lpSize->cy = LpBottomRight.y - LpTopLeft.y;

}

void CGDCGeo::GeoPtoLP(LPRECT lpRect, CGRectGeo *pRectGeo)
{
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;
	ASSERT(pRectGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRectGeo)) return;
	ASSERT(m_pGeoObj);	// mapping mode가 설정되어 있는지 검사.
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj)) return;
	ASSERT(m_pGeoObj->m_nPointType == pRectGeo->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(m_pGeoObj->m_nPointType == pRectGeo->m_nPointType)) return;

	// 우선 지리좌표 값만을 사용한다.	***** 중요 *****
	CGRectGP *pRectGP = (CGRectGP*) pRectGeo;

	CGPointGP gpTopLeft(pRectGP->m_R.left, pRectGP->m_R.top);
	CGPointGP gpBottomRight(pRectGP->m_R.right, pRectGP->m_R.bottom);

	// 지도좌표 ==> 논리좌표
	CPoint LpTopLeft, LpBottomRight;

	// 왼쪽 상단의 지도좌표를 논리좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpTopLeft, &gpTopLeft);
	// 오른쪽 하단의 지도좌표를 논리좌표로 변환
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpBottomRight, &gpBottomRight);

	lpRect->left = LpTopLeft.x;
	lpRect->top  = LpTopLeft.y;
	lpRect->right  = LpBottomRight.x;
	lpRect->bottom = LpBottomRight.y;
}
*/

/////////////////////////////////////////////////////////////////////////////
// Special Coordinate Functions (useful for dealing with metafiles and OLE)
void CGDCGeo::GeoPtoHIMETRIC(LPSIZE lpSize, CGSizeGeo *pSize)
{
	lpSize = lpSize;
	pSize = pSize;
}

void CGDCGeo::HIMETRICtoGeoP(CGSizeGeo *pSize, LPSIZE lpSize)
{
	pSize = pSize;
	lpSize = lpSize;
}

/////////////////////////////////////////////////////////////////////////////
// Type-Safe Selection Helpers
CGdiObject* CGDCGeo::SelectObject(CGdiObject *pGdiObj)
{	return (CGdiObject*) CDC::SelectObject(pGdiObj);	}

CPen* CGDCGeo::SelectObject(CPen* pPen)
{	return CDC::SelectObject(pPen);		}

CBrush* CGDCGeo::SelectObject(CBrush* pBrush)
{	return CDC::SelectObject(pBrush);	}

CFont* CGDCGeo::SelectObject(CFont* pFont)
{	return CDC::SelectObject(pFont);	}

CBitmap* CGDCGeo::SelectObject(CBitmap* pBitmap)
{	return CDC::SelectObject(pBitmap);	}

int CGDCGeo::SelectObject(CRgn* pRgn)
{	return CDC::SelectObject(pRgn);		}

int CGDCGeo::SelectObject(CGRgn *pRgn)
{
	int nRetVal = ERROR;

	pRgn->SetGRgnDC();
	nRetVal = CDC::SelectObject(pRgn->m_pRgn);
	pRgn->RestoreDC();

	return nRetVal;

}

/////////////////////////////////////////////////////////////////////////////
// Region Functions
BOOL CGDCGeo::FillRgn(CRgn* pRgn, CBrush* pBrush)
{	return CDC::FillRgn(pRgn, pBrush);			}
BOOL CGDCGeo::FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight)
{	return CDC::FrameRgn(pRgn, pBrush, nWidth, nHeight);	}
BOOL CGDCGeo::InvertRgn(CRgn* pRgn)
{	return CDC::InvertRgn(pRgn);	}
BOOL CGDCGeo::PaintRgn(CRgn* pRgn)
{	return CDC::PaintRgn(pRgn);	}

BOOL CGDCGeo::FillRgn(CGRgn* pRgn, CBrush* pBrush)
{
	BOOL bRetVal = FALSE;

	pRgn->SetGRgnDC();
	bRetVal = CDC::FillRgn(pRgn->m_pRgn, pBrush);
	pRgn->RestoreDC();

	return bRetVal;
}

BOOL CGDCGeo::FrameRgn(CGRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight)
{
	BOOL bRetVal = FALSE;

	pRgn->SetGRgnDC();
	bRetVal = CDC::FrameRgn(pRgn->m_pRgn, pBrush, nWidth, nHeight);
	pRgn->RestoreDC();

	return bRetVal;
}

BOOL CGDCGeo::InvertRgn(CGRgn* pRgn)
{
	BOOL bRetVal = FALSE;

	pRgn->SetGRgnDC();
	bRetVal = CDC::InvertRgn(pRgn->m_pRgn);
	pRgn->RestoreDC();

	return bRetVal;
}

BOOL CGDCGeo::PaintRgn(CGRgn* pRgn)
{
	BOOL bRetVal = FALSE;

	pRgn->SetGRgnDC();
	bRetVal = CDC::PaintRgn(pRgn->m_pRgn);
	pRgn->RestoreDC();

	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// Clipping Functions
// 테스트 중
UINT CGDCGeo::SetBoundsRect(CGRectGeo *pRect, UINT flags)
{
	CRect	rect;

	if(pRect==NULL)
	{
		return CDC::SetBoundsRect(NULL, flags);
	}
	else
	{
		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&rect, pRect);
		return CDC::SetBoundsRect(&rect, flags);
	}
}

// 테스트 중
UINT CGDCGeo::GetBoundsRect(CGRectGeo *pRect, UINT flags)
{
	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	// DCB_RESET : Bounding rectangle is empty.
	if (!(pRect)) return DCB_RESET;

	CRect	rectBounds;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rectBounds, pRect);

	return CDC::GetBoundsRect(&rectBounds, flags);
}

// 테스트 중
int CGDCGeo::GetClipBox(CGRectGeo *pRect)
{
	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return ERROR;

	CRect	rect;
	int	retValue = ERROR;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	retValue = CDC::GetClipBox(&rect);

	return retValue;
}

int CGDCGeo::SelectClipRgn(CRgn* pRgn)
{	return CDC::SelectClipRgn(pRgn);			}
int CGDCGeo::SelectClipRgn(CRgn* pRgn, int nMode)
{	return CDC::SelectClipRgn(pRgn, nMode);	}

int CGDCGeo::SelectClipRgn(CGRgn* pRgn)
{
	int nRetVal = ERROR;

	if (pRgn)
	{
		pRgn->SetGRgnDC();
		nRetVal = CDC::SelectClipRgn(pRgn->m_pRgn);
		pRgn->RestoreDC();
	}
	else	// 전체 영역이 선택된다.
	{
		nRetVal = ERROR;
	}

	return nRetVal;
}

int CGDCGeo::SelectClipRgn(CGRgn* pRgn, int nMode)
{
	int nRetVal = ERROR;

	if (pRgn)
	{
		pRgn->SetGRgnDC();
		nRetVal = CDC::SelectClipRgn(pRgn->m_pRgn, nMode);
		pRgn->RestoreDC();
	}
	else	// 전체 영역이 선택된다.
	{
		nRetVal = ERROR;
	}

	return nRetVal;
}

int CGDCGeo::ExcludeClipRect(double lon1, double lat1, double lon2, double lat2)
{
	int nRetVal = ERROR;

	CGPointGP gp[2];
	gp[0].SetPoint(lon1, lat1);
	gp[0].SetPoint(lon2, lat2);

	CPoint LP[2];
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(LP, gp, 2);

	nRetVal = CDC::ExcludeClipRect(LP[0].x, LP[0].y, LP[1].x, LP[1].y);

	return nRetVal;
}

int CGDCGeo::ExcludeClipRect(CGPointGeo *pPoint1, CGPointGeo *pPoint2)
{
	int nRetVal = ERROR;

	if (pPoint1 && pPoint2)
	{
		if (pPoint1->m_nPointType == GPTYPE_GP && pPoint2->m_nPointType == GPTYPE_GP)
		{
			CGPointGP *gp1, *gp2;
			gp1 = (CGPointGP *) pPoint1;
			gp2 = (CGPointGP *) pPoint2;

			nRetVal = ExcludeClipRect(gp1->m_P.lon, gp2->m_P.lat,
									  gp2->m_P.lon, gp2->m_P.lat);
		}
	}

	return nRetVal;
}

int CGDCGeo::ExcludeClipRect(CGRectGeo *pRect)
{
	int nRetVal = ERROR;

	if (pRect)
	{
		CRect	rect;
		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&rect, pRect);

		nRetVal = CDC::ExcludeClipRect(&rect);
	}
	
	return nRetVal;
}

int CGDCGeo::ExcludeUpdateRgn(CWnd* pWnd)
{
	return CDC::ExcludeUpdateRgn(pWnd);
}

int CGDCGeo::IntersectClipRect(double lon1, double lat1, double lon2, double lat2)
{
	int nRetVal = ERROR;

	CGPointGP gp[2];
	gp[0].SetPoint(lon1, lat1);
	gp[0].SetPoint(lon2, lat2);

	CPoint LP[2];
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(LP, gp, 2);

	nRetVal = CDC::IntersectClipRect(LP[0].x, LP[0].y, LP[1].x, LP[1].y);

	return nRetVal;
}

int CGDCGeo::IntersectClipRect(CGPointGeo *pPoint1, CGPointGeo *pPoint2)
{
	int nRetVal = ERROR;

	if (pPoint1 && pPoint2)
	{
		if (pPoint1->m_nPointType == GPTYPE_GP && pPoint2->m_nPointType == GPTYPE_GP)
		{
			CGPointGP *gp1, *gp2;
			gp1 = (CGPointGP *) pPoint1;
			gp2 = (CGPointGP *) pPoint2;

			nRetVal = IntersectClipRect(gp1->m_P.lon, gp2->m_P.lat,
									  gp2->m_P.lon, gp2->m_P.lat);
		}
	}

	return nRetVal;
}

int CGDCGeo::IntersectClipRect(CGRectGeo *pRect)
{
	int nRetVal = ERROR;

	if (pRect)
	{
		CRect	rect;
		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&rect, pRect);

		nRetVal = CDC::IntersectClipRect(&rect);
	}
	
	return nRetVal;
}

int CGDCGeo::OffsetClipRgn(double clon, double clat)
{
	CGSizeGP sizeGP(clon, clat);
	CSize sizeLP;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&sizeLP, &sizeGP);

	return CDC::OffsetClipRgn(sizeLP.cx, sizeLP.cy);
}

int CGDCGeo::OffsetClipRgn(CGSizeGP *pSizeGP)
{
	int nRetVal = ERROR;

	if (pSizeGP)
	{
		if (pSizeGP->m_nPointType == GPTYPE_GP)
		{
			CGSizeGP *pSZ = (CGSizeGP *) pSizeGP;
			nRetVal = OffsetClipRgn(pSZ->m_S.clon, pSZ->m_S.clat);
		}
	}
	
	return nRetVal;
}

BOOL CGDCGeo::PtVisible(double lon, double lat)
{
	CGPointGP GP(lon, lat);
	CPoint LP;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&LP, &GP, 1);

	return CDC::PtVisible(LP.x, LP.y);
}

BOOL CGDCGeo::PtVisible(CGPointGeo *pPoint)
{
	BOOL bRetVal = FALSE;

	if (pPoint)
	{
		if (pPoint->m_nPointType == GPTYPE_GP)
		{
			CGPointGP *pGP = (CGPointGP *) pPoint;
			bRetVal = PtVisible(pGP->m_P.lon, pGP->m_P.lat);
		}
	}
	return bRetVal;
}

BOOL CGDCGeo::RectVisible(CGRectGeo *pRectGeo)
{
	BOOL bRetVal = FALSE;

	if (pRectGeo)
	{
		if (pRectGeo->m_nPointType == GPTYPE_GP)
		{
			CGRectGP *pRectGP = (CGRectGP *) pRectGeo;
			CRect rectLP;
			// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
			GeoPtoLP(&rectLP, pRectGP);
			bRetVal = CDC::RectVisible(&rectLP);
		}
	}
	return bRetVal;
}





/////////////////////////////////////////////////////////////////////////////
// Line-Output Functions
void CGDCGeo::GetCurrentPosition(CGPointGeo *pPoint)
{
	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	CPoint curPoint;

	// get current position in logical unit.
	curPoint = CDC::GetCurrentPosition();

	// to geometric unit
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pPoint, &curPoint);
}

void CGDCGeo::MoveTo(CGPointGeo *pMove, CGPointGeo *pPrev)
{
	ASSERT(pMove);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pMove)) return;

	CPoint		point;
	CPoint		prevPoint;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&point, pMove);
	prevPoint = CDC::MoveTo(point);

	if(pPrev != NULL)
	{
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
		LPtoGeoP(pPrev, &prevPoint);
	}
}

BOOL CGDCGeo::LineTo(CGPointGeo *pLine, BOOL bGrid)
{
	ASSERT(pLine);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLine)) return FALSE;

	ASSERT(pLine->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLine->m_nPointType == GPTYPE_GP)) return FALSE;

	CPoint	point;
	BOOL	bRet = TRUE;
	
	if (bGrid && (m_pGeoObj->GetMapMode() == GMM_UTM || m_pGeoObj->GetMapMode() == GMM_LCC))
	{
		CGPointGP startGP, *endGP;
		// 경위선망 그리드를 그린다.
		this->GetCurrentPosition(&startGP);
		endGP = (CGPointGP*) pLine;
		
		CGSizeGP sizeGP(endGP->m_P.lon - startGP.m_P.lon,
					    endGP->m_P.lat - startGP.m_P.lat);

//		sizeGP.m_S.clat = (sizeGP.m_S.clat > 0) ? sizeGP.m_S.clat: sizeGP.m_S.clat * (-1.0);
//		sizeGP.m_S.clon = (sizeGP.m_S.clon > 0) ? sizeGP.m_S.clon: sizeGP.m_S.clon * (-1.0);

		// 한 픽셀의 크기를 읽어온다.
		CGSizeGP pixelSizeGP = this->GetPixelSizeGP();

		int nXcnt, nYcnt;
		nXcnt = (int) (sizeGP.m_S.clon / (pixelSizeGP.m_S.clon * 1.0));
		nYcnt = (int) (sizeGP.m_S.clat / (pixelSizeGP.m_S.clat * 1.0));

		// 큰 값을 분할하는 값으로 정한다.
		int nCount = (nXcnt > nYcnt) ? nXcnt : nYcnt;

		CGSizeGP incSizeGP;
		incSizeGP.m_S.clon = sizeGP.m_S.clon / (double) nCount;
		incSizeGP.m_S.clat = sizeGP.m_S.clat / (double) nCount;

		int i;
		CGPointGP gp = startGP;
		CPoint LP;
		for (i = 0; i < nCount - 1; i++)
		{
			startGP.m_P.lon += incSizeGP.m_S.clon;
			startGP.m_P.lat += incSizeGP.m_S.clat;

			// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
			this->GeoPtoLP(&LP, &startGP);
			bRet = ::LineTo(m_hDC, LP.x, LP.y);
		}

		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		this->GeoPtoLP(&LP, endGP);
		bRet = ::LineTo(m_hDC, LP.x, LP.y);
	}
	else
	{
		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&point, pLine);
		bRet = ::LineTo(m_hDC, point.x, point.y);
	}

	return bRet;
}

BOOL CGDCGeo::Arc(CGPointGeo *pP1, CGPointGeo *pP2, CGPointGeo *pP3,
				CGPointGeo *pP4, BOOL bGrid)
{
	ASSERT(pP1 && pP2 && pP3 && pP4);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pP1 && pP2 && pP3 && pP4)) return FALSE;

	CPoint	pt1, pt2, pt3, pt4;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt1, pP1);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt2, pP2);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt3, pP3);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt4, pP4);

	bGrid = bGrid;
	return CDC::Arc(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);
}

BOOL CGDCGeo::Arc(CGRectGeo *pRect, CGPointGeo *pStart, CGPointGeo *pEnd,
						BOOL bGrid)
{
	CRect	rect;
	CPoint	ptStart, ptEnd;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptStart, pStart);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptEnd, pEnd);

	bGrid = bGrid;
	return CDC::Arc(&rect, ptStart, ptEnd);
}

BOOL CGDCGeo::ArcTo(CGPointGeo *pP1, CGPointGeo *pP2, CGPointGeo *pP3,
				CGPointGeo *pP4, BOOL bGrid)
{
	ASSERT(pP1 && pP2 && pP3 && pP4);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pP1 && pP2 && pP3 && pP4)) return FALSE;

	CPoint	pt1, pt2, pt3, pt4;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt1, pP1);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt2, pP2);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt3, pP3);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt4, pP4);
	
	bGrid = bGrid;
	return CDC::ArcTo(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);
}

BOOL CGDCGeo::ArcTo(CGRectGeo *pRect, CGPointGeo *pStart, CGPointGeo *pEnd,
						BOOL bGrid)
{
	CRect	rect;
	CPoint	ptStart, ptEnd;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptStart, pStart);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptEnd, pEnd);

	bGrid = bGrid;
	return CDC::Arc(&rect, ptStart, ptEnd);
}

// 테스트 중
BOOL CGDCGeo::AngleArc(CGPointGeo *pCenter, double dRadius, float fStartAngle,
				float fSweepAngle, BOOL bGrid)
{
	CPoint		ptCenter;
	CSize		size;
	CGSizeGeo	*radius = CreateSize();

	ASSERT(radius);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(radius)) return FALSE;

	radius->SetSize(dRadius, dRadius);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&size, radius);
	delete radius;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptCenter, pCenter);

	bGrid = bGrid;
	return CDC::AngleArc(ptCenter.x, ptCenter.y, size.cx,
						fStartAngle, fSweepAngle);
}

//2005.10.27 jdkim
BOOL CGDCGeo::AngleArc2(CGPointGeo *pCenter, CPoint ptRad, float fStartAngle,
				float fSweepAngle, BOOL bGrid)
{
	CPoint		ptCenter;
	CSize		size;
	CGSizeGeo	*radius = CreateSize();

	ASSERT(radius);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(radius)) return FALSE;


	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoDP(&ptCenter, pCenter);
	CPoint ptCenterLeft = ptCenter+ptRad;
	CGPointGP pGP, pGPCenter;

	CPoint ptr = ptCenter - ptCenterLeft;
	//size.cx = ptr.x;

//	if(ptr.x<0)		ptr.x=ptr.x*-1;
//	if(ptr.y<0)		ptr.y=ptr.y*-1;
	
#pragma warning(disable : 4244)
	int ptrDistance = (int)sqrt((double)((ptr.x*ptr.x)+(ptr.y*ptr.y)));
#pragma warning(default : 4244)

	size.cx = ptrDistance;
	DPtoLP(&size);
	DPtoLP(&ptCenter);
	bGrid = bGrid;
	return CDC::AngleArc(ptCenter.x, ptCenter.y, size.cx,
						fStartAngle, fSweepAngle);
}

BOOL CGDCGeo::PolyDraw(CGPointGeo *pPoints, const BYTE* pTypes,
					int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount>0)) return FALSE;

	BOOL	bRet;

	CPoint	*lpPts = new CPoint[nCount];
	ASSERT(lpPts);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(lpPts, pPoints, nCount);
	bRet = CDC::PolyDraw(lpPts, pTypes, nCount);

	delete[] lpPts;

	bGrid = bGrid;
	return bRet;
}

BOOL CGDCGeo::Polyline(CGPointGeo *pPoints, int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount > 0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount > 0)) return FALSE;

	BOOL	bRet = TRUE;


	if (bGrid && this->m_pGeoObj->GetMapMode() == GMM_UTM)
	{
		// 경위선 망으로 그린다.
		CPoint curPoint = this->CDC::GetCurrentPosition();
		
		for (int i = 0; i < nCount; i++)
			bRet = this->LineTo(&pPoints[i], TRUE);

		// 다 그린 후 이전의 위치로 되돌린다.
		this->CDC::MoveTo(curPoint);
	}
	else
	{
		CPoint	*lpPts = new CPoint[nCount];
		ASSERT(lpPts);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		this->GeoPtoLP(lpPts, pPoints, nCount);
		bRet = ::Polyline(m_hDC, lpPts, nCount);
	
		delete[] lpPts;
	}

	return bRet;
}

BOOL CGDCGeo::PolyPolyline(CGPointGeo *pPoints, const DWORD *pPolyPoints,
					int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(pPolyPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPolyPoints)) return FALSE;

	ASSERT(nCount > 0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount > 0)) return FALSE;

	BOOL	bRet = TRUE;

	if (bGrid && this->m_pGeoObj->GetMapMode() == GMM_UTM)
	{
		// 경위선 망으로 그린다.
		CPoint curPoint = this->CDC::GetCurrentPosition();
		
		for (int i = 0; i < nCount; i++)
			bRet = this->Polyline(&pPoints[i], pPolyPoints[i], TRUE);

		// 다 그린 후 이전의 위치로 되돌린다.
		this->CDC::MoveTo(curPoint);
	}
	else
	{
		CPoint	*lpPts = new CPoint[nCount];
		ASSERT(lpPts);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(lpPts)) return FALSE;

		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		this->GeoPtoLP(lpPts, pPoints, nCount);
		bRet = CDC::PolyPolyline(lpPts, pPolyPoints, nCount);

		delete[] lpPts;
	}

	return bRet;
}

BOOL CGDCGeo::PolylineTo(CGPointGeo *pPoints, int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount > 0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount > 0)) return FALSE;

	BOOL	bRet = TRUE;

	if (bGrid && this->m_pGeoObj->GetMapMode() == GMM_UTM)
	{
		// 경위선 망으로 그린다.
		CPoint curPoint = this->CDC::GetCurrentPosition();
		
		for (int i = 0; i < nCount; i++)
			bRet = this->LineTo(&pPoints[i], TRUE);
	}
	else
	{
		CPoint	*lpPts = new CPoint[nCount];
		ASSERT(lpPts);
		// Release에서 Down되지 않도록 if문으로 처리
		if (!(lpPts)) return FALSE;
		
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		this->GeoPtoLP(lpPts, pPoints, nCount);
		bRet = ::PolylineTo(m_hDC, lpPts, (DWORD) nCount);
	
		delete[] lpPts;
	}

	return bRet;
}

BOOL CGDCGeo::PolyBezier(CGPointGeo *pPoints, int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount>0)) return FALSE;

	BOOL	bRet;

	CPoint	*lpPts = new CPoint[nCount];
	ASSERT(lpPts);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(lpPts, pPoints, nCount);
	bRet = CDC::PolyBezier(lpPts, nCount);

	delete[] lpPts;

	bGrid = bGrid;
	return bRet;
}

BOOL CGDCGeo::PolyBezierTo(CGPointGeo *pPoints, int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount>0)) return FALSE;

	BOOL	bRet;

	CPoint	*lpPts = new CPoint[nCount];
	ASSERT(lpPts);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(lpPts, pPoints, nCount);
	bRet = CDC::PolyBezierTo(lpPts, nCount);

	delete[] lpPts;

	bGrid = bGrid;
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// Simple Drawing Functions
void CGDCGeo::FillRect(CGRectGeo *pRect, CBrush* pBrush, BOOL bGrid)
{
	CRect	rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	CDC::FillRect(&rect, pBrush);
}

void CGDCGeo::FrameRect(CGRectGeo *pRect, CBrush* pBrush, BOOL bGrid)
{
	CRect	rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	CDC::FrameRect(&rect, pBrush);
}

void CGDCGeo::InvertRect(CGRectGeo *pRect, BOOL bGrid)
{
	CRect	rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	CDC::InvertRect(&rect);
}

BOOL CGDCGeo::DrawIcon(CGPointGeo *pPoint, HICON hIcon)
{
	CPoint	point;
	CRect rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&point, pPoint);

	return CDC::DrawIcon(point, hIcon);}

BOOL CGDCGeo::DrawIcon(CPoint *pPoint, HICON hIcon)
{
	CPoint	point;
	CRect rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
//	GeoPtoLP(&point, pPoint);

	point.x = pPoint->x;
	point.y = pPoint->y;
	return CDC::DrawIcon(point, hIcon);
}

void CGDCGeo::DrawDragRect(CGRectGeo *pRect, CGSizeGeo *pSize,
			   CGRectGeo *pRectLast, CGSizeGeo *pSizeLast,
			   CBrush* pBrush, CBrush* pBrushLast, BOOL bGrid)
{
	CRect	rect, rectLast;
	CSize	size, sizeLast;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rectLast, pRectLast);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&size, pSize);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&sizeLast, pSizeLast);

	bGrid = bGrid;
	CDC::DrawDragRect(&rect, size, &rectLast, sizeLast, pBrush, pBrushLast);
}

void CGDCGeo::FillSolidRect(int x, int y, int cx, int cy, COLORREF clr )
{

	CDC::FillSolidRect(x,y,cx,cy,clr);
}

void CGDCGeo::FillSolidRect(CGRectGeo *pRect, COLORREF clr, BOOL bGrid)
{
	CRect	rect;
	
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	CDC::FillSolidRect(&rect, clr);
}

void CGDCGeo::Draw3dRect(CGRectGeo *pRect, COLORREF clrTopLeft,
				COLORREF clrBottomRight, BOOL bGrid)
{
	CRect	rect;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	CDC::Draw3dRect(&rect, clrTopLeft, clrBottomRight);
}

BOOL CGDCGeo::DrawEdge(CGRectGeo *pRect, UINT nEdge, UINT nFlags, BOOL bGrid)
{
	CRect	rect;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	return CDC::DrawEdge(&rect, nEdge, nFlags);
}

BOOL CGDCGeo::DrawFrameControl(CGRectGeo *pRect, UINT nType, UINT nState,
							BOOL bGrid)
{
	CRect	rect;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	return CDC::DrawFrameControl(&rect, nType, nState);
}

/////////////////////////////////////////////////////////////////////////////
// Ellipse and Polygon Functions
BOOL CGDCGeo::Chord(CGRectGeo *pRect, CGPointGeo *pStart, CGPointGeo *pEnd,
							BOOL bGrid)
{
	CRect	rect;
	CPoint	ptStart, ptEnd;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptStart, pStart);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptEnd, pEnd);

	bGrid = bGrid;
	return CDC::Chord(&rect, ptStart, ptEnd);
}

BOOL CGDCGeo::Chord(CGPointGeo *pP1, CGPointGeo *pP2, CGPointGeo *pP3,
					CGPointGeo *pP4, BOOL bGrid)
{
	ASSERT(pP1 && pP2 && pP3 && pP4);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pP1 && pP2 && pP3 && pP4)) return FALSE;

	CPoint	pt1, pt2, pt3, pt4;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt1, pP1);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt2, pP2);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt3, pP3);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt4, pP4);
	
	bGrid = bGrid;
	return CDC::Chord(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);
}

void CGDCGeo::DrawFocusRect(CGRectGeo *pRect)
{
	ASSERT(pRect->m_nPointType);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect->m_nPointType)) return;

	CRect	rect;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&rect, pRect);

	CDC::DrawFocusRect(&rect);
}

BOOL CGDCGeo::Ellipse(CGRectGeo *pRect, BOOL bGrid)
{
	CRect	rect;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	bGrid = bGrid;
	return CDC::Ellipse(&rect);
}

BOOL CGDCGeo::Pie(CGRectGeo *pRect, CGPointGeo *pStart, CGPointGeo *pEnd,
						BOOL bGrid)
{
	CRect	rect;
	CPoint	ptStart, ptEnd;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptStart, pStart);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptEnd, pEnd);

	bGrid = bGrid;
	return CDC::Pie(&rect, ptStart, ptEnd);
}

BOOL CGDCGeo::Pie(CGPointGeo *pP1, CGPointGeo *pP2, CGPointGeo *pP3,
				CGPointGeo *pP4, BOOL bGrid)
{
	ASSERT(pP1 && pP2 && pP3 && pP4);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pP1 && pP2 && pP3 && pP4)) return FALSE;

	CPoint	pt1, pt2, pt3, pt4;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt1, pP1);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt2, pP2);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt3, pP3);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt4, pP4);
	
	bGrid = bGrid;
	return CDC::Pie(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);
}

BOOL CGDCGeo::Polygon(CGPointGeo *pPoints, int nCount, BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount>0)) return FALSE;

	BOOL	bRet;

	CPoint	*lpPts = new CPoint[nCount];
	ASSERT(lpPts);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(lpPts, pPoints, nCount);

	CSize size = CDC::GetWindowExt();

//	bRet = CDC::Polygon(lpPts, nCount);
	bRet = ::Polygon(m_hDC, lpPts, nCount);

	delete[] lpPts;

	bGrid = bGrid;
	return bRet;
}

BOOL CGDCGeo::PolyPolygon(CGPointGeo *pPoints, LPINT lpPolyCounts, int nCount,
						BOOL bGrid)
{
	ASSERT(pPoints);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints)) return FALSE;

	ASSERT(nCount>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nCount>0)) return FALSE;

	BOOL	bRet;
	int	countSum, i;

	countSum = 0;
	for(i=0; i<nCount; i++) {
		countSum += lpPolyCounts[i];
	}

	CPoint	*lpPts = new CPoint[countSum];
	ASSERT(lpPts);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPts)) return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(lpPts, pPoints, countSum);
	bRet = CDC::PolyPolygon(lpPts, lpPolyCounts, nCount);

	delete[] lpPts;

	bGrid = bGrid;
	return bRet;
}

BOOL CGDCGeo::Rectangle(CGRectGeo *pRectGeo, BOOL bGrid)
{
	CRect	rect;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRectGeo);

//	return CDC::Rectangle(&rect);
	bGrid = bGrid;
	return ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
}


BOOL CGDCGeo::Rectangle(CGPointGeo *pP1, CGPointGeo *pP2, BOOL bGrid)
{
	CGRectGP	rectGP;
	rectGP.SetRect(pP1, pP2);

	return this->Rectangle(&rectGP, bGrid);
}

BOOL CGDCGeo::Rectangle(double lon1, double lat1, double lon2, double lat2, BOOL bGrid)
{
	CGRectGP rectGP(lon1, lat1, lon2, lat2);

	return this->Rectangle(&rectGP, bGrid);
}

BOOL CGDCGeo::RoundRect(CGRectGeo *pRect, CGPointGeo *pPoint, BOOL bGrid)
{
	CRect	rect;
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&point, pPoint);

	bGrid = bGrid;
	return CDC::RoundRect(&rect, point);
}

/////////////////////////////////////////////////////////////////////////////
// Bitmap Functions
BOOL CGDCGeo::PatBlt(double lon, double lat,
					 double clon, double clat, DWORD dwRop)
{
	CGPointGP gpLeftTop(lon, lat);
	CGSizeGP gpSizeGP(clon, clat);

	return this->PatBlt(&gpLeftTop, &gpSizeGP, dwRop);
}

BOOL CGDCGeo::PatBlt(CGPointGeo *pLeftTop, CGSizeGeo *pSizeGeo, DWORD dwRop)
{
	ASSERT(pLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	CPoint ptPoint;
	CSize  ptSize;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptPoint, pLeftTop);
	ptSize = this->GeoPSZtoLPSZ(pLeftTop, pSizeGeo);

//	return CDC::PatBlt(ptPoint.x, ptPoint.y,
//					   ptSize.cx, ptSize.cy, dwRop);
	return ::PatBlt(m_hDC, ptPoint.x, ptPoint.y,
					       ptSize.cx, ptSize.cy, dwRop);
}

BOOL CGDCGeo::BitBlt(double lon, double lat, double clon, double clat,
					 CGDCGeo* pSrcDCGeo,
					 double lonSrc, double latSrc, DWORD dwRop)
{
	CGPointGP gpPoint(lon, lat);
	CGSizeGP gpSize(clon, clat);

	CGPointGP gpSrcPoint(lonSrc, latSrc);

	return this->BitBlt(&gpPoint, &gpSize, pSrcDCGeo, &gpSrcPoint, dwRop) ;

}

BOOL CGDCGeo::BitBlt(CGPointGeo *pLeftTop, CGSizeGeo *pSizeGeo,
					 CGDCGeo* pSrcDCGeo,
					 CGPointGeo *pSrcLeftTop, DWORD dwRop)
{
	ASSERT(pLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	CPoint ptPoint;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptPoint, pLeftTop);

//	ptSize = this->GeoPSZtoLPSZ(pLeftTop, pSizeGeo);
	CSize  ptSize;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptSize, pSizeGeo);

	CPoint ptSrcPoint;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	pSrcDCGeo->GeoPtoLP(&ptSrcPoint, pSrcLeftTop);

	return CDC::BitBlt(ptPoint.x, ptPoint.y, ptSize.cx, ptSize.cy,
					   (CDC*) pSrcDCGeo,
					   ptSrcPoint.x, ptSrcPoint.y, dwRop);
}

BOOL CGDCGeo::StretchBlt(double lon, double lat, double clon, double clat,
						 CGDCGeo* pSrcDCGeo,
						 double lonSrc, double latSrc,
						 double clonSrc, double clatSrc, DWORD dwRop)
{
	CGPointGP gpPoint(lon, lat);
	CGSizeGP  gpSize(clon, clat);
	CPoint	ptPoint;
	CSize	ptSize;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptPoint, &gpPoint);
	ptSize = this->GPSZtoLPSZ(gpPoint, gpSize);
	
	CGPointGP gpSrcPoint(lonSrc, latSrc);
	CGSizeGP  gpSrcSize(clonSrc, clatSrc);
	CPoint	ptSrcPoint;
	CSize	ptSrcSize;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	pSrcDCGeo->GeoPtoLP(&ptSrcPoint, &gpSrcPoint);
	ptSrcSize = pSrcDCGeo->GPSZtoLPSZ(gpSrcPoint, gpSrcSize);

	return CDC::StretchBlt(ptPoint.x, ptPoint.y, ptSize.cx, ptSize.cy,
						   (CDC*) pSrcDCGeo,
						   ptSrcPoint.x, ptSrcPoint.y, ptSrcSize.cx, ptSrcSize.cy,
						   dwRop);

}

BOOL CGDCGeo::StretchBlt(CGPointGeo *pLeftTop, CGSizeGeo *pSizeGeo,
						 CGDCGeo* pSrcDCGeo,
						 CGPointGeo *pSrcLeftTop, CGSizeGeo *pSrcSizeGeo,
						 DWORD dwRop)
{
	ASSERT(pLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	CGPointGP *pointGP = (CGPointGP*) pLeftTop;
	CGSizeGP  *sizeGP  = (CGSizeGP* ) pSizeGeo;

	CGPointGP *pointSrcGP = (CGPointGP*) pSrcLeftTop;
	CGSizeGP  *sizeSrcGP  = (CGSizeGP* ) pSrcSizeGeo;

	return this->StretchBlt(pointGP->m_P.lon, pointGP->m_P.lat,
							sizeGP->m_S.clon, sizeGP->m_S.clat,
							pSrcDCGeo,
							pointSrcGP->m_P.lon, pointSrcGP->m_P.lat,
							sizeSrcGP->m_S.clon, sizeSrcGP->m_S.clat, dwRop);
}

COLORREF CGDCGeo::GetPixel(CGPointGeo *pPoint)
{
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, pPoint);
	
	return CDC::GetPixel(point);
}

COLORREF CGDCGeo::SetPixel(CGPointGeo *pPoint, COLORREF crColor)
{
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, pPoint);

	return CDC::SetPixel(point, crColor);
}

BOOL CGDCGeo::SetPixelV(CGPointGeo *pPoint, COLORREF crColor)
{
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, pPoint);

	return CDC::SetPixelV(point, crColor);
}

BOOL CGDCGeo::FloodFill(CGPointGeo *pPoint, COLORREF crColor)
{
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&point, pPoint);

	return CDC::FloodFill(point.x, point.y, crColor);
}

BOOL CGDCGeo::ExtFloodFill(CGPointGeo *pPoint, COLORREF crColor, UINT nFillType)
{
	CPoint	point;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&point, pPoint);

	return CDC::ExtFloodFill(point.x, point.y, crColor, nFillType);
}

BOOL CGDCGeo::MaskBlt(double lon, double lat, double clon, double clat,
					  CGDCGeo *pStrDCGeo, double lonSrc, double latSrc,
					  CBitmap &maskBitmap, int xMask, int yMask, DWORD dwRop)
{
	CGPointGP gpLeftTop(lon, lat);
	CGSizeGP  gpSize(clon, clat);
	CGPointGP gpSrcLeftTop(lonSrc, latSrc);

	return this->MaskBlt(&gpLeftTop, &gpSize, pStrDCGeo, &gpSrcLeftTop,
						 maskBitmap, xMask, yMask, dwRop);
}

BOOL CGDCGeo::MaskBlt(CGPointGeo *pLeftTop, CGSizeGeo *pSizeGeo,
					  CGDCGeo *pStrDCGeo, CGPointGeo *pSrcLeftTop,
					  CBitmap &maskBitmap, int xMask, int yMask, DWORD dwRop)
{
	ASSERT(pLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	CPoint ptPoint;
	CSize  ptSize;
	CPoint ptSrcPoint;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptPoint, pLeftTop);
	ptSize = this->GeoPSZtoLPSZ(pLeftTop, pSizeGeo);
	
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	pStrDCGeo->GeoPtoLP(&ptSrcPoint, pSrcLeftTop);

	return CDC::MaskBlt(ptPoint.x, ptPoint.y, ptSize.cx, ptSize.cy,
						(CDC*) pStrDCGeo,
						ptSrcPoint.x, ptSrcPoint.y,
						maskBitmap, xMask, yMask, dwRop);
}

BOOL CGDCGeo::PlgBlt(CGPointGeo *pPoints,
					 CGDCGeo *pSrcDCGeo,
					 double lonSrc, double latSrc, double clonSrc, double clatSrc,
					 CBitmap& maskBitmap, int xMask, int yMask)
{
	CGPointGP gpSrcPoint(lonSrc, latSrc);
	CGSizeGP  gpSrcSize(clonSrc, clatSrc);

	return this->PlgBlt(pPoints, pSrcDCGeo, &gpSrcPoint, &gpSrcSize, maskBitmap, xMask, yMask);
}

BOOL CGDCGeo::PlgBlt(CGPointGeo *pPoints, CGDCGeo *pSrcDCGeo,
				     CGPointGeo *pSrcLeftTop, CGSizeGeo *pSrcSizeGeo,
				     CBitmap& maskBitmap, int xMask, int yMask)
{
	ASSERT(pPoints->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoints->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcLeftTop->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcLeftTop->m_nPointType == GPTYPE_GP)) return FALSE;

	ASSERT(pSrcSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcSizeGeo->m_nPointType == GPTYPE_GP)) return FALSE;

	CPoint ptPoint[3];
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(ptPoint, pPoints, 3);
	
	CPoint ptSrcPoint;
	CSize  ptSrcSize;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	pSrcDCGeo->GeoPtoLP(&ptSrcPoint, pSrcLeftTop);
	ptSrcSize = pSrcDCGeo->GeoPSZtoLPSZ(pSrcLeftTop, pSrcSizeGeo);

	return CDC::PlgBlt(ptPoint,
				pSrcDCGeo,
				ptSrcPoint.x, ptSrcPoint.y, ptSrcSize.cx, ptSrcSize.cy,
				maskBitmap, xMask, yMask);
}

int CGDCGeo::StretchDIBits(double lonDest, double latDest,
						   double clonDest, double clatDest,
						   double lonSrc, double latSrc,
						   double clonSrc, double clatSrc,
						   CONST VOID *lpBits,		// address of bitmap bits 
						   CONST BITMAPINFO *lpBitsInfo,// address of bitmap data 
						   UINT iUsage,			// usage 
						   DWORD dwRop)			// raster operation code
{
	CGPointGP gpDest(lonDest, latDest);
	CGSizeGP  gpDestSize(clonDest, clatDest);
	CGPointGP gpSrc(lonSrc, latSrc);
	CGSizeGP  gpSrcSize(clonSrc, clatSrc);

	return this->StretchDIBits(&gpDest, &gpDestSize, &gpSrc, &gpSrcSize,
							   lpBits, lpBitsInfo, iUsage, dwRop);
}

int CGDCGeo::StretchDIBits(CGPointGeo *pDestPointGeo, CGSizeGeo *pDestSizeGeo,
						   CGPointGeo *pSrcPointGeo, CGSizeGeo *pSrcSizeGeo,
						   CONST VOID *lpBits,		// address of bitmap bits 
						   CONST BITMAPINFO *lpBitsInfo,// address of bitmap data 
						   UINT iUsage,			// usage 
						   DWORD dwRop)			// raster operation code
{
	ASSERT(pDestPointGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDestPointGeo->m_nPointType == GPTYPE_GP)) return -1;

	ASSERT(pDestSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDestSizeGeo->m_nPointType == GPTYPE_GP)) return -1;

	ASSERT(pSrcPointGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcPointGeo->m_nPointType == GPTYPE_GP)) return -1;

	ASSERT(pSrcSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcSizeGeo->m_nPointType == GPTYPE_GP)) return -1;

	CPoint	ptDest;
	CSize	ptDestSize;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptDest, pDestPointGeo);
	ptDestSize = this->GeoPSZtoLPSZ(pDestPointGeo, pDestSizeGeo);

	CPoint	ptSrc;
	CSize	ptSrcSize;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&ptSrc, pSrcPointGeo);
	ptSrcSize = this->GeoPSZtoLPSZ(pSrcPointGeo, pSrcSizeGeo);

	return ::StretchDIBits(m_hDC, ptDest.x, ptDest.y, ptDestSize.cx, ptDestSize.cy,
						   ptSrc.x, ptSrc.y, ptSrcSize.cx, ptSrcSize.cy,
						   lpBits, lpBitsInfo, iUsage, dwRop);
}

int CGDCGeo::StretchDIBits(CGPointGeo *pDestPointGeo, CGSizeGeo *pDestSizeGeo,
						   int xSrc, int ySrc, int cxSrc, int cySrc,
						   CONST VOID *lpBits,				// address of bitmap bits 
						   CONST BITMAPINFO *lpBitsInfo,	// address of bitmap data 
						   UINT iUsage,						// usage 
						   DWORD dwRop)						// raster operation code
{
	ASSERT(pDestPointGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDestPointGeo->m_nPointType == GPTYPE_GP)) return -1;

	ASSERT(pDestSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDestSizeGeo->m_nPointType == GPTYPE_GP)) return -1;

	CPoint LpDestPoint;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&LpDestPoint, pDestPointGeo);
	CSize  LpDestSize = this->GeoPSZtoLPSZ(pDestPointGeo, pDestSizeGeo);

	return ::StretchDIBits(this->m_hDC,
						   LpDestPoint.x, LpDestPoint.y,
						   LpDestSize.cx, LpDestSize.cy,
						   xSrc, ySrc, cxSrc, cySrc,
						   lpBits, lpBitsInfo, iUsage, dwRop);
}

/////////////////////////////////////////////////////////////////////////////
// Text Functions
BOOL CGDCGeo::TextOut(CGPointGeo *pPoint, const CString& str)
{
	CPoint	ptPoint;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptPoint, pPoint);

	return CDC::TextOut(ptPoint.x, ptPoint.y, str);
}

BOOL CGDCGeo::TextOut(CPoint *pPoint, const CString& str)
{
//	CPoint	ptPoint;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
//	GeoPtoLP(&ptPoint, pPoint);
	return CDC::TextOut(pPoint->x, pPoint->y, str);
}


BOOL CGDCGeo::ExtTextOut(CGPointGeo *pPoint, UINT nOptions, CGRectGeo *pRect,
					const CString& str, LPINT lpDxWidths)
{
	CPoint	ptPoint;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptPoint, pPoint);

	if (pRect!=NULL) {
		CRect	rect;

		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&rect, pRect);
		return CDC::ExtTextOut(ptPoint.x, ptPoint.y, nOptions, &rect,
					str, lpDxWidths);
	}
	else {
		return CDC::ExtTextOut(ptPoint.x, ptPoint.y, nOptions, NULL,
					str, lpDxWidths);
	}
}

BOOL CGDCGeo::ExtTextOut(CGPointGeo *pPoint, UINT nOptions, CGRectGeo *pRect,
				LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths)
{
	CPoint	ptPoint;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&ptPoint, pPoint);

	if (pRect!=NULL) {
		CRect	rect;

		// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
		GeoPtoLP(&rect, pRect);
		return CDC::ExtTextOut(ptPoint.x, ptPoint.y, nOptions, &rect,
					lpszString, nCount, lpDxWidths);
	}
	else {
		return CDC::ExtTextOut(ptPoint.x, ptPoint.y, nOptions, NULL,
					lpszString, nCount, lpDxWidths);
	}
}



void CGDCGeo::TabbedTextOut(CGPointGeo *pPoint, LPCTSTR lpszString, int nCount,
			int nTabPositions, LPINT lpnTabStopPosition, int nTabOrigin,
			CGSizeGeo *pSize)
{
	CPoint	pt;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&pt, pPoint);

	CSize szRet = CDC::TabbedTextOut(pt.x, pt.y, lpszString, nCount,
				nTabPositions, lpnTabStopPosition, nTabOrigin);

	// 크기를 리턴한다.
	if(pSize!=NULL)
	{
		// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
		LPtoGeoP(pSize, &szRet);
	}

	return;
}

int CGDCGeo::DrawText(const CString& str, CGRectGeo *pRect, UINT nFormat)
{
	CRect	rect;
	
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	return CDC::DrawText(str, &rect, nFormat);
}

int CGDCGeo::DrawText(LPCTSTR lpszString, int nCount, CGRectGeo *pRect, UINT nFormat)
{
	CRect	rect;
	
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	GeoPtoLP(&rect, pRect);

	return CDC::DrawText(lpszString, nCount, &rect, nFormat);
}

void CGDCGeo::GetTextExtent(LPCTSTR lpszString, int nCount, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetTextExtent(lpszString, nCount);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetTextExtent(const CString& str, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetTextExtent(str);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetOutputTextExtent(LPCTSTR lpszString, int nCount, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetOutputTextExtent(lpszString, nCount);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetOutputTextExtent( const CString& str, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetOutputTextExtent(str);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetOutputTabbedTextExtent(const CString &str,
					int nTabPositions, LPINT lpnTabStopPositions,
					CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetOutputTabbedTextExtent(str, nTabPositions, lpnTabStopPositions);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount,
					int nTabPositions, LPINT lpnTabStopPositions,
					CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetOutputTabbedTextExtent(lpszString, nCount, nTabPositions,
							lpnTabStopPositions);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetTabbedTextExtent(const CString& str, int nTabPositions,
				  LPINT lpnTabStopPositions, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetTabbedTextExtent(str, nTabPositions, lpnTabStopPositions);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

void CGDCGeo::GetTabbedTextExtent(LPCTSTR lpszString, int nCount, int nTabPositions,
				  LPINT lpnTabStopPositions, CGSizeGeo *pSize)
{
	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	CSize	size;

	size = CDC::GetTabbedTextExtent(lpszString, nCount, nTabPositions, lpnTabStopPositions);
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	LPtoGeoP(pSize, &size);
}

CSize CGDCGeo::GPSZtoLPSZ(CGPointGP gpOrg, CGSizeGP gpSize)
{
	CPoint lpOrg;
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&lpOrg, &gpOrg);

	CPoint szWidthHeight;
	CGPointGP pointGP(gpOrg.m_P.lon + gpSize.m_S.clon, gpOrg.m_P.lat + gpSize.m_S.clat);
	
	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(&szWidthHeight, &pointGP);

	szWidthHeight -= lpOrg;

	return (CSize) szWidthHeight;
}

CGSizeGP CGDCGeo::LPSZtoGPSZ(CPoint lpOrg, CSize lpSize)
{
	CGPointGP gpOrg;
	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&gpOrg, &lpOrg);

	CGPointGP szWidthHeight;
	CPoint point(lpOrg.x + lpSize.cx, lpOrg.y + lpSize.cy);

	// 타원체가 Bessel인 경우 내부에서 WGS 84로 변환한다.
	this->LPtoGeoP(&szWidthHeight, &point);

	szWidthHeight.m_P.lon -= gpOrg.m_P.lon;
	szWidthHeight.m_P.lat -= gpOrg.m_P.lat;

	CGSizeGP sizeGP(szWidthHeight.m_P.lon, szWidthHeight.m_P.lat);

	return sizeGP;
}

CSize CGDCGeo::GeoPSZtoLPSZ(CGPointGeo *pPointGeoOrg, CGSizeGeo *pSizeGeo)
{
	ASSERT(pPointGeoOrg->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPointGeoOrg->m_nPointType == GPTYPE_GP)) return (CSize) CSize(0,0);

	ASSERT(pSizeGeo->m_nPointType == GPTYPE_GP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSizeGeo->m_nPointType == GPTYPE_GP)) return (CSize) CSize(0,0);

	CGPointGP pointGPOrg;
	CGSizeGP  sizeGP;
	pointGPOrg.SetPoint(pPointGeoOrg);
	sizeGP.SetSize(pSizeGeo);

	return this->GPSZtoLPSZ(pointGPOrg, sizeGP);
}

CGSizeGP CGDCGeo::LPSZtoGeoPSZ(CPoint *pLPOrg, CSize *pLPSize)
{
	CPoint LPOrg(pLPOrg->x, pLPOrg->y);
	CSize  LPSize(pLPSize->cx, pLPSize->cy);

	return this->LPSZtoGPSZ(LPOrg, LPSize);
}
	
int CGDCGeo::GetPath(CGPointGeo *pGeoPoints, LPBYTE lpTypes, int nCount)
{
	if (!pGeoPoints || nCount <= 0) return -1;
	if (pGeoPoints->m_nPointType != GPTYPE_GP) return -1;

	CPoint *lpPoints = new CPoint[nCount];
	if (!(lpPoints))	// 메모리가 할당되지 않음
		return FALSE;

	// 타원체가 Bessel인 경우 내부에서 Bessel로 변환한다.
	this->GeoPtoLP(lpPoints, pGeoPoints, nCount);

	return CDC::GetPath(lpPoints, lpTypes, nCount);
}
	
	/*
BOOL CGDCGeo::GrayString(CBrush* pBrush,
			 BOOL ( CALLBACK EXPORT* lpfnOutput )( HDC, LPARAM, int ),
			 LPARAM lpData, int nCount, CGPointGeo *pPoint,
			 CGSizeGeo *pSize)
{
	SetMapMode(MM_TEXT);

	SetMapMode(MM_ISOTROPIC);

	return TRUE;
}
*/


/////////////////////////////////////////////////////////////////////////////
// Helper of CGGeoDC

#ifdef _DEBUG
void CGClientDCGeo::AssertValid() const
{
	CGDCGeo::AssertValid();
	ASSERT(m_hWnd == NULL || ::IsWindow(m_hWnd));
}

void CGClientDCGeo::Dump(CDumpContext& dc) const
{
	CGDCGeo::Dump(dc);

	dc << "m_hWnd = " << m_hWnd;
	dc << "\n";
}
#endif

CGClientDCGeo::CGClientDCGeo(CWnd* pWnd)// : CGDCGeo(pWnd->GetDC())
{
	ASSERT(pWnd == NULL || ::IsWindow(pWnd->m_hWnd));

	if (!Attach(::GetDC(m_hWnd = pWnd->GetSafeHwnd())))
		AfxThrowResourceException();
}

CGClientDCGeo::~CGClientDCGeo()
{
	//ASSERT(m_hDC != NULL);
	// 2005/12/12 No1 Test.... Detach() -> m_hDC
	TRACE0("CGClientDCGeo::~CGClientDCGeo() ReleaeDC() 합니다.\n");
	::ReleaseDC(m_hWnd, m_hDC);
}

/////////////////////////////////////////////////////////////////////////////
// CGWindowDCGeo class
#ifdef _DEBUG
void CGWindowDCGeo::AssertValid() const
{
	CGDCGeo::AssertValid();
	ASSERT(m_hWnd == NULL || ::IsWindow(m_hWnd));
}

void CGWindowDCGeo::Dump(CDumpContext& dc) const
{
	CGDCGeo::Dump(dc);

	dc << "m_hWnd = " << m_hWnd;
	dc << "\n";
}
#endif

CGWindowDCGeo::CGWindowDCGeo(CWnd* pWnd)// : CGDCGeo(pWnd->GetDC())
{
	ASSERT(pWnd == NULL || ::IsWindow(pWnd->m_hWnd));

	if (!Attach(::GetWindowDC(m_hWnd = pWnd->GetSafeHwnd())))
		AfxThrowResourceException();
}

CGWindowDCGeo::~CGWindowDCGeo()
{
	ASSERT(m_hDC != NULL);
	::ReleaseDC(m_hWnd, Detach());
}

/////////////////////////////////////////////////////////////////////////////
// CGPaintDCGeo class
#ifdef _DEBUG
void CGPaintDCGeo::AssertValid() const
{
	CGDCGeo::AssertValid();
	ASSERT(::IsWindow(m_hWnd));
}

void CGPaintDCGeo::Dump(CDumpContext& dc) const
{
	CGDCGeo::Dump(dc);

	dc << "m_hWnd = " << m_hWnd;
	dc << "\nm_ps.hdc = " << m_ps.hdc;
	dc << "\nm_ps.fErase = " << m_ps.fErase;
	dc << "\nm_ps.rcPaint = " << (CRect)m_ps.rcPaint;

	dc << "\n";
}
#endif

CGPaintDCGeo::CGPaintDCGeo(CWnd* pWnd) // : CGDCGeo(pWnd->GetDC())
{
	ASSERT_VALID(pWnd);
	ASSERT(::IsWindow(pWnd->m_hWnd));

	if (!Attach(::BeginPaint(m_hWnd = pWnd->m_hWnd, &m_ps)))
		AfxThrowResourceException();
}

CGPaintDCGeo::CGPaintDCGeo(HWND hWnd, HDC hDC)
{
	ASSERT(::IsWindow(hWnd));

	m_hWnd = hWnd;
	if (hDC==NULL) {
		hDC = ::BeginPaint(hWnd, &m_ps);
	}

	if (!Attach(hDC)) {
		AfxThrowResourceException();
	}
}

CGPaintDCGeo::~CGPaintDCGeo()
{
	//by Peter 2005-11-29 아래의 2줄을 제거합니다.
	//ASSERT(m_hDC != NULL);
	//ASSERT(::IsWindow(m_hWnd));
	
	try {
		::EndPaint(m_hWnd, &m_ps);
//		Detach(); 
	} catch(...) {
	}
	TRACE0("CGPaintDCGeo::~CGPaintDCGeo()\n");
}


int CGDCGeo::SetMapScale(int nMapScale)
{
	int prevMapScale = NULL;
	if (m_pMapScale == NULL)
	{
		m_pMapScale = new (int);
	}
	else prevMapScale = *m_pMapScale;

	*m_pMapScale = nMapScale;

	return prevMapScale;
}

int CGDCGeo::GetMapScale() const
{
	if (m_pMapScale == NULL) return NULL;
	return *m_pMapScale;
}

int CGDCGeo::GetEllipsoid() const
{
	if (m_pGeoObj == NULL) return NULL;
	return m_pGeoObj->GetEllipsoid();
}

double CGDCGeo::SetZoomRatio(double dZoom)
{
	double prevZoomRatio = NULL;
	if (m_pZoomRatio == NULL)
	{
		m_pZoomRatio = new (double);
	}
	else
		prevZoomRatio = *m_pZoomRatio;

	*m_pZoomRatio = dZoom;

	return prevZoomRatio;
}

double CGDCGeo::GetZoomRatio() const
{
	if (m_pZoomRatio == NULL) return NULL;
	return *m_pZoomRatio;
}

void CGDCGeo::SetRealScaleGeoDC()
{
	CDC::SetMapMode(MM_ISOTROPIC);

	if (m_pZoomRatio == NULL) return;
	if (m_pMapScale == NULL) return;

	CSize winExt = CDC::GetWindowExt();
	CSize viewExt = CDC::GetViewportExt();

	
	double times, temp;
	temp = (double)(*m_pMapScale) * (*m_pZoomRatio);

	times = temp / (3600);	// 10,000	// MM_ISOTROPIC일 경우
	times /= 1.6;
	
	winExt.cx = (int) ((double)winExt.cx * times);
	winExt.cy = (int) ((double)winExt.cy * times);

	CDC::SetWindowExt(winExt);
	CDC::SetViewportExt(viewExt);
}

CGSizeGP CGDCGeo::GetPixelSizeGP()
{
	CGSizeGP sizeGP;
	CSize pixelSize(1, 1);
	this->DPtoGeoP(&sizeGP, &pixelSize);
	return sizeGP;
}

void CGDCGeo::MemReset()
{
	// m_pGeoObj는 다른 곳에서 생성하는 것이므로 delete 하지 않는다.
	if(m_pGeoObj!=NULL) {/*delete m_pGeoObj;m_pGeoObj = NULL;*/}
	if(m_pWinOrg!=NULL) {delete m_pWinOrg; m_pWinOrg = NULL;}
	if(m_pWinExt!=NULL) {delete m_pWinExt; m_pWinExt = NULL;}
	if (m_pMapScale != NULL) {delete m_pMapScale; m_pMapScale = NULL;}
	if (m_pZoomRatio != NULL) {delete m_pZoomRatio; m_pZoomRatio = NULL;}
}
// GRgn.cpp: implementation of the CGRgn class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "GDCGeo.h"
#include "GRgn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGRgn::CGRgn(CGDCGeo *pDCGeo)
{
	this->m_pDCGeo = pDCGeo;
	
	this->m_nRgnType = GRGN_NULL;	// CGRgn Type

	this->m_pRgn = NULL;

	this->m_pGPs = NULL;
	this->m_nCount = 0;
	this->m_nMode = 0;

	if (pDCGeo)
		this->SaveDC();
}

CGRgn::~CGRgn()
{
	if (m_pGPs) delete[] m_pGPs;
	if (m_pRgn) 
	{
		m_pRgn->DeleteObject();
		delete m_pRgn;
	}
}

void CGRgn::SetDC(CGDCGeo *pDCGeo)
{
	this->m_pDCGeo = pDCGeo;
	
	this->m_nRgnType = GRGN_NULL;	// CGRgn Type

	this->m_pRgn = NULL;

	this->m_pGPs = NULL;
	this->m_nCount = 0;
	this->m_nMode = 0;

	if (pDCGeo)
		this->SaveDC();
}

void CGRgn::SaveDC()
{
	m_MapMode = m_pDCGeo->CDC::GetMapMode();
	
	m_WinOrg = m_pDCGeo->CDC::GetWindowOrg();
	m_ViewOrg = m_pDCGeo->CDC::GetViewportOrg();
	
	m_WinExt = m_pDCGeo->CDC::GetWindowExt();
	m_ViewExt = m_pDCGeo->CDC::GetViewportExt();
}

void CGRgn::RestoreDC()
{
	m_pDCGeo->CDC::SetMapMode(m_MapMode);
	
	m_pDCGeo->CDC::SetWindowOrg(m_WinOrg);
	m_pDCGeo->CDC::SetViewportOrg(m_ViewOrg);

	m_pDCGeo->CDC::SetWindowExt(m_WinExt);
	m_pDCGeo->CDC::SetViewportExt(m_ViewExt);
}

void CGRgn::SetGRgnDC()
{
	CPoint	dpWinOrg = m_WinOrg;

	CDC *pDC = (CDC*) m_pDCGeo;
	
	pDC->LPtoDP(&dpWinOrg);

	pDC->SetMapMode(MM_TEXT);
	pDC->SetWindowOrg(dpWinOrg);
	pDC->SetViewportOrg(m_ViewOrg);
}

BOOL CGRgn::CreateRectRgn(double lon1, double lat1, double lon2, double lat2)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	m_pGPs[0].SetPoint(lon1, lat1);
	m_pGPs[1].SetPoint(lon2, lat2);

	this->m_nRgnType = GRGN_RECT;
	this->m_nCount = 2;

	return Create();
}

BOOL CGRgn::CreateRectRgnIndirect(CGRectGP *pRectGP)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top);
	m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.bottom);

	this->m_nRgnType = GRGN_RECT_INDIRECT;
	this->m_nCount = 2;

	return Create();
}

BOOL CGRgn::CreateEllipticRgn(double lon1, double lat1, double lon2, double lat2)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	m_pGPs[0].SetPoint(lon1, lat1);
	m_pGPs[1].SetPoint(lon2, lat2);

	this->m_nRgnType = GRGN_ELLIPTIC;
	this->m_nCount = 2;

	return Create();
}

BOOL CGRgn::CreateEllipticRgnIndirect(CGRectGP *pRectGP)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top);
	m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.bottom);

	this->m_nRgnType = GRGN_ELLIPTIC_INDIRECT;
	this->m_nCount = 2;

	return Create();
}

BOOL CGRgn::CreatePolygonRgn(CGPointGP *pGPs, int nCount, int nMode)
{
	this->m_pGPs = new CGPointGP[nCount];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	for (int i = 0; i < nCount; i++)
		m_pGPs[i].SetPoint(&pGPs[i]);

	this->m_nRgnType = GRGN_POLYGON;
	this->m_nCount = nCount;
	this->m_nMode = nMode;

	return Create();
}
/*
BOOL CGRgn::CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode)
{
	this->m_pGPs = new CGPointGP[nCount];
	
	for (int i = 0; i < nCount; i++)
		m_pGPs[i].SetPoint(pGPs);

	this->m_nRgnType = GRGN_POLYPOLYGON;
	this->m_nCount = nCount;
	this->m_nMode = nMode;

	return Create();
}
*/
BOOL CGRgn::CreateRoundRectRgn(double lon1, double lat1, double lon2, double lat2,  double clon, double clat)
{
	this->m_pGPs = new CGPointGP[3];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;
	
	m_pGPs[0].SetPoint(lon1, lat1);
	m_pGPs[1].SetPoint(lon2, lat2);
	m_pGPs[2].SetPoint(clon, clat);

	this->m_nRgnType = GRGN_ROUND_RECT;
	this->m_nCount = 3;

	return Create();
}

BOOL CGRgn::Create()
{
	BOOL bRetVal = FALSE;
	if (m_pGPs == NULL || m_nCount == 0 || m_nRgnType == GRGN_NULL || m_pDCGeo == NULL)
		return bRetVal;

	SaveDC();	// 현재 DC의 내용을 저장

	CPoint *pDPs = new CPoint[m_nCount];	// DP 생성
	if (!(pDPs))	// 메모리가 할당되지 않음
		return FALSE;

	m_pDCGeo->GeoPtoDP(pDPs, m_pGPs, m_nCount);	// 지리좌표를 장치좌표로 변환
	if (this->m_nRgnType == GRGN_ROUND_RECT)
	{
		CGSizeGP sizeGP(m_pGPs[2].m_P.lon, m_pGPs[2].m_P.lat);
		CSize sizeDP;
		m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP);
		pDPs[2] = sizeDP;
	}

	SetGRgnDC();	// DC 설정

	if (m_pRgn)
	{
		m_pRgn->DeleteObject();
		delete m_pRgn;
		m_pRgn = NULL;
	}

	m_pRgn = new CRgn;	// 영역을 생성
	if (!(m_pRgn))	// 메모리가 할당되지 않음
		return FALSE;

	switch(this->m_nRgnType)
	{
	case GRGN_RECT :				// CreateRectRgn()
		bRetVal = m_pRgn->CreateRectRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
		break;
	case GRGN_RECT_INDIRECT :		// CreateRectRgnIndirect()
		{
			CRect rect(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
			bRetVal = m_pRgn->CreateRectRgnIndirect(&rect);
		}
		break;
	case GRGN_ELLIPTIC :			// CreateEllipticRgn()
		bRetVal = m_pRgn->CreateEllipticRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
		break;
	case GRGN_ELLIPTIC_INDIRECT :	// CreateEllipticRgnIndirect()
		{
			CRect rect(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
			bRetVal = m_pRgn->CreateEllipticRgnIndirect(&rect);
		}
		break;
	case GRGN_POLYGON :
		bRetVal = m_pRgn->CreatePolygonRgn(pDPs, m_nCount, m_nMode);
		break;
	case GRGN_POLYPOLYGON :
		break;
	case GRGN_ROUND_RECT :
		bRetVal = m_pRgn->CreateRoundRectRgn(pDPs[0].x, pDPs[0].y,
			pDPs[1].x, pDPs[1].y, pDPs[2].x, pDPs[2].y);
		break;
	case GRGN_SET_RECT :
		m_pRgn->SetRectRgn(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
		bRetVal = TRUE;
		break;
	case GRGN_SET_RECT_RECT :
		{
			CRect rect(pDPs[0].x, pDPs[0].y, pDPs[1].x, pDPs[1].y);
			m_pRgn->SetRectRgn(&rect);
			bRetVal = TRUE;
		}
		break;
	}
	
	if (pDPs) delete[] pDPs;

	RestoreDC();	// 기존의 DC 를 복원

	return bRetVal;
}

void CGRgn::SetRectRgn(double lon1, double lat1, double lon2, double lat2)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return;
	
	m_pGPs[0].SetPoint(lon1, lat1);
	m_pGPs[1].SetPoint(lon2, lat2);

	this->m_nRgnType = GRGN_SET_RECT;
	this->m_nCount = 2;

	Create();
}

void CGRgn::SetRectRgn(CGRectGP *pRectGP)
{
	this->m_pGPs = new CGPointGP[2];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return;
	
	m_pGPs[0].SetPoint(pRectGP->m_R.left, pRectGP->m_R.top);
	m_pGPs[1].SetPoint(pRectGP->m_R.right, pRectGP->m_R.right);

	this->m_nRgnType = GRGN_SET_RECT_RECT;
	this->m_nCount = 2;

	Create();
}

int CGRgn::CombineRgn(CGRgn* pRgn1, CGRgn* pRgn2, int nCombineMode)
{
	int nRetVal = ERROR;
	
	if (pRgn1->m_pDCGeo == pRgn2->m_pDCGeo)
	{
		this->m_pDCGeo = pRgn1->m_pDCGeo;

		// Device Context
		this->m_MapMode = pRgn1->m_MapMode;

		this->m_WinOrg = pRgn1->m_WinOrg;
		this->m_WinExt = pRgn1->m_WinExt;

		this->m_ViewOrg = pRgn1->m_ViewOrg;
		this->m_ViewExt = pRgn1->m_ViewExt;

		this->m_pRgn = new CRgn;
		if (!(this->m_pRgn))	// 메모리가 할당되지 않음
			return -1;

		nRetVal = this->m_pRgn->CombineRgn(pRgn1->m_pRgn, pRgn2->m_pRgn, nCombineMode);
	}
	return nRetVal;
}

int CGRgn::CopyRgn(CGRgn* pRgnSrc)
{
	this->m_pDCGeo = pRgnSrc->m_pDCGeo;

	// Device Context
	this->m_MapMode = pRgnSrc->m_MapMode;

	this->m_WinOrg = pRgnSrc->m_WinOrg;
	this->m_WinExt = pRgnSrc->m_WinExt;

	this->m_ViewOrg = pRgnSrc->m_ViewOrg;
	this->m_ViewExt = pRgnSrc->m_ViewExt;

	// CGRgn type
	this->m_nRgnType = pRgnSrc->m_nRgnType;

	// 좌표값을 유지
	this->m_nCount = pRgnSrc->m_nCount;	// 갯수
	this->m_nMode = pRgnSrc->m_nMode;	// CreatePolygon 함수만 적용
	
	this->m_pGPs = new CGPointGP[this->m_nCount];
	if (!(this->m_pGPs))	// 메모리가 할당되지 않음
		return FALSE;

	for (int i = 0; i < this->m_nCount; i++)
		this->m_pGPs[i].SetPoint(&pRgnSrc->m_pGPs[i]);
	Create();

	return this->m_pRgn->CopyRgn(pRgnSrc->m_pRgn);
}

BOOL CGRgn::EqualRgn(CGRgn* pRgn)
{
	BOOL bRetVal = FALSE;

	if (this->m_pRgn != NULL)
		if (this->m_pDCGeo == pRgn->m_pDCGeo)
			bRetVal = this->m_pRgn->EqualRgn(pRgn->m_pRgn);

	return bRetVal;
}

int CGRgn::OffsetRgn(double lon, double lat)
{
	int nRetVal = ERROR;

	if (this->m_pDCGeo && this->m_pGPs)
	{
		for (int i = 0; i < this->m_nCount; i++)
			this->m_pGPs[i].Offset(lon, lat);

		CGSizeGP sizeGP(lon, lat);
		CSize sizeDP;
		this->m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP);

		nRetVal = this->m_pRgn->OffsetRgn(sizeDP.cx, sizeDP.cy);
		Create();
	}
	return nRetVal;
}


int CGRgn::OffsetRgn(CGPointGP gp)
{
	int nRetVal = ERROR;

	if (this->m_pDCGeo && this->m_pGPs)
	{
		for (int i = 0; i < this->m_nCount; i++)
			this->m_pGPs[i].Offset(gp.m_P.lon, gp.m_P.lat);

		CGSizeGP sizeGP(gp.m_P.lon, gp.m_P.lat);
		CSize sizeDP;
		this->m_pDCGeo->GeoPtoDP(&sizeDP, &sizeGP);

		nRetVal = this->m_pRgn->OffsetRgn(sizeDP.cx, sizeDP.cy);
		Create();
	}
	return nRetVal;
}

int CGRgn::GetRgnBox(CGRectGP *pRectGP)
{
	int nRetVal = ERROR;

	if (this->m_pRgn != NULL)
	{
		CRect rect;
		nRetVal = this->m_pRgn->GetRgnBox(&rect);
		this->m_pDCGeo->DPtoGeoP(pRectGP, rect);
	}
	return nRetVal;
}

BOOL CGRgn::PtInRegion(double lon, double lat) const
{
	BOOL bRetVal = FALSE;

	if (this->m_pDCGeo && this->m_pRgn)
	{
		CGPointGP GP(lon, lat);
		CPoint DP;
		this->m_pDCGeo->GeoPtoDP(&DP, &GP, 1);
		bRetVal = this->m_pRgn->PtInRegion(DP.x, DP.y);
	}
	return bRetVal;
}

BOOL CGRgn::PtInRegion(CGPointGP gp) const
{
	BOOL bRetVal = FALSE;

	if (this->m_pDCGeo && this->m_pRgn)
	{
		CPoint DP;
		this->m_pDCGeo->GeoPtoDP(&DP, &gp, 1);
		bRetVal = this->m_pRgn->PtInRegion(DP);
	}
	return bRetVal;
}

BOOL CGRgn::RectInRegion(CGRectGP *pRectGP) const
{
	BOOL bRetVal = FALSE;

	if (this->m_pDCGeo && this->m_pRgn)
	{
		CRect rectDP;
		this->m_pDCGeo->GeoPtoDP(&rectDP, pRectGP);
		bRetVal = this->m_pRgn->RectInRegion(rectDP);
	}
	return bRetVal;
}

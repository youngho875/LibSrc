// GGeoObjUTM.cpp : implementation file
//

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoUTM.h"

#include "GGeoProj.h"

#include "GGeoObject.h"
#include "GGeoObjUTM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGeoObjUTM

CGGeoObjUTM::CGGeoObjUTM() : CGGeoObject(GPTYPE_UTM)
{
}

CGGeoObjUTM::~CGGeoObjUTM()
{
}

//
// Projection operators
void CGGeoObjUTM::Forward(LPPOINT lpPoint, CGPointGeo *pPoint, int nPoints)
{
	ASSERT(lpPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPoint)) return;

	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	ASSERT(nPoints>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nPoints>0)) return;

	CGPointUTM	*utmPoint = (CGPointUTM *) pPoint;

	POINTPLANE	*pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;
	int		i;

//	m_GeoProj.Forward(pPlane, utmPoint, nPoints);

	for(i=0; i<nPoints; i++) {
		lpPoint[i].x = (int)(pPlane[i].x / m_Validity);
		lpPoint[i].y = (int)(pPlane[i].y / m_Validity);
	}

	delete[] pPlane;
}

void CGGeoObjUTM::Forward(LPSIZE lpSize, CGSizeGeo *pSize)
{
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	POINTPLANE	plane;
	CGPointUTM	utm;
	CGSizeUTM	*utmSize = (CGSizeUTM *) pSize;

	utm.m_P.x = utmSize->m_S.cx;
	utm.m_P.y = utmSize->m_S.cy;
	utm.m_P.zone = utmSize->m_S.zone;
	utm.m_P.dgn = utmSize->m_S.dgn;

	m_GeoProj.ForwardUTM(&plane, &utm);

	lpSize->cx = (int)(plane.x / m_Validity);
	lpSize->cy = (int)(plane.y / m_Validity);
}

void CGGeoObjUTM::Forward(LPRECT lpRect, CGRectGeo *pRect)
{
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	POINTPLANE	plane[2];
	CGPointUTM	utm[2];
	CGRectUTM	*utmRect = (CGRectUTM *) pRect;

	utm[0].m_P.x = utmRect->m_R.left;
	utm[0].m_P.y = utmRect->m_R.top;
	utm[1].m_P.x = utmRect->m_R.right;
	utm[1].m_P.y = utmRect->m_R.bottom;
	utm[0].m_P.zone = utm[1].m_P.zone = utmRect->m_R.zone;
	utm[0].m_P.dgn = utm[1].m_P.dgn = utmRect->m_R.dgn;

	m_GeoProj.ForwardUTM(plane, utm, 2);

	lpRect->left = (int)(plane[0].x / m_Validity);
	lpRect->top = (int)(plane[0].y / m_Validity);
	lpRect->right = (int)(plane[1].x / m_Validity);
	lpRect->bottom = (int)(plane[1].y / m_Validity);
}

void CGGeoObjUTM::Inverse(CGPointGeo *pPoint, LPPOINT lpPoint, int nPoints)
{
	ASSERT(lpPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpPoint)) return;

	ASSERT(pPoint);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pPoint)) return;

	ASSERT(nPoints>0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(nPoints>0)) return;

	CGPointUTM	*utmPoint = (CGPointUTM *) pPoint;

	POINTPLANE	*pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	int		i;

	for(i=0; i<nPoints; i++) {
		pPlane[i].x = ((double)lpPoint[i].x) * m_Validity;
		pPlane[i].y = ((double)lpPoint[i].y) * m_Validity;
	}

//	m_GeoProj.Inverse(utmPoint, pPlane, nPoints);

	delete[] pPlane;
}


void CGGeoObjUTM::Inverse(CGSizeGeo *pSize, LPSIZE lpSize)
{
	ASSERT(lpSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpSize)) return;

	ASSERT(pSize);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSize)) return;

	POINTPLANE	plane;
	CGPointUTM	utm;
	CGSizeUTM	*utmSize = (CGSizeUTM *) pSize;

	plane.x = ((double)lpSize->cx) * m_Validity;
	plane.y = ((double)lpSize->cy) * m_Validity;

//	m_GeoProj.Inverse(&utm, &plane);

	utmSize->m_S.cx = utm.m_P.x;
	utmSize->m_S.cy = utm.m_P.y;
	utmSize->m_S.zone = utm.m_P.zone;
	utmSize->m_S.dgn = utm.m_P.dgn;
}

void CGGeoObjUTM::Inverse(CGRectGeo *pRect, LPRECT lpRect)
{
	ASSERT(lpRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(lpRect)) return;

	ASSERT(pRect);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pRect)) return;

	POINTPLANE	plane[2];
	CGPointUTM	utm[2];
	CGRectUTM	*utmRect = (CGRectUTM *) pRect;

	plane[0].x = ((double)lpRect->left) * m_Validity;
	plane[0].y = ((double)lpRect->top) * m_Validity;
	plane[1].x = ((double)lpRect->right) * m_Validity;
	plane[1].y = ((double)lpRect->bottom) * m_Validity;

//	m_GeoProj.Inverse(utm, plane, 2);

	utmRect->m_R.left = utm[0].m_P.x;
	utmRect->m_R.top = utm[0].m_P.y;
	utmRect->m_R.right = utm[1].m_P.x;
	utmRect->m_R.bottom = utm[1].m_P.y; 
	utmRect->m_R.zone = utm[0].m_P.zone;
	utmRect->m_R.dgn = utm[0].m_P.dgn;
}


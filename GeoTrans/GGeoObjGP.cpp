// GGeoObjGP.cpp : implementation file
//

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"

#include "GGeoProj.h"

#include "GGeoObject.h"

#include "GGeoObjGP.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGeoObjGP

CGGeoObjGP::CGGeoObjGP() : CGGeoObject(GPTYPE_GP)
{
}

CGGeoObjGP::~CGGeoObjGP()
{
}

CGPointGeo* CGGeoObjGP::Point(int nPoints)
{
	// jjjjj memory leak! jwpark find
	return (CGPointGeo*) ((nPoints == 1) ? new CGPointGP
										 : new CGPointGP[nPoints]);
}
CGSizeGeo* CGGeoObjGP::Size()
{
	// jjjjj memory leak! jwpark find
	return new CGSizeGP;
}

CGRectGeo* CGGeoObjGP::Rect()
{
	return new CGRectGP;
}

//
// Projection operators
void CGGeoObjGP::Forward(LPPOINT lpPoint, CGPointGeo *pPoint, int nPoints)
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

	POINTPLANE	*pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	int		i;

	switch (this->m_nMapMode)
	{
	case GMM_UTM:	// UTM 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.ForwardUTM(pPlane, (CGPointGP*) pPoint, nPoints);
			break;
		case GPTYPE_UTM:
			// UTM 좌표 ==> 평면좌표
			// UTM좌표를 현재 UTM투영의 기준점과 비교하여 같으면 직접 변환하고
			// 틀리면 지리좌표로 변환한 후에 화면좌표로 변환한다.
			m_GeoProj.ForwardUTM(pPlane, (CGPointUTM*) pPoint, nPoints);
			break;
		case GPTYPE_UPS:
			// UPS 좌표 ==> 평면좌표
			// UPS좌표를 지리좌표로 변환한 후에 화면좌표로 변환한다.
//			m_GeoProj.ForwardGP(pPlane, (CGPointUPS*) pPoint, nPoints);
			break;
		}
		break;
	case GMM_UPS:	// UPS 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.ForwardUPS(pPlane, (CGPointGP*) pPoint, nPoints);
			break;
		}
	case GMM_LCC:	// Lambert Conformal Conic Projection
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.ForwardLCC(pPlane, (CGPointGP*) pPoint, nPoints);
			break;
		}
		break;
	case GMM_MER:	// Mercator 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.ForwardMER(pPlane, (CGPointGP*) pPoint, nPoints);
			break;
		case GPTYPE_UTM:
			// UTM 좌표 ==> 평면좌표
			// UTM좌표를 현재 UTM투영의 기준점과 비교하여 같으면 직접 변환하고
			// 틀리면 지리좌표로 변환한 후에 화면좌표로 변환한다.
//			m_GeoProj.ForwardMER(pPlane, (CGPointUTM*) pPoint, nPoints);
			break;
		case GPTYPE_UPS:
			// UPS 좌표 ==> 평면좌표
			// UPS좌표를 지리좌표로 변환한 후에 화면좌표로 변환한다.
//			m_GeoProj.ForwardGP(pPlane, (CGPointUPS*) pPoint, nPoints);
			break;
		}
		break;
	case GMM_GP:	// 투영안함
	default:
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.ForwardGP(pPlane, (CGPointGP*) pPoint, nPoints);
			break;
		case GPTYPE_UTM:
			// UTM 좌표 ==> 평면좌표
			// UTM좌표를 지리좌표로 변환한 후 평면좌표로 변환한다.
			m_GeoProj.ForwardGP(pPlane, (CGPointUTM*) pPoint, nPoints);
			break;
		case GPTYPE_UPS:
			// UPS 좌표 ==> 평면좌표
			// UPS좌표를 지리좌표로 변환한 후 UPS좌표로 변환한다.
//			m_GeoProj.ForwardGP(pPlane, (CGPointUPS*) pPoint, nPoints);
			break;
		}
		break;
	}

	// 평면좌표 ==> 논리좌표
	for(i=0; i<nPoints; i++)
	{
		lpPoint[i].x = (int)(pPlane[i].x / m_Validity);
		lpPoint[i].y = (int)(pPlane[i].y / m_Validity);
	}

	delete[] pPlane;
}

void CGGeoObjGP::Inverse(CGPointGeo *pPoint, LPPOINT lpPoint, int nPoints)
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

//	CGPointGP	*gpPoint = (CGPointGP *) pPoint;

	POINTPLANE	*pPlane = new POINTPLANE[nPoints];
	if (!(pPlane))	// 메모리가 할당되지 않음
		return;

	// 논리좌표(LP) ==> 평면좌표
	int		i;
	for(i=0; i<nPoints; i++) {
		pPlane[i].x = ((double)lpPoint[i].x * m_Validity);
		pPlane[i].y = ((double)lpPoint[i].y * m_Validity);
	}

	switch (this->m_nMapMode)
	{
	case GMM_UTM:	// UTM 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 평면좌표 ==> 지리좌표
			m_GeoProj.InverseUTM((CGPointGP*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UTM:
			// 평면좌표 ==> UTM 좌표
			m_GeoProj.InverseUTM((CGPointUTM*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UPS:
			// 평면좌표 ==> UPS 좌표
//			m_GeoProj.InverseGP((CGPointUPS*)pPoint, pPlane, nPoints);
			break;
		}
		break;
	case GMM_UPS:	// UPS 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 평면좌표 ==> 지리좌표
			m_GeoProj.InverseUPS((CGPointGP*)pPoint, pPlane, nPoints);
			break;
		}
	case GMM_LCC:	// Lambert Conformal Conic Projection
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 지리좌표 ==> 평면좌표
			m_GeoProj.InverseLCC((CGPointGP*)pPoint, pPlane, nPoints);
			break;
		}
		break;
	case GMM_MER:	// Mercator 투영법
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 평면좌표 ==> 지리좌표
			m_GeoProj.InverseMER((CGPointGP*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UTM:
			// 평면좌표 ==> UTM 좌표
//			m_GeoProj.InverseUTM((CGPointUTM*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UPS:
			// 평면좌표 ==> UPS 좌표
//			m_GeoProj.InverseGP((CGPointUPS*)pPoint, pPlane, nPoints);
			break;
		}
		break;
	case GMM_GP:	// 투영안함
	default:
		switch ( pPoint->m_nPointType )
		{
		case GPTYPE_GP:
			// 평면좌표 ==> 지리좌표
			m_GeoProj.InverseGP((CGPointGP*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UTM:
			// 평면좌표 ==> UTM 좌표
			m_GeoProj.InverseGP((CGPointUTM*)pPoint, pPlane, nPoints);
			break;
		case GPTYPE_UPS:
			// 평면좌표 ==> UPS 좌표
//			m_GeoProj.InverseGP((CGPointUPS*)pPoint, pPlane, nPoints);
			break;
		}
		break;
	}

	delete[] pPlane;
}



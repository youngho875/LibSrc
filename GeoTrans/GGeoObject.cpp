// GGeoObject.cpp : implementation file
//

#include "pch.h"

#include "GGeoObjStruct.h"

#include "GGeoProj.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"

#include "GGeoObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGeoObject

CGGeoObject::CGGeoObject(int nPointType) : m_nPointType(nPointType)
{
	m_Validity = 1.0;
	m_GeoProj.SetEllipsoid(EWGE);	// WGS84
}

CGGeoObject::~CGGeoObject()
{
}

void CGGeoObject::Forward(LPPOINT pt, CGPointGeo *pPointGeo, int nPoints)
{
	pt = pt;	pPointGeo = pPointGeo; nPoints = nPoints;
	ASSERT(FALSE);
}
void CGGeoObject::Inverse(CGPointGeo *pPointGeo, LPPOINT pt, int nPoints)
{
	pPointGeo = pPointGeo;	pt = pt; nPoints = nPoints;
	ASSERT(FALSE);
}

int CGGeoObject::SetEllipsoid(int eInx)
{
	return this->m_GeoProj.SetEllipsoid(eInx);

}

int CGGeoObject::GetEllipsoid() const
{
	return this->m_GeoProj.GetEllipsoid();
}

double CGGeoObject::SetValidity(double validity)
{
	ASSERT(validity>=0.0);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(validity>=0.0)) return (double) -1.0;

	double	oldValidity = m_Validity;
	m_Validity = validity;

	return oldValidity;
}

CGPointGeo* CGGeoObject::Point(int nPoint)
{
	ASSERT(FALSE);
	nPoint = nPoint;
	return NULL;
}

int CGGeoObject::SetMapMode(int nMapMode)
{
	int preMapMode;
	preMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	return preMapMode;
}

int CGGeoObject::GetMapMode() const
{
	return m_nMapMode;
}

CGPointGP CGGeoObject::SetProjBase(CGPointGP gp)
{
	return m_GeoProj.SetProjBase(gp);
}

CGPointGP CGGeoObject::SetProjBase(POINTGP gpBase)
{
	return m_GeoProj.SetProjBase(gpBase);
}

CGPointGP CGGeoObject::SetProjBase(double lonBase, double latBase)
{
	return m_GeoProj.SetProjBase(lonBase, latBase);
}

CGPointGP CGGeoObject::GetProjBase() const
{
	return m_GeoProj.GetProjBase();
}

double CGGeoObject::SetProjScaleFactor(double dScaleFactor)
{
	return m_GeoProj.SetProjScaleFactor(dScaleFactor);
}

double CGGeoObject::GetProjScaleFactor() const
{
	return m_GeoProj.GetProjScaleFactor();
}

void CGGeoObject::SetStdParallel(double dStdParallel1, double dStdParallel2)
{
	this->m_GeoProj.SetStdParallel(dStdParallel1, dStdParallel2);
}

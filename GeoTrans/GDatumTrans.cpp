#include "pch.h"
#include "GDatumTrans.h"

#include "GGeoObjStruct.h"
#include "GGeoAbst.h"
#include "GGeoGP.h"
#include "GEllipsoid.h"
#include "GDatum.h"
#include "GGeoTrans.h"



/////////////////////////////////////////////////////////////////
static CGGeoTrans gb_DatumTrans(EWGE);
static CGDatum	gb_TokyoDatum(DTOK);
static CGDatum	gb_Wgs84Datum(DWGE);

void DTWgsToTok(CGPointGeo *pToGPs, CGPointGeo *pFromGPs, int nCount)
{
	if (pToGPs == NULL) return;
	if (pFromGPs == NULL) return;
	
	CGPointGP *pNewFromGPs = NULL;
	BOOL bCreate = FALSE;
	int i = 0;

	if (pToGPs == pFromGPs)
	{
		pNewFromGPs = new CGPointGP[nCount];

		for (i = 0; nCount > i; i++)
			pNewFromGPs[i].SetPoint(&((CGPointGP *) pFromGPs)[i]);

		bCreate = TRUE;
	}
	else
		pNewFromGPs = (CGPointGP *) pFromGPs;
	
	CGPointGP	*pToGP, *pFromGP;
	pToGP = pFromGP = NULL;

	for (i = 0; nCount > i; i++)
	{
		pToGP   = &((CGPointGP *) pToGPs)[i];
		pFromGP = (CGPointGP *) &pNewFromGPs[i];
		gb_DatumTrans.StdMolodensky(pToGP,   &gb_TokyoDatum,
									pFromGP, &gb_Wgs84Datum/*, &(pToGP->m_P.ht), &(pFromGP->m_P.ht)*/);
		pToGP = pFromGP = NULL;
	}

	if (bCreate)
	{
		if (pNewFromGPs) delete [] pNewFromGPs;
	}
}

void DTTokToWgs(CGPointGeo *pToGPs, CGPointGeo *pFromGPs, int nCount)
{
	if (pToGPs == NULL) return;
	if (pFromGPs == NULL) return;

	CGPointGP *pNewFromGPs = NULL;
	BOOL bCreate = FALSE;
	int i = 0;

	if (pToGPs == pFromGPs)
	{
		pNewFromGPs = new CGPointGP[nCount];

		for (i = 0; nCount > i; i++)
			pNewFromGPs[i].SetPoint(&((CGPointGP *) pFromGPs)[i]);

		bCreate = TRUE;
	}
	else
		pNewFromGPs = (CGPointGP *) pFromGPs;
	
	CGPointGP	*pToGP, *pFromGP;
	pToGP = pFromGP = NULL;

	for (i = 0; nCount > i; i++)
	{
		pToGP   = &((CGPointGP *) pToGPs)[i];
		pFromGP = (CGPointGP *) &pNewFromGPs[i];

		gb_DatumTrans.StdMolodensky(pToGP,   &gb_Wgs84Datum,
									pFromGP, &gb_TokyoDatum/*, &(pToGP->m_P.ht), &(pFromGP->m_P.ht)*/);
		pToGP = pFromGP = NULL;
	}

	if (bCreate)
	{
		if (pNewFromGPs) delete [] pNewFromGPs;
	}
}

/*
void DTWgsToTok(CGRectGP *pToRectGP, CGRectGP *pFromRectGP)
{
	if (pToRectGP == NULL) return;
	if (pFromRectGP == NULL) return;
	
	CGPointGP TopLeftGP, BottomRightGP;

	TopLeftGP     = pFromRectGP->TopLeft();
	BottomRightGP = pFromRectGP->BottomRight();

	DTWgsToTok(&TopLeftGP, &TopLeftGP, 1);
	DTWgsToTok(&BottomRightGP, &BottomRightGP, 1);

	pToRectGP->SetRect(&TopLeftGP, &BottomRightGP);
}

void DTTokToWgs(CGRectGP *pToRectGP, CGRectGP *pFromRectGP)
{
	if (pToRectGP == NULL) return;
	if (pFromRectGP == NULL) return;
	
	CGPointGP TopLeftGP, BottomRightGP;

	TopLeftGP     = pFromRectGP->TopLeft();
	BottomRightGP = pFromRectGP->BottomRight();

	DTTokToWgs(&TopLeftGP, &TopLeftGP, 1);
	DTTokToWgs(&BottomRightGP, &BottomRightGP, 1);

	pToRectGP->SetRect(&TopLeftGP, &BottomRightGP);
}
*/
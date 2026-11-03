#include "pch.h"
#include "ObjectLinkedMultiCurve.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectLinkedMultiCurve::ObjectLinkedMultiCurve(UINT nNumberOfPoint)
		:ObjectLinkedLine(nNumberOfPoint, LINKED_MULTI_CURVE)
	{
	}

	ObjectLinkedMultiCurve::~ObjectLinkedMultiCurve()
	{
	}


	void ObjectLinkedMultiCurve::SetSecondLinePoint(Point* pPoint)
	{
		if (pPoint == NULL)
			return;

		pObjectPoint[1]->SetXY(pPoint->X(), pPoint->Y());
	}

	Point* ObjectLinkedMultiCurve::GetSecondPoint()
	{
		return pObjectPoint[1];
	}
}

#include "pch.h"
#include "ObjectLinkedCurve.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectLinkedCurve::ObjectLinkedCurve(UINT nNumberOfPoint)
		:ObjectLinkedLine(nNumberOfPoint, LINKED_CURVE)
	{
		Property.bIsFill = FALSE;
	}

	ObjectLinkedCurve::~ObjectLinkedCurve()
	{
	}
}

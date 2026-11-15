#include "pch.h"
#include "ObjectLinkedMultiLine.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectLinkedMultiLine::ObjectLinkedMultiLine(UINT nNumberOfPoint)
		:ObjectLinkedLine(nNumberOfPoint, LINKED_MULTI_LINE)
	{
	}

	ObjectLinkedMultiLine::~ObjectLinkedMultiLine()
	{
	}

	void ObjectLinkedMultiLine::SetSecondLinePoint(Point* pPoint)
	{
		if (pPoint == NULL)
			return;

		pObjectPoint[1]->SetXY(pPoint->X(), pPoint->Y());
	}

	Point* ObjectLinkedMultiLine::GetSecondPoint()
	{
		return pObjectPoint[1];
	}
}
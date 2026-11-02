#include "pch.h"
#include "ObjectLinkedLine.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectLinkedLine::ObjectLinkedLine(UINT nNumberOfPoint)
		:OBJECT(LINKED_LINE, nNumberOfPoint)
	{
		nPoints = nNumberOfPoint;

		for (UINT i = 0; i < nNumberOfPoint; i++)
		{
			if (i >= MAX_LINKED_OBJECT)
				break;
			pObjectPoint[i] = &pt[1];
		}

		nLinkedLine_Start_ID = 0;
		nLinkedLine_End_ID = 0;


		nObject_start_point_id = 0;
		nObject_end_point_id = 0;

		Property.bIsFill = FALSE;
	}

	ObjectLinkedLine::ObjectLinkedLine(UINT nNumberOfPoint, OVERLAY::OBJECT_TYPE eType)
		:OBJECT(eType, nNumberOfPoint)
	{
		if (nNumberOfPoint > MAX_LINKED_OBJECT)
			return;

		nPoints = nNumberOfPoint;

		for (UINT i = 0; i < nNumberOfPoint; i++)
		{
			if (i >= MAX_LINKED_OBJECT)
				break;
			pObjectPoint[i] = &pt[1];
		}

		nObject_start_point_id = 0;
		nObject_end_point_id = 0;
	}

	ObjectLinkedLine::~ObjectLinkedLine()
	{
	}

	void ObjectLinkedLine::SetLineStyle(LINE_STYLE eLineStyle)
	{
		Property.eLineStyle = eLineStyle;
	}

	void ObjectLinkedLine::SetLineColor(COLORREF LineColor)
	{
		Property.LineColor = LineColor;
	}

	void ObjectLinkedLine::SetStartArrow(LineCap eLineStyle)
	{
		Property.eStartLineStyle = eLineStyle;
	}

	void ObjectLinkedLine::SetEndArrow(LineCap eLineStyle)
	{
		Property.eEndLineStyle = eLineStyle;
	}
	/*
	void ObjectLinkedLine::SetStartLinePoint(Point* pPoint)
	{
		if(nPoints >= MAX_LINKED_OBJECT)
			return;

		if(pPoint==NULL)
			return;

		pObjectPoint[0] = pPoint;
	}

	void ObjectLinkedLine::SetEndLinePoint(Point* pPoint)
	{
		if(nPoints >= MAX_LINKED_OBJECT)
			return;

		if(pPoint==NULL)
			return;

		pObjectPoint[nPoints-1] = pPoint;
	}
	*/

	Point* ObjectLinkedLine::GetStartPoint()
	{
		return pObjectPoint[0];
	}

	Point* ObjectLinkedLine::GetEndPoint()
	{
		return pObjectPoint[nPoints - 1];
	}


	void ObjectLinkedLine::SetLinkPoint(UINT nIndex, OVERLAY::Point* pPoint)
	{
		if (pPoint == NULL)
			return;

		if (nIndex >= nPoints)
			return;

		pObjectPoint[nIndex] = pPoint;
	}

	void ObjectLinkedLine::SetPoint(UINT nIndex, int x, int y)
	{
		double dx = (double)x;
		double dy = (double)y;

		if (nIndex == 0 || nIndex >= nPoints)
			return;

		pt[nIndex].SetXY(dx, dy);

		pObjectPoint[nIndex] = &pt[nIndex];
	}

	Point** ObjectLinkedLine::GetLinkedLinePoint()
	{
		return pObjectPoint;
	}

	/*
	void ObjectLinkedLine::SetLinkedStartID(UINT nLinkedLineID)
	{
		nLinkedLine_Start_ID = nLinkedLineID;
	}

	void ObjectLinkedLine::SetLinkedEndID(UINT nLinkedLineID)
	{
		nLinkedLine_End_ID = nLinkedLineID;
	}

	UINT ObjectLinkedLine::GetLinkedStartID(UINT nLinkedLineID)
	{
		return nLinkedLine_Start_ID;
	}

	UINT ObjectLinkedLine::GetLinkedEndID(UINT nLinkedLineID)
	{
		return nLinkedLine_End_ID;
	}
	*/
}
#include "pch.h"
#include "ObjectLinkedMultiLineDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectLinkedMultiLineDraw::CObjectLinkedMultiLineDraw()
	{
	}

	CObjectLinkedMultiLineDraw::~CObjectLinkedMultiLineDraw()
	{
	}
	void CObjectLinkedMultiLineDraw::DrawLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->GetAddedPointsNum() < 2)
			return;

		ObjectLinkedLine* pObject = (ObjectLinkedLine*)object;
		if (pObject == NULL)
			return;

		OVERLAY::Point** pPoints = pObject->GetLinkedLinePoint();

		int i = 0;
		for (i = 0; i < pObject->GetAddedPointsNum(); i++)
			if (pPoints[i] == NULL)
				return;

		Gdiplus::Pen		pen(object->Property.LineColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);
		/*
		pen.SetLineCap(
		(Gdiplus::LineCap)object->Property.eStartLineStyle,
		(Gdiplus::LineCap)object->Property.eEndLineStyle,
		(Gdiplus::DashCap)object->Property.eDashStyle
		);
		*/
		///////////////////////////////////////////////////////////
		// 도형(선) 화살표 설정.
		Gdiplus::AdjustableArrowCap StartArrow(0, 0, true);
		Gdiplus::AdjustableArrowCap EndArrow(0, 0, true);

		StartArrow.SetHeight(object->Property.nStartLineArrowStyle);
		StartArrow.SetWidth(object->Property.nStartLineArrowStyle);

		EndArrow.SetHeight(object->Property.nEndLineArrowStyle);
		EndArrow.SetWidth(object->Property.nEndLineArrowStyle);

		pen.SetCustomStartCap(&StartArrow);
		pen.SetCustomEndCap(&EndArrow);
		///////////////////////////////////////////////////////////

		Gdiplus::PointF	pt[MAX_LINKED_OBJECT];

		for (i = 0; i < pObject->GetAddedPointsNum(); i++)
		{
			pt[i].X = (Gdiplus::REAL)pPoints[i]->X();
			pt[i].Y = (Gdiplus::REAL)pPoints[i]->Y();
		}

		if (pObject->Property.bIsLine)
		{
			if (pObject->GetType() == OVERLAY::LINKED_MULTI_LINE)
				g->DrawLines(&pen, pt, pObject->GetAddedPointsNum());
		}

		//SAFE_DELETE_ARRAY(pt);
	}

}


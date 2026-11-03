#include "pch.h"
#include "ObjectLinkedLineDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectLinkedLineDraw::CObjectLinkedLineDraw()
	{
	}

	CObjectLinkedLineDraw::~CObjectLinkedLineDraw()
	{
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawOutLine()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectLinkedLineDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		ObjectLinkedLine* pObject = (ObjectLinkedLine*)object;
		if (pObject == NULL)
			return;

		OVERLAY::Point** pPoints = pObject->GetLinkedLinePoint();

		int i = 0;
		for (i = 0; i < pObject->GetPoints(); i++)
			if (pPoints[i] == NULL)
				return;

		Gdiplus::SolidBrush brush(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));


		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		for (i = 0; i < object->GetPoints(); i++)
			g->FillEllipse(&brush, (INT)pPoints[i]->X() - nSizeOfOutLinePoint, (INT)pPoints[i]->Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectLinkedLineDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{

		ObjectLinkedLine* pObject = (ObjectLinkedLine*)object;
		if (pObject == NULL)
			return;

		OVERLAY::Point** pPoints = pObject->GetLinkedLinePoint();

		int i = 0;
		for (i = 0; i < pObject->GetPoints(); i++)
			if (pPoints[i] == NULL)
				return;

		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

		COLORREF	temp_line_color = object->Property.LineColor;
		double		temp_line_width = object->Property.dLineWidth;

		object->Property.LineColor = 0xffff0000 | OUT_LINE_SELECT;
		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;

		Draw(object, g);

		object->Property.LineColor = temp_line_color;
		object->Property.dLineWidth = temp_line_width;

		for (i = 0; i < object->GetPoints(); i++)
		{
			brush.SetColor(Gdiplus::Color(255, 255, 0, GENERAL_POINTS + i));
			g->FillEllipse(&brush, (INT)pPoints[i]->X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)pPoints[i]->Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		}
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectLinkedLineDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DrawLine(object, g);
	}

	// 10. 선 (화살표)
	void CObjectLinkedLineDraw::DrawLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->GetPoints() < 2)
			return;

		ObjectLinkedLine* pObject = (ObjectLinkedLine*)object;
		if (pObject == NULL)
			return;

		OVERLAY::Point** pPoints = pObject->GetLinkedLinePoint();

		int i = 0;
		for (i = 0; i < pObject->GetPoints(); i++)
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

		Gdiplus::PointF* pt = new Gdiplus::PointF[pObject->GetPoints()];
		for (i = 0; i < pObject->GetPoints(); i++)
		{
			pt[i].X = (Gdiplus::REAL)pPoints[i]->X();
			pt[i].Y = (Gdiplus::REAL)pPoints[i]->Y();
		}

		if (pObject->Property.bIsLine)
		{
			if (pObject->GetType() == OVERLAY::LINKED_LINE)

				g->DrawLines(&pen, pt, pObject->GetPoints());
			else if (pObject->GetType() == OVERLAY::LINKED_CURVE ||
				pObject->GetType() == OVERLAY::LINKED_MULTI_CURVE)
				g->DrawCurve(&pen, pt, pObject->GetPoints());
		}
	}
}

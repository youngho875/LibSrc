#include "pch.h"
#include "ObjectLineDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectLineDraw::CObjectLineDraw()
	{
	}

	CObjectLineDraw::~CObjectLineDraw()
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
	void CObjectLineDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		g->FillEllipse(&brush_point, (INT)object->pt[LEFT_UP].X() - nSizeOfOutLinePoint, (INT)object->pt[LEFT_UP].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt[RIGHT_DOWN].X() - nSizeOfOutLinePoint, (INT)object->pt[RIGHT_DOWN].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectLineDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

		COLORREF	temp_fill_color = object->Property.FillColor;
		COLORREF	temp_line_color = object->Property.LineColor;
		double		temp_line_width = object->Property.dLineWidth;

		object->Property.FillColor = 0xffff0000 | OUT_LINE_SELECT;
		object->Property.LineColor = 0xffff0000 | OUT_LINE_SELECT;
		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;

		Draw(object, g);

		object->Property.FillColor = temp_fill_color;
		object->Property.LineColor = temp_line_color;
		object->Property.dLineWidth = temp_line_width;

		for (int i = 0; i < object->GetPoints(); i++)
		{
			brush.SetColor(Gdiplus::Color(255, 255, 0, GENERAL_POINTS + i));
			g->FillEllipse(&brush, (INT)object->pt[i].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt[i].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
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
	void CObjectLineDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DrawLine(object, g);
	}

	// 10. 선 (화살표)
	void CObjectLineDraw::DrawLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->GetPoints() < 2)
			return;
		Gdiplus::PointF		pt[6];

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
		for (int i = 0; i < object->GetPoints(); i++)
		{
			pt[i].X = (Gdiplus::REAL)object->pt[i].X();
			pt[i].Y = (Gdiplus::REAL)object->pt[i].Y();
		}

		if (object->Property.bIsLine)
			g->DrawLine(&pen, pt[0], pt[1]);
	}
}

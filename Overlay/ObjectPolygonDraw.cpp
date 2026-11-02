#include "pch.h"
#include "ObjectPolygonDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectPolygonDraw::CObjectPolygonDraw()
	{
	}

	CObjectPolygonDraw::~CObjectPolygonDraw()
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
	void CObjectPolygonDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		// 각 점의 편집 지점.
		for (int i = 0; i < object->nPointIndex; i++)
			g->FillEllipse(
				&brush_point,
				(float)object->pt[i].X() - nSizeOfOutLinePoint,
				(float)object->pt[i].Y() - nSizeOfOutLinePoint,
				(float)SIZE_OF_OUT_LINE_POINT,
				(float)SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectPolygonDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
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

		// 각 점의 편집 지점.
		SHORT nPointsOfFigure = 0;
		// 점 편집 영역은  Red,Green 값으로 구분한다.
		Gdiplus::SolidBrush brush_selected_point(Gdiplus::Color(255, 255, 255, 255));

		char ColorID[2];
		Gdiplus::REAL pt_x = 0;
		Gdiplus::REAL pt_y = 0;
		for (int i = 0; i < object->nPointIndex; i++)
		{
			pt_x = (Gdiplus::REAL)object->pt[i].X();
			pt_y = (Gdiplus::REAL)object->pt[i].Y();

			nPointsOfFigure = GENERAL_POINTS + i;
			memcpy(ColorID, &nPointsOfFigure, sizeof(ColorID));
			brush_selected_point.SetColor(Gdiplus::Color(255, 255, ColorID[1], ColorID[0]));	// 아웃라인과 겹치는 색은 배제함.				
			g->FillEllipse(&brush_selected_point, (INT)pt_x - SIZE_OF_OUT_LINE_POINT / 2, (INT)pt_y - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
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
	void CObjectPolygonDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DrawPolygon(object, g);
	}

	void CObjectPolygonDraw::DrawPolygon(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->nPointIndex < 2)
			return;
		Gdiplus::PointF		pt[MAX_POINTS];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		for (UINT i = 0; i < object->nPointIndex; i++)
		{
			pt[i].X = (Gdiplus::REAL)object->pt[i].X();
			pt[i].Y = (Gdiplus::REAL)object->pt[i].Y();
		}

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, (INT)object->nPointIndex);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, (INT)object->nPointIndex);
	}

	void CObjectPolygonDraw::DrawRotatePolygon(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->nPointIndex < 2)
			return;
		Gdiplus::PointF		pt[MAX_POINTS];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		double cx = 0.0;
		double cy = 0.0;

		cx = (object->pt_out_line[RIGHT_BOTTOM].X() + object->pt_out_line[LEFT_TOP].X()) / 2;
		cy = (object->pt_out_line[RIGHT_BOTTOM].Y() + object->pt_out_line[LEFT_TOP].Y()) / 2;

		UINT i = 0;
		for (i = 0; i < object->nPointIndex; i++)
		{
			pt[i].X = (Gdiplus::REAL)object->pt[i].X();
			pt[i].Y = (Gdiplus::REAL)object->pt[i].Y();
		}

		double dDistance = 0.0;
		double dAngle = 0.0;
		double dx = 0;
		double dy = 0;

		for (i = 0; i < object->nPointIndex; i++)
		{
			dDistance = Distance((double)cx, (double)cy, (double)pt[i].X, (double)pt[i].Y);

			if (cy - pt[i].Y > 0)
			{
				if (cy - pt[i].Y > dDistance)
					dDistance = cy - pt[i].Y;
			}
			else if (cy - pt[i].Y == 0 && abs(cx - pt[i].X) < 1)
			{
				pt[i].Y = cy;
				continue;
			}
			else
			{
				if (pt[i].Y - cy > dDistance)
					dDistance = pt[i].Y - cy;
			}

			dAngle = acos(((double)(cy - (double)pt[i].Y)) / dDistance);
			dAngle = Radian2Degree(dAngle);

			if (cx > (double)pt[i].X)
				dAngle *= -1;

			ReferencePositon_Distance_Angle2TargetPosition(dDistance, object->Property.dAngle + (dAngle)-90, 0, 0, &dx, &dy);
			pt[i].X = (Gdiplus::REAL)(dx + cx);
			pt[i].Y = (Gdiplus::REAL)(dy + cy);
		}

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, (INT)object->nPointIndex);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, (INT)object->nPointIndex);
	}
}
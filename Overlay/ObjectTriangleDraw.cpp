#include "pch.h"
#include "ObjectTriangleDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectTriangleDraw::CObjectTriangleDraw()
	{
	}

	CObjectTriangleDraw::~CObjectTriangleDraw()
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
	void CObjectTriangleDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		object->SetOutLine();

		Gdiplus::Pen ppen(Gdiplus::Color(OUT_LINE_COLOR_A, OUT_LINE_COLOR_R, OUT_LINE_COLOR_G, OUT_LINE_COLOR_B));
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		ppen.SetWidth(EDIT_LINE_WIDTH);

		Gdiplus::PointF temp_point_for_gdi[9];
		for (int i = 0; i < 9; i++)
		{
			temp_point_for_gdi[i].X = (Gdiplus::REAL)object->pt_out_line[i].X();
			temp_point_for_gdi[i].Y = (Gdiplus::REAL)object->pt_out_line[i].Y();
		}

		g->DrawPolygon(&ppen, temp_point_for_gdi, 8);
		g->DrawLine(&ppen, (INT)object->pt_out_line[ROTATE_CENTER].X(), (INT)object->pt_out_line[ROTATE_CENTER].Y(), (INT)object->pt_out_line[TOP_CENTER].X(), (INT)object->pt_out_line[TOP_CENTER].Y());

		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		for (int j = 0; j < 9; j++)
			g->FillEllipse(&brush_point, (INT)temp_point_for_gdi[j].X - nSizeOfOutLinePoint, (INT)temp_point_for_gdi[j].Y - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);

		brush_point.SetColor(Gdiplus::Color(OUT_LINE_POINT_OPTION_COLOR_A, OUT_LINE_POINT_OPTION_COLOR_R, OUT_LINE_POINT_OPTION_COLOR_B, OUT_LINE_POINT_OPTION_COLOR_B));
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[SWEEP_POSITION].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[SWEEP_POSITION].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectTriangleDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		object->SetOutLine();

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

		Gdiplus::Pen ppen(Gdiplus::Color(255, 255, 0, OUT_LINE_SELECT));
		ppen.SetWidth(EDIT_SELECT_LINE_WIDTH);

		Gdiplus::PointF	temp_point_for_gdi[8];

		for (int i = 0; i < 8; i++)
		{
			temp_point_for_gdi[i].X = (Gdiplus::REAL)object->pt_out_line[i].X();
			temp_point_for_gdi[i].Y = (Gdiplus::REAL)object->pt_out_line[i].Y();
		}

		g->DrawPolygon(&ppen, temp_point_for_gdi, 8);
		g->DrawLine(&ppen, (INT)object->pt_out_line[ROTATE_CENTER].X(), (INT)object->pt_out_line[ROTATE_CENTER].Y(), (INT)object->pt_out_line[TOP_CENTER].X(), (INT)object->pt_out_line[TOP_CENTER].Y());

		Gdiplus::SolidBrush brush_rotate(Gdiplus::Color(255, 255, 0, ROTATE_CENTER));
		Gdiplus::SolidBrush brush_left_top(Gdiplus::Color(255, 255, 0, LEFT_TOP));
		Gdiplus::SolidBrush brush_top_center(Gdiplus::Color(255, 255, 0, TOP_CENTER));
		Gdiplus::SolidBrush brush_right_top(Gdiplus::Color(255, 255, 0, RIGHT_TOP));
		Gdiplus::SolidBrush brush_left_center(Gdiplus::Color(255, 255, 0, LEFT_CENTER));
		Gdiplus::SolidBrush brush_right_center(Gdiplus::Color(255, 255, 0, RIGHT_CENTER));
		Gdiplus::SolidBrush brush_left_bottom(Gdiplus::Color(255, 255, 0, LEFT_BOTTOM));
		Gdiplus::SolidBrush brush_right_bottom(Gdiplus::Color(255, 255, 0, RIGHT_BOTTOM));
		Gdiplus::SolidBrush brush_bottom_center(Gdiplus::Color(255, 255, 0, BOTTOM_CENTER));

		g->FillEllipse(&brush_rotate, (INT)object->pt_out_line[ROTATE_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[ROTATE_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_top, (INT)object->pt_out_line[LEFT_TOP].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_TOP].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_top_center, (INT)object->pt_out_line[TOP_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[TOP_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_top, (INT)object->pt_out_line[RIGHT_TOP].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_TOP].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_center, (INT)object->pt_out_line[LEFT_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_center, (INT)object->pt_out_line[RIGHT_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_bottom, (INT)object->pt_out_line[LEFT_BOTTOM].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_BOTTOM].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_bottom, (INT)object->pt_out_line[RIGHT_BOTTOM].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_BOTTOM].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_bottom_center, (INT)object->pt_out_line[BOTTOM_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[BOTTOM_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);

		Gdiplus::SolidBrush brush_sweep_position(Gdiplus::Color(255, 255, 0, SWEEP_POSITION));
		g->FillEllipse(&brush_sweep_position, (INT)object->pt_out_line[SWEEP_POSITION].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[SWEEP_POSITION].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectTriangleDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		if (object->Property.dAngle != 0)
			DrawRotateTriangle(object, g);
		else
			DrawTriangle(object, g);
	}

	//--------------------------------------------
	// 4. 삼각형
	//--------------------------------------------
	void CObjectTriangleDraw::DrawTriangle(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::PointF		pt[3];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		pt[0].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[0].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[1].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[1].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[2].X = (Gdiplus::REAL)object->pt[TRIANGLE_VERTEX].X();
		pt[2].Y = (Gdiplus::REAL)object->pt[TRIANGLE_VERTEX].Y();

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, 3);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 3);
	}

	void CObjectTriangleDraw::DrawRotateTriangle(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::PointF		pt[3];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		double cx = 0.0;
		double cy = 0.0;

		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;

		pt[0].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[0].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[1].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[1].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[2].X = (Gdiplus::REAL)object->pt[TRIANGLE_VERTEX].X();
		pt[2].Y = (Gdiplus::REAL)object->pt[TRIANGLE_VERTEX].Y();

		double dDistance = 0.0;
		double dAngle = 0.0;
		double dx = 0;
		double dy = 0;

		for (int i = 0; i < 3; i++)
		{
			dDistance = Distance((double)cx, (double)cy, (double)pt[i].X, (double)pt[i].Y);
			/////////////////////////
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
			//////////////////////////////////////////
			dAngle = acos(((double)(cy - (double)pt[i].Y)) / dDistance);
			dAngle = Radian2Degree(dAngle);
			if (cx > (double)pt[i].X)
				dAngle *= -1;

			ReferencePositon_Distance_Angle2TargetPosition(dDistance, object->Property.dAngle + (dAngle)-90, 0, 0, &dx, &dy);
			pt[i].X = (Gdiplus::REAL)(dx + cx);
			pt[i].Y = (Gdiplus::REAL)(dy + cy);
		}

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, 3);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 3);
	}
}
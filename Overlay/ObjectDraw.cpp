#include "pch.h"
#include "ObjectDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectDraw::CObjectDraw()
	{
	}

	CObjectDraw::~CObjectDraw()
	{
	}
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: GetEllipsePositon()
	// 목적				: 타원을 형성하고 있는 점의 위치 획득
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectDraw::GetEllipsePositon(double a, double b, double cx, double cy, double theta, double* x, double* y)
	{
		if (theta > 360)
			theta -= 360;
		else if (theta < 0)
			theta += 360;

		*x = cx + a * cos(Degree2Radian((double)theta));
		*y = cy + b * sin(Degree2Radian((double)theta));
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawOutLine()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		Gdiplus::Pen ppen(Gdiplus::Color(OUT_LINE_COLOR_A, OUT_LINE_COLOR_R, OUT_LINE_COLOR_G, OUT_LINE_COLOR_B));
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		ppen.SetWidth(EDIT_LINE_WIDTH);
		Gdiplus::PointF temp_point_for_gdi[9];

		int i = 0;
		////////////////////////////////////////////////////////	
		object->SetOutLine();

		for (i = 0; i < 9; i++)
		{
			temp_point_for_gdi[i].X = (Gdiplus::REAL)object->pt_out_line[i].X();
			temp_point_for_gdi[i].Y = (Gdiplus::REAL)object->pt_out_line[i].Y();
		}

		g->DrawPolygon(&ppen, temp_point_for_gdi, 8);
		g->DrawLine(&ppen, (INT)object->pt_out_line[ROTATE_CENTER].X(), (INT)object->pt_out_line[ROTATE_CENTER].Y(), (INT)object->pt_out_line[TOP_CENTER].X(), (INT)object->pt_out_line[TOP_CENTER].Y());

		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		for (int j = 0; j < 9; j++)
			g->FillEllipse(&brush_point, (INT)temp_point_for_gdi[j].X - nSizeOfOutLinePoint, (INT)temp_point_for_gdi[j].Y - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);

	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::Pen ppen(Gdiplus::Color(255, 255, 0, OUT_LINE_SELECT));
		ppen.SetWidth(EDIT_SELECT_LINE_WIDTH);

		Gdiplus::PointF	temp_point_for_gdi[8];

		int i = 0;
		object->SetOutLine();
		for (i = 0; i < 8; i++)
		{
			temp_point_for_gdi[i].X = (Gdiplus::REAL)object->pt_out_line[i].X();
			temp_point_for_gdi[i].Y = (Gdiplus::REAL)object->pt_out_line[i].Y();
		}

		g->DrawPolygon(&ppen, temp_point_for_gdi, 8);
		g->DrawLine(&ppen, (INT)object->pt_out_line[ROTATE_CENTER].X(), (INT)object->pt_out_line[ROTATE_CENTER].Y(), (INT)object->pt_out_line[TOP_CENTER].X(), (INT)object->pt_out_line[TOP_CENTER].Y());

		Gdiplus::SolidBrush brush_rotate(Gdiplus::Color(255, 255, 0, (UINT)ROTATE_CENTER));
		Gdiplus::SolidBrush brush_left_top(Gdiplus::Color(255, 255, 0, (UINT)LEFT_TOP));
		Gdiplus::SolidBrush brush_top_center(Gdiplus::Color(255, 255, 0, (UINT)TOP_CENTER));
		Gdiplus::SolidBrush brush_right_top(Gdiplus::Color(255, 255, 0, (UINT)RIGHT_TOP));
		Gdiplus::SolidBrush brush_left_center(Gdiplus::Color(255, 255, 0, (UINT)LEFT_CENTER));
		Gdiplus::SolidBrush brush_right_center(Gdiplus::Color(255, 255, 0, (UINT)RIGHT_CENTER));
		Gdiplus::SolidBrush brush_left_bottom(Gdiplus::Color(255, 255, 0, (UINT)LEFT_BOTTOM));
		Gdiplus::SolidBrush brush_right_bottom(Gdiplus::Color(255, 255, 0, (UINT)RIGHT_BOTTOM));
		Gdiplus::SolidBrush brush_bottom_center(Gdiplus::Color(255, 255, 0, (UINT)BOTTOM_CENTER));

		g->FillEllipse(&brush_rotate, (INT)object->pt_out_line[ROTATE_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[ROTATE_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_top, (INT)object->pt_out_line[LEFT_TOP].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_TOP].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_top_center, (INT)object->pt_out_line[TOP_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[TOP_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_top, (INT)object->pt_out_line[RIGHT_TOP].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_TOP].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_center, (INT)object->pt_out_line[LEFT_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_center, (INT)object->pt_out_line[RIGHT_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_left_bottom, (INT)object->pt_out_line[LEFT_BOTTOM].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[LEFT_BOTTOM].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_right_bottom, (INT)object->pt_out_line[RIGHT_BOTTOM].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[RIGHT_BOTTOM].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_bottom_center, (INT)object->pt_out_line[BOTTOM_CENTER].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[BOTTOM_CENTER].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);

	}

	void CObjectDraw::DrawMap(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DWORD	temp_color_line = object->Property.LineColor;
		DWORD	temp_color_fill = object->Property.FillColor;
		double	temp_line_width = object->Property.dLineWidth;

		OVERLAY::LINE_STYLE temp_eLineStyle = object->Property.eLineStyle;
		OVERLAY::LineCap temp_eStartLineStyle = object->Property.eStartLineStyle;
		OVERLAY::LineCap temp_eEndLineStyle = object->Property.eEndLineStyle;

		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;

		object->Property.LineColor = 0xff000000 | (object->nID & 0xffff);
		object->Property.FillColor = 0xff000000 | (object->nID & 0xffff);

		object->Property.eLineStyle = OVERLAY::Solid;
		object->Property.eStartLineStyle = OVERLAY::LineCapFlat;
		object->Property.eEndLineStyle = OVERLAY::LineCapFlat;

		Draw(object, g);

		object->Property.LineColor = temp_color_line;
		object->Property.FillColor = temp_color_fill;
		object->Property.dLineWidth = temp_line_width;

		object->Property.eLineStyle = temp_eLineStyle;
		object->Property.eStartLineStyle = temp_eStartLineStyle;
		object->Property.eEndLineStyle = temp_eEndLineStyle;
	}
}
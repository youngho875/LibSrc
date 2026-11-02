#include "pch.h"
#include "ObjectPieDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectPieDraw::CObjectPieDraw()
	{
	}

	CObjectPieDraw::~CObjectPieDraw()
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
	void CObjectPieDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
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
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[SWEEP_ANGLE_1].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[SWEEP_ANGLE_1].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[SWEEP_ANGLE_2].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[SWEEP_ANGLE_2].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectPieDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
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

		Gdiplus::SolidBrush brush_sweep_angle1(Gdiplus::Color(255, 255, 0, SWEEP_ANGLE_1));
		Gdiplus::SolidBrush brush_sweep_angle2(Gdiplus::Color(255, 255, 0, SWEEP_ANGLE_2));

		g->FillEllipse(&brush_sweep_angle1, (INT)object->pt_out_line[SWEEP_ANGLE_1].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[SWEEP_ANGLE_1].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_sweep_angle2, (INT)object->pt_out_line[SWEEP_ANGLE_2].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt_out_line[SWEEP_ANGLE_2].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawMap()
	// 목적				: 도형을 선택하기위한 메모리 멥 설정
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	/*
	void CObjectPieDraw::DrawMap(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		OVERLAY::ObjectPie* temp_object = (OVERLAY::ObjectPie*)object;

		DWORD	temp_color_line		= object->Property.LineColor;
		DWORD	temp_color_fill		= object->Property.FillColor;
		double	temp_line_width	= object->Property.dLineWidth;

		if(object->Property.dLineWidth<EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;
		if(temp_object->Property.bIsLine==TRUE)
		{
			//도형 선택을 위한 메모리
			temp_object->Property.LineColor = 0xff000000 | (temp_object->nID&0xffff);
			Draw(temp_object, g);
		}

		if(temp_object->Property.bIsFill==TRUE)
		{
			//도형 선택을 위한 메모리
			temp_object->Property.FillColor = 0xff000000 | (temp_object->nID&0xffff);
			Draw(temp_object, g);
		}
		temp_object->Property.LineColor = temp_color_line;
		temp_object->Property.FillColor = temp_color_fill;
		object->Property.dLineWidth= temp_line_width;
	}
	*/
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectPieDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		object->SetOutLine();
		if (object->Property.dAngle != 0)
			DrawRotatePie(object, g, nResolution);
		else
			DrawPie(object, g, nResolution);
	}

	void CObjectPieDraw::DrawPie(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, UINT nResolution)
	{
		double a = 0.0;	// 장축.
		double b = 0.0;	// 단축.
		double cx = 0.0;
		double cy = 0.0;
		Gdiplus::PointF		_pt[362];
		UINT	nPtCount = 2;

		OVERLAY::ObjectPie* temp_object = (OVERLAY::ObjectPie*)object;

		Gdiplus::Pen		pen(temp_object->Property.LineColor);
		Gdiplus::SolidBrush	brush(temp_object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)temp_object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)temp_object->Property.eLineStyle);

		a = abs((double)(temp_object->pt[RIGHT_DOWN].X() - temp_object->pt[LEFT_UP].X())) / 2;
		b = abs((double)(temp_object->pt[RIGHT_DOWN].Y() - temp_object->pt[LEFT_UP].Y())) / 2;

		cx = (temp_object->pt[RIGHT_DOWN].X() + temp_object->pt[LEFT_UP].X()) / 2;
		cy = (temp_object->pt[RIGHT_DOWN].Y() + temp_object->pt[LEFT_UP].Y()) / 2;

		//부채꼴은 중점에서 그리기를 시작한다.
		_pt[0].X = (Gdiplus::REAL)cx;
		_pt[0].Y = (Gdiplus::REAL)cy;

		double dSweepAngle = temp_object->m_dSweepAngle[1];

		if (temp_object->m_dSweepAngle[0] > 360)
			temp_object->m_dSweepAngle[0] -= 360;
		else if (temp_object->m_dSweepAngle[0] < 0)
			temp_object->m_dSweepAngle[0] += 360;

		if (temp_object->m_dSweepAngle[1] > 360)
			temp_object->m_dSweepAngle[1] -= 360;
		else if (temp_object->m_dSweepAngle[1] < 0)
			temp_object->m_dSweepAngle[1] += 360;

		if (temp_object->m_dSweepAngle[0] > temp_object->m_dSweepAngle[1])
			dSweepAngle = (360 + temp_object->m_dSweepAngle[0] - (temp_object->m_dSweepAngle[0] - temp_object->m_dSweepAngle[1]));

		double dx = 0;
		double dy = 0;
		for (double i = temp_object->m_dSweepAngle[0]; i < dSweepAngle; i += nResolution)
		{
			GetEllipsePositon(a, b, cx, cy, i + 270, &dx, &dy);
			_pt[nPtCount].X = (Gdiplus::REAL)dx;
			_pt[nPtCount].Y = (Gdiplus::REAL)dy;
			nPtCount++;
		}

		if (nPtCount > 360)
			nPtCount = 360;

		///////////////////////////////////////////
		_pt[1].X = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_1].X();
		_pt[1].Y = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_1].Y();

		_pt[nPtCount].X = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_2].X();
		_pt[nPtCount].Y = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_2].Y();

		nPtCount++;
		////////////////////////////////////////////

		if (temp_object->Property.bIsLine)
			g->DrawPolygon(&pen, _pt, nPtCount);

		if (temp_object->Property.bIsFill)
			g->FillPolygon(&brush, _pt, nPtCount);
	}

	void CObjectPieDraw::DrawRotatePie(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, UINT nResolution)
	{
		double a = 0.0;	// 장축.
		double b = 0.0;	// 단축.
		double ax = 0.0;
		double ay = 0.0;

		double cx = 0.0;
		double cy = 0.0;
		Gdiplus::PointF		_pt[362];
		UINT	nPtCount = 2;

		OVERLAY::ObjectPie* temp_object = (OVERLAY::ObjectPie*)object;

		Gdiplus::Pen		pen(temp_object->Property.LineColor);
		Gdiplus::SolidBrush	brush(temp_object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)temp_object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)temp_object->Property.eLineStyle);

		a = abs((double)(temp_object->pt[RIGHT_DOWN].X() - temp_object->pt[LEFT_UP].X())) / 2;
		b = abs((double)(temp_object->pt[RIGHT_DOWN].Y() - temp_object->pt[LEFT_UP].Y())) / 2;

		cx = (temp_object->pt[RIGHT_DOWN].X() + temp_object->pt[LEFT_UP].X()) / 2;
		cy = (temp_object->pt[RIGHT_DOWN].Y() + temp_object->pt[LEFT_UP].Y()) / 2;

		//부채꼴은 중점에서 그리기를 시작한다.
		_pt[0].X = (Gdiplus::REAL)cx;
		_pt[0].Y = (Gdiplus::REAL)cy;

		double dSweepAngle = temp_object->m_dSweepAngle[1];

		if (temp_object->m_dSweepAngle[0] > 360)
			temp_object->m_dSweepAngle[0] -= 360;
		else if (temp_object->m_dSweepAngle[0] < 0)
			temp_object->m_dSweepAngle[0] += 360;

		if (temp_object->m_dSweepAngle[1] > 360)
			temp_object->m_dSweepAngle[1] -= 360;
		else if (temp_object->m_dSweepAngle[1] < 0)
			temp_object->m_dSweepAngle[1] += 360;

		if (temp_object->m_dSweepAngle[0] > temp_object->m_dSweepAngle[1])
			dSweepAngle = (360 + temp_object->m_dSweepAngle[0] - (temp_object->m_dSweepAngle[0] - temp_object->m_dSweepAngle[1]));

		for (double i = temp_object->m_dSweepAngle[0]; i < dSweepAngle; i += nResolution)
		{
			GetEllipsePositon(a, b, cx, cy, i + 270, &ax, &ay);

			_pt[nPtCount].X = (Gdiplus::REAL)(ax * cos(Degree2Radian(temp_object->Property.dAngle)) - ay * sin(Degree2Radian(temp_object->Property.dAngle)) - cx * cos(Degree2Radian(temp_object->Property.dAngle)) + cy * sin(Degree2Radian(temp_object->Property.dAngle)) + cx);
			_pt[nPtCount].Y = (Gdiplus::REAL)(ax * sin(Degree2Radian(temp_object->Property.dAngle)) + ay * cos(Degree2Radian(temp_object->Property.dAngle)) - cx * sin(Degree2Radian(temp_object->Property.dAngle)) - cy * cos(Degree2Radian(temp_object->Property.dAngle)) + cy);

			nPtCount++;
		}

		if (nPtCount > 360)
			nPtCount = 360;

		///////////////////////////////////////////
		_pt[1].X = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_1].X();
		_pt[1].Y = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_1].Y();

		_pt[nPtCount].X = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_2].X();
		_pt[nPtCount].Y = (Gdiplus::REAL)object->pt_out_line[SWEEP_ANGLE_2].Y();

		nPtCount++;
		////////////////////////////////////////////

		if (temp_object->Property.bIsLine)
			g->DrawPolygon(&pen, _pt, nPtCount);

		if (temp_object->Property.bIsFill)
			g->FillPolygon(&brush, _pt, nPtCount);
	}
}

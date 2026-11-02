#include "pch.h"
#include "ObjectCurveDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectCurveDraw::CObjectCurveDraw()
	{
	}

	CObjectCurveDraw::~CObjectCurveDraw()
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
	void CObjectCurveDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		Gdiplus::Pen ppen(Gdiplus::Color(OUT_LINE_COLOR_A, OUT_LINE_COLOR_R, OUT_LINE_COLOR_G, OUT_LINE_COLOR_B));
		Gdiplus::PointF	temp_point_for_gdi[8];

		UINT i = 0;
		//---------------------------------------------
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		// 각 점의 편집 지점.
		for (i = 0; i < object->nPointIndex; i++)
		{
			INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
			g->FillEllipse(&brush_point, (INT)object->pt[i].X() - nSizeOfOutLinePoint, (INT)object->pt[i].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		}
		//----------------------------------------------

	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectCurveDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
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

		UINT i = 0;
		// 각 점의 편집 지점.
		SHORT nPointsOfFigure = 0;
		Gdiplus::SolidBrush brush_selected_point(Gdiplus::Color(255, 255, 255, 255));
		char ColorID[2];
		for (i = 0; i < object->nPointIndex; i++)
		{
			nPointsOfFigure = GENERAL_POINTS + i;
			memcpy(ColorID, &nPointsOfFigure, sizeof(ColorID));
			brush_selected_point.SetColor(Gdiplus::Color(255, 255, ColorID[1], ColorID[0]));	// 아웃라인과 겹치는 색은 배제함.				
			g->FillEllipse(&brush_selected_point, (INT)object->pt[i].X() - SIZE_OF_OUT_LINE_POINT / 2, (INT)object->pt[i].Y() - SIZE_OF_OUT_LINE_POINT / 2, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		}
	}
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawMap()
	// 목적				: 도형을 선택하기위한 메모리 멥 설정
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectCurveDraw::DrawMap(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DWORD	temp_color_line = object->Property.LineColor;
		double	temp_line_width = object->Property.dLineWidth;

		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;
		if (object->Property.bIsLine == TRUE)
		{
			//도형 선택을 위한 메모리
			object->Property.LineColor = 0xff000000 | (object->nID & 0xffff);
			Draw(object, g);
		}

		object->Property.LineColor = temp_color_line;
		object->Property.dLineWidth = temp_line_width;
	}
	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectCurveDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		if (object->Property.dAngle != 0)
			DrawRotateCurve(object, g);
		else
			DrawCurve(object, g);
	}
	// 8. 곡선
	void CObjectCurveDraw::DrawCurve(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->nPointIndex < 2)
			return;
		Gdiplus::PointF		pt[MAX_POINTS];
		Gdiplus::Pen		pen(object->Property.LineColor);

		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

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

		for (UINT i = 0; i < object->nPointIndex; i++)
		{
			pt[i].X = (Gdiplus::REAL)object->pt[i].X();
			pt[i].Y = (Gdiplus::REAL)object->pt[i].Y();
		}

		if (object->Property.bIsLine)
			g->DrawCurve(&pen, pt, object->nPointIndex, (Gdiplus::REAL)object->Property.dTension);
	}

	void CObjectCurveDraw::DrawRotateCurve(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->nPointIndex < 2)
			return;
		Gdiplus::PointF		pt[MAX_POINTS];

		Gdiplus::Pen		pen(object->Property.LineColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

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
			g->DrawCurve(&pen, pt, (INT)object->nPointIndex, (Gdiplus::REAL)object->Property.dTension);
	}
}
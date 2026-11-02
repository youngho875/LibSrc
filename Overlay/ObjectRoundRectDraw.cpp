#include "pch.h"
#include "ObjectRoundRectDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectRoundRectDraw::CObjectRoundRectDraw()
	{
	}

	CObjectRoundRectDraw::~CObjectRoundRectDraw()
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
	void CObjectRoundRectDraw::DrawOutLine(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		if (object->pt[0].X() > object->pt[1].X())
			return;

		object->SetOutLine();

		Gdiplus::Pen ppen(Gdiplus::Color(OUT_LINE_COLOR_A, OUT_LINE_COLOR_R, OUT_LINE_COLOR_G, OUT_LINE_COLOR_B));
		Gdiplus::SolidBrush brush_point(Gdiplus::Color(OUT_LINE_POINT_COLOR_A, OUT_LINE_POINT_COLOR_R, OUT_LINE_POINT_COLOR_G, OUT_LINE_POINT_COLOR_B));

		ppen.SetWidth(EDIT_LINE_WIDTH);

		Gdiplus::PointF temp_point_for_gdi[8];
		for (int i = 0; i < 8; i++)
		{
			temp_point_for_gdi[i].X = (Gdiplus::REAL)object->pt_out_line[i].X();
			temp_point_for_gdi[i].Y = (Gdiplus::REAL)object->pt_out_line[i].Y();
		}

		g->DrawPolygon(&ppen, temp_point_for_gdi, 8);
		g->DrawLine(&ppen, (INT)object->pt_out_line[ROTATE_CENTER].X(), (INT)object->pt_out_line[ROTATE_CENTER].Y(), (INT)object->pt_out_line[TOP_CENTER].X(), (INT)object->pt_out_line[TOP_CENTER].Y());

		INT nSizeOfOutLinePoint = SIZE_OF_OUT_LINE_POINT / 2;
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[ROTATE_CENTER].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[ROTATE_CENTER].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[LEFT_TOP].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[LEFT_TOP].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[TOP_CENTER].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[TOP_CENTER].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[RIGHT_TOP].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[RIGHT_TOP].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[LEFT_CENTER].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[LEFT_CENTER].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[RIGHT_CENTER].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[RIGHT_CENTER].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[LEFT_BOTTOM].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[LEFT_BOTTOM].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[RIGHT_BOTTOM].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[RIGHT_BOTTOM].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
		g->FillEllipse(&brush_point, (INT)object->pt_out_line[BOTTOM_CENTER].X() - nSizeOfOutLinePoint, (INT)object->pt_out_line[BOTTOM_CENTER].Y() - nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);

		brush_point.SetColor(Gdiplus::Color(OUT_LINE_POINT_OPTION_COLOR_A, OUT_LINE_POINT_OPTION_COLOR_R, OUT_LINE_POINT_OPTION_COLOR_B, OUT_LINE_POINT_OPTION_COLOR_B));
		//g->FillEllipse(&brush_point,(INT)object->pt_out_line[SWEEP_POSITION].X()-nSizeOfOutLinePoint, (INT)object->pt_out_line[SWEEP_POSITION].Y()-nSizeOfOutLinePoint, SIZE_OF_OUT_LINE_POINT, SIZE_OF_OUT_LINE_POINT);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawSelectPoint()
	// 목적				: 도형 편집영역 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectRoundRectDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		if (object->pt[0].X() > object->pt[1].X())
			return;

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
	void CObjectRoundRectDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		if (object->pt[0].X() > object->pt[1].X())
			return;

		if (object->Property.dAngle != 0)
			DrawRotateRoundRect(object, g);
		else
			DrawRoundRect(object, g);
	}

	void CObjectRoundRectDraw::DrawRoundRect(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		// 	if(object->pt[0].X() >object->pt[1].X())
		// 		return;

		Gdiplus::PointF		pt[360];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		double cx = 0.0;
		double cy = 0.0;
		double qt = 0.0;// 1/4
		double qt_y = 0.0;// 1/4

		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;
		//TRACE("cx = %f \t cy = %f\n",cx,cy);
		//TRACE("pt[0].X =%f \t pt[1].X=%f \n",object->pt[0].X(),object->pt[1].X());


		double tempX = cx;
		double tempY = cy;

		// 	tempX = abs(tempX-object->pt[LEFT_UP].X());
		//  	tempY = abs(tempY-object->pt[LEFT_UP].Y());
		tempX -= object->pt[LEFT_UP].X();
		tempY -= object->pt[LEFT_UP].Y();
		//TRACE("tempX = %f  tempY = %f\n",tempX,tempY);
	// 	if(tempX <0){ //tempX 가 0보다 작으면 x위치 바꾼다. 
	// 		double tempChangeX=0;
	// 		tempChangeX = object->pt[0].X();
	// 		object->pt[0].SetXY(object->pt[1].X(),object->pt[0].Y());
	// 		object->pt[1].SetXY(tempChangeX,object->pt[1].Y());
	// 	}



		//tempX =abs(tempX);
		//tempY =abs(tempY);

		//TRACE("tempx =%f  tempy=%f\n",object->pt[LEFT_UP].X(),object->pt[LEFT_UP].Y());
		if (tempX > tempY)
			object->pt[ROUND_RECT_VERTEX].SetXY(object->pt[0].X() + abs(tempY), object->pt[0].Y());
		else
			object->pt[ROUND_RECT_VERTEX].SetXY(object->pt[0].X() + abs(tempX), object->pt[0].Y());
		//	TRACE("object->pt[0].X =%f \t object->pt[0].Y=%f \t object->pt[1].X=%f \t object->pt[1].Y=%f\n",object->pt[0].X(),object->pt[0].Y(),object->pt[1].X(),object->pt[1].Y());


			//gap을 기본 항상 최대값 처리?
		double gap = abs(object->pt[0].X() - object->pt[ROUND_RECT_VERTEX].X());
		//TRACE("gap = %d\n",(int)gap);

		double cx_ = object->pt[ROUND_RECT_VERTEX].X();
		double cy_ = (object->pt[ROUND_RECT_VERTEX].Y() + gap);
		//TRACE("tempx =%f  tempy=%f\n",cx_,cy_);

		double dRadius = 0.0;
		dRadius = gap;

		UINT nCount = 0;
		double dx = 0;
		double dy = 0;
		// 1. 
		int i = 0;
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 180),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			//TRACE("X =%6.2f \t Y =%6.2f\n",pt[nCount].X,pt[nCount].Y);


			nCount++;
		}

		// 2.
		cx_ = object->pt[RIGHT_DOWN].X() - gap;
		cy_ = (object->pt[LEFT_UP].Y() + gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 270),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		//// 3.
		cx_ = object->pt[RIGHT_DOWN].X() - gap;
		cy_ = (object->pt[RIGHT_DOWN].Y() - gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		// 4.
		cx_ = object->pt[LEFT_UP].X() + gap;
		cy_ = (object->pt[RIGHT_DOWN].Y() - gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 90),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, 360);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 360);
	}

	void CObjectRoundRectDraw::DrawRotateRoundRect(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::PointF		pt[360];

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		double cx = 0.0;
		double cy = 0.0;
		double qt = 0.0;// 1/4
		double qt_y = 0.0;// 1/4

		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;
		//--------------------------------------------------------------------------------------
		double tempX = cx;
		double tempY = cy;
		tempX -= object->pt[LEFT_UP].X();
		tempY -= object->pt[LEFT_UP].Y();
		if (tempX > tempY)
			object->pt[ROUND_RECT_VERTEX].SetXY(object->pt[0].X() + tempY, object->pt[0].Y());
		else
			object->pt[ROUND_RECT_VERTEX].SetXY(object->pt[0].X() + tempX, object->pt[0].Y());

		//---------------------------------------------------------------


		double gap = abs(object->pt[0].X() - object->pt[ROUND_RECT_VERTEX].X());

		double cx_ = object->pt[ROUND_RECT_VERTEX].X();
		double cy_ = (object->pt[ROUND_RECT_VERTEX].Y() + gap);


		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;



		double dRadius = 0.0;
		dRadius = gap;
		double dx = 0;
		double dy = 0;

		UINT nCount = 0;
		// 1.
		UINT i = 0;
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 180),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		// 2.
		cx_ = object->pt[RIGHT_DOWN].X() - gap;
		cy_ = (object->pt[LEFT_UP].Y() + gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 270),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		//// 3.
		cx_ = object->pt[RIGHT_DOWN].X() - gap;
		cy_ = (object->pt[RIGHT_DOWN].Y() - gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		// 4.
		cx_ = object->pt[LEFT_UP].X() + gap;
		cy_ = (object->pt[RIGHT_DOWN].Y() - gap);
		for (i = 0; i < 90; i++)
		{
			ReferencePositon_Radius_Angle2TargetPosition
			(dRadius,
				(i + 90),
				0,
				0,
				&dx,
				&dy
			);

			pt[nCount].X = (Gdiplus::REAL)(dx + cx_);
			pt[nCount].Y = (Gdiplus::REAL)(dy + cy_);
			nCount++;
		}

		double dDistance = 0.0;
		double dAngle = 0.0;

		for (i = 0; i < 360; i++)
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
			g->DrawPolygon(&pen, pt, 360);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 360);
	}
}
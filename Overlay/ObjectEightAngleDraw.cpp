#include "pch.h"
#include "ObjectEightAngleDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectEightAngleDraw::CObjectEightAngleDraw()
	{
	}

	CObjectEightAngleDraw::~CObjectEightAngleDraw()
	{
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectEightAngleDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		if (object->Property.dAngle != 0)
			DrawRotateEightAngle(object, g);
		else
			DrawEightAngle(object, g);

	}

	// 7. 팔각형
	void CObjectEightAngleDraw::DrawEightAngle(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::PointF		pt[8];

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

		qt = (object->pt[RIGHT_DOWN].X() - object->pt[LEFT_UP].X()) / 4;
		qt_y = (object->pt[RIGHT_DOWN].Y() - object->pt[LEFT_UP].Y()) / 4;

		pt[0].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[0].Y = (Gdiplus::REAL)(cy + qt_y);

		pt[1].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[1].Y = (Gdiplus::REAL)(cy - qt_y);

		pt[2].X = (Gdiplus::REAL)(cx - qt);
		pt[2].Y = (Gdiplus::REAL)object->pt[LEFT_UP].Y();

		pt[3].X = (Gdiplus::REAL)(cx + qt);
		pt[3].Y = (Gdiplus::REAL)object->pt[LEFT_UP].Y();

		pt[4].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[4].Y = (Gdiplus::REAL)(cy - qt_y);

		pt[5].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[5].Y = (Gdiplus::REAL)(object->pt[RIGHT_DOWN].Y() - qt_y);

		pt[6].X = (Gdiplus::REAL)(cx + qt);
		pt[6].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[7].X = (Gdiplus::REAL)(cx - qt);
		pt[7].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, 8);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 8);
	}

	void CObjectEightAngleDraw::DrawRotateEightAngle(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		Gdiplus::PointF		pt[8];

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

		qt = (object->pt[RIGHT_DOWN].X() - object->pt[LEFT_UP].X()) / 4;
		qt_y = (object->pt[RIGHT_DOWN].Y() - object->pt[LEFT_UP].Y()) / 4;

		pt[0].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[0].Y = (Gdiplus::REAL)(cy + qt_y);

		pt[1].X = (Gdiplus::REAL)object->pt[LEFT_UP].X();
		pt[1].Y = (Gdiplus::REAL)(cy - qt_y);

		pt[2].X = (Gdiplus::REAL)(cx - qt);
		pt[2].Y = (Gdiplus::REAL)object->pt[LEFT_UP].Y();

		pt[3].X = (Gdiplus::REAL)(cx + qt);
		pt[3].Y = (Gdiplus::REAL)object->pt[LEFT_UP].Y();

		pt[4].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[4].Y = (Gdiplus::REAL)(cy - qt_y);

		pt[5].X = (Gdiplus::REAL)object->pt[RIGHT_DOWN].X();
		pt[5].Y = (Gdiplus::REAL)(object->pt[RIGHT_DOWN].Y() - qt_y);

		pt[6].X = (Gdiplus::REAL)(cx + qt);
		pt[6].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		pt[7].X = (Gdiplus::REAL)(cx - qt);
		pt[7].Y = (Gdiplus::REAL)object->pt[RIGHT_DOWN].Y();

		double dDistance = 0.0;
		double dAngle = 0.0;
		double dx = 0;
		double dy = 0;

		for (int i = 0; i < 8; i++)
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

			dAngle = acos(((double)(cy - (Gdiplus::REAL)pt[i].Y)) / dDistance);
			dAngle = Radian2Degree(dAngle);

			if (cx > (Gdiplus::REAL)pt[i].X)
				dAngle *= -1;

			ReferencePositon_Distance_Angle2TargetPosition(dDistance, object->Property.dAngle + (dAngle)-90, 0, 0, &dx, &dy);
			pt[i].X = (Gdiplus::REAL)(dx + cx);
			pt[i].Y = (Gdiplus::REAL)(dy + cy);
		}

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, pt, 8);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, pt, 8);
	}

	void CObjectEightAngleDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
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

		CObjectDraw::DrawSelectPoint(object, g);
	}
}
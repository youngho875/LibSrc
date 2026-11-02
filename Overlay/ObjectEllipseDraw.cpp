#include "pch.h"
#include "ObjectEllipseDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectEllipseDraw::CObjectEllipseDraw()
	{
	}

	CObjectEllipseDraw::~CObjectEllipseDraw()
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
	void CObjectEllipseDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		if (object->Property.dAngle != 0)
		{
			DrawRotateEllipse(object, g), nResolution;
		}
		else
			DrawEllipse(object, g, nResolution);
	}

	//--------------------------------------------
	// 1. 타원
	//--------------------------------------------
	void CObjectEllipseDraw::DrawRotateEllipse(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		double a = 0.0;	// 장축.
		double b = 0.0;	// 단축.
		double ax = 0.0;
		double ay = 0.0;

		double cx = 0.0;
		double cy = 0.0;
		Gdiplus::PointF	_pt[360];
		UINT	nPtCount = 0;

		Gdiplus::Pen pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		a = abs((double)(object->pt[RIGHT_DOWN].X() - object->pt[LEFT_UP].X())) / 2;
		b = abs((double)(object->pt[RIGHT_DOWN].Y() - object->pt[LEFT_UP].Y())) / 2;

		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;

		for (double i = 0; i < 360; i += nResolution)
		{
			GetEllipsePositon(a, b, cx, cy, i, &ax, &ay);

			_pt[nPtCount].X = (Gdiplus::REAL)(ax * cos(Degree2Radian(object->Property.dAngle)) - ay * sin(Degree2Radian(object->Property.dAngle)) - cx * cos(Degree2Radian(object->Property.dAngle)) + cy * sin(Degree2Radian(object->Property.dAngle)) + cx);
			_pt[nPtCount].Y = (Gdiplus::REAL)(ax * sin(Degree2Radian(object->Property.dAngle)) + ay * cos(Degree2Radian(object->Property.dAngle)) - cx * sin(Degree2Radian(object->Property.dAngle)) - cy * cos(Degree2Radian(object->Property.dAngle)) + cy);

			nPtCount++;
		}

		if (nPtCount > 360)
			nPtCount = 360;

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, _pt, nPtCount);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, _pt, nPtCount);
	}

	void CObjectEllipseDraw::DrawEllipse(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		double a = 0.0;	// 장축.
		double b = 0.0;	// 단축.
		double cx = 0.0;
		double cy = 0.0;
		Gdiplus::PointF		_pt[360];
		UINT	nPtCount = 0;

		Gdiplus::Pen		pen(object->Property.LineColor);
		Gdiplus::SolidBrush	brush(object->Property.FillColor);
		pen.SetWidth((Gdiplus::REAL)object->Property.dLineWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)object->Property.eLineStyle);

		a = abs((double)(object->pt[RIGHT_DOWN].X() - object->pt[LEFT_UP].X())) / 2;
		b = abs((double)(object->pt[RIGHT_DOWN].Y() - object->pt[LEFT_UP].Y())) / 2;

		cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;

		double dx = 0;
		double dy = 0;
		for (double i = 0; i < 360; i += nResolution)
		{
			GetEllipsePositon(a, b, cx, cy, i, &dx, &dy);
			_pt[nPtCount].X = (Gdiplus::REAL)dx;
			_pt[nPtCount].Y = (Gdiplus::REAL)dy;
			nPtCount++;
		}

		if (nPtCount > 360)
			nPtCount = 360;

		if (object->Property.bIsLine)
			g->DrawPolygon(&pen, _pt, nPtCount);

		if (object->Property.bIsFill)
			g->FillPolygon(&brush, _pt, nPtCount);
	}

	void CObjectEllipseDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
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
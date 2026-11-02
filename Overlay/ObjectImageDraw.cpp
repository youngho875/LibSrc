#include "pch.h"
#include "ObjectImageDraw.h"
#include "ObjectImage.h"
#include "math.h"
#include "ObjectRectDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectImageDraw::CObjectImageDraw()
	{
	}

	CObjectImageDraw::~CObjectImageDraw()
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
	void CObjectImageDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{

		DrawImage(object, g);
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawMap()
	// 목적				: 도형을 선택하기위한 메모리 멥 설정
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectImageDraw::DrawMap(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DWORD	temp_color_line = object->Property.LineColor;
		DWORD	temp_color_fill = object->Property.FillColor;
		double	temp_line_width = object->Property.dLineWidth;

		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;

		object->Property.LineColor = 0xff000000 | (object->nID & 0xffff);
		object->Property.FillColor = 0xff000000 | (object->nID & 0xffff);

		CObjectRectDraw DrawRect;

		DrawRect.Draw(object, g);
	}
	// 14. 이미지
	void CObjectImageDraw::DrawImage(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		OVERLAY::ObjectImage* pObject = (OVERLAY::ObjectImage*)object;
		if (pObject == NULL)
			return;
		WCHAR IamageFileName[512];
		memset(IamageFileName, 0, sizeof(IamageFileName));

		UINT nStrLen = strlen(pObject->m_strImageFileName);
		if (nStrLen >= 512)
		{
			AfxMessageBox(_T("파일경로가 너무 길어 읽을 수 없습니다.\n파일을 이동시켜주세요."));
			return;
		}

		MultiByteToWideChar(CP_ACP, 0, pObject->m_strImageFileName, nStrLen, IamageFileName, nStrLen);
		Gdiplus::Image image(IamageFileName);
		Gdiplus::RectF rect;

		double cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2;
		double cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2;

		// 이미지 회전.
		if (object->Property.dAngle != 0)
		{
			Gdiplus::Matrix transformMatrix;
			transformMatrix.Translate(+cx, +cy);
			transformMatrix.Rotate(object->Property.dAngle);
			g->SetTransform(&transformMatrix);

			rect.X = object->pt[LEFT_UP].X() - cx;
			rect.Y = object->pt[LEFT_UP].Y() - cy;
			rect.Width = object->pt[RIGHT_DOWN].X() - cx - rect.X;
			rect.Height = object->pt[RIGHT_DOWN].Y() - cy - rect.Y;

			g->DrawImage(&image, rect);

			transformMatrix.Rotate(-object->Property.dAngle);
			transformMatrix.Translate(-cx, -cy);
			g->SetTransform(&transformMatrix);
		}
		else
		{
			rect.X = object->pt[LEFT_UP].X();
			rect.Y = object->pt[LEFT_UP].Y();
			rect.Width = object->pt[RIGHT_DOWN].X() - rect.X;
			rect.Height = object->pt[RIGHT_DOWN].Y() - rect.Y;
			g->DrawImage(&image, rect);
		}
	}

	void CObjectImageDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		COLORREF	temp_fill_color = object->Property.FillColor;
		COLORREF	temp_line_color = object->Property.LineColor;
		double		temp_line_width = object->Property.dLineWidth;

		object->Property.FillColor = 0xffff0000 | OUT_LINE_SELECT;
		object->Property.LineColor = 0xffff0000 | OUT_LINE_SELECT;
		if (object->Property.dLineWidth < EDIT_SELECT_LINE_WIDTH)
			object->Property.dLineWidth = EDIT_SELECT_LINE_WIDTH;

		CObjectRectDraw DrawRect;
		DrawRect.Draw(object, g);

		object->Property.FillColor = temp_fill_color;
		object->Property.LineColor = temp_line_color;
		object->Property.dLineWidth = temp_line_width;

		CObjectDraw::DrawSelectPoint(object, g);
	}
}
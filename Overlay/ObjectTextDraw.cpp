#include "pch.h"
#include "ObjectText.h"
#include "ObjectTextDraw.h"
#include "ObjectRectDraw.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectTextDraw::CObjectTextDraw()
	{
	}

	CObjectTextDraw::~CObjectTextDraw()
	{
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: DrawMap()
	// 목적				: 도형을 선택하기위한 메모리 멥 설정
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectTextDraw::DrawMap(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		//도형 선택을 위한 메모리
		COLORREF tempColor = object->Property.FillColor;

		object->Property.FillColor = 0xff000000 | (object->nID & 0xffff);
		CObjectRectDraw	DrawRect;
		DrawRect.Draw(object, g);

		object->Property.FillColor = tempColor;
	}

	//---------------------------------------
	// 작성자			: 유영호
	// 함수명			: Draw()
	// 목적				: 도형 그리기
	// 생성일			: 2024-10-07
	// 최종수정자		:
	// 최종수정일		:
	//---------------------------------------
	void CObjectTextDraw::Draw(OVERLAY::OBJECT* object, Gdiplus::Graphics* g, int nResolution)
	{
		DrawText(object, g);
	}

	// 15. 텍스트
	void CObjectTextDraw::DrawText(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		OVERLAY::ObjectText* pObejct = (OVERLAY::ObjectText*)object;

		pObejct->m_dFontSize = (double)abs(pObejct->pt[RIGHT_DOWN].Y() - pObejct->pt[LEFT_UP].Y()) * FONT_RESOLUTION;

		Gdiplus::Font genericMonoFont(
			strFamilyName[pObejct->m_eFamilyName],
			(Gdiplus::REAL)pObejct->m_dFontSize,
			pObejct->m_eFontStyle,
			pObejct->m_eUnit
		);

		Gdiplus::SolidBrush solidbrush(pObejct->m_FontColor);
		UINT nStrLength = wcslen(pObejct->m_Text);// 수정 필요.

		// TEXT 회전.
		if (object->Property.dAngle != 0)
		{
			Gdiplus::Matrix transformMatrix;
			double cx = (object->pt[RIGHT_DOWN].X() + object->pt[LEFT_UP].X()) / 2.0;
			double cy = (object->pt[RIGHT_DOWN].Y() + object->pt[LEFT_UP].Y()) / 2.0;

			transformMatrix.Translate(cx, cy);
			transformMatrix.Rotate(object->Property.dAngle);
			g->SetTransform(&transformMatrix);

			g->DrawString(
				pObejct->m_Text,
				nStrLength,
				&genericMonoFont,
				Gdiplus::PointF((Gdiplus::REAL)(object->pt[LEFT_UP].X() - cx), (Gdiplus::REAL)(object->pt[LEFT_UP].Y() - cy)),
				&solidbrush
			);

			transformMatrix.Rotate(-object->Property.dAngle);
			transformMatrix.Translate(-cx, -cy);
			g->SetTransform(&transformMatrix);
		}
		else
		{
			g->DrawString(
				pObejct->m_Text,
				nStrLength,
				&genericMonoFont,
				Gdiplus::PointF((Gdiplus::REAL)pObejct->pt[0].X(), (Gdiplus::REAL)pObejct->pt[0].Y()),
				&solidbrush
			);
		}
	}

	void CObjectTextDraw::DrawSelectPoint(OVERLAY::OBJECT* object, Gdiplus::Graphics* g)
	{
		OVERLAY::ObjectText* pObejct = (OVERLAY::ObjectText*)object;
		COLORREF temp_font_color = pObejct->m_FontColor;

		pObejct->m_FontColor = 0xffff0000 | OUT_LINE_SELECT;
		Draw(object, g);
		pObejct->m_FontColor = temp_font_color;

		CObjectDraw::DrawSelectPoint(object, g);
	}
}
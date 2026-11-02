#include "pch.h"
#include "DrawGridField.h"
#include "ObjectPoint.h"
#include "../GeoDC/CoordMgr.h"
#include "../GeoDC/Coord.h"
#include "../../inc/Useful.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawGridField::DrawGridField(Gdiplus::Graphics& g, long lw, long lh)
		:BMP(lw, lh, &g)
		, GRP(&BMP)
		, m_nIsLatLong(0)
	{
		m_bIsFirst = FALSE;
		dCenterWorldX = 0.0;
		dCenterWorldY = 0.0;
		dLeftWorldX = 0.0;
		dLeftWorldY = 0.0;
		dRightWorldX = 0.0;
		dRightWorldY = 0.0;
		dTopWorldX = 0.0;
		dTopWorldY = 0.0;
		dBottomWorldX = 0.0;
		dBottomWorldY = 0.0;
		dScreenX = 0.0;
		dScreenY = 0.0;

	}

	DrawGridField::~DrawGridField()
	{

	}

	void DrawGridField::Clear()
	{
		GRP.Clear(Gdiplus::Color(0, 0, 0, 0));
	}

	void DrawGridField::ClearColor(Gdiplus::Color color)
	{
		GRP.Clear(color);
	}

	Gdiplus::Bitmap* DrawGridField::GetImage()
	{
		return &BMP;
	}

	void DrawGridField::DrawGrid(HWND hWnd, UINT nDetail)
	{
		if (nDetail < 1)
			return;

		Clear();

		RECT rect;
		GetClientRect(hWnd, &rect);

		CCoordMgr* pCoord = Point::g_pCoord;
		CCoord	coord;

		Gdiplus::Pen ppen(Gdiplus::Color(255, 0, 127, 0));

		// 	double dCenterWorldX = 0.0;
		// 	double dCenterWorldY = 0.0;
		// 	double dLeftWorldX = 0.0;
		// 	double dLeftWorldY = 0.0;
		// 	double dRightWorldX = 0.0;
		// 	double dRightWorldY = 0.0;
		// 	double dTopWorldX = 0.0;
		// 	double dTopWorldY = 0.0;
		// 	double dBottomWorldX = 0.0;
		// 	double dBottomWorldY = 0.0;
		// 	double dScreenX = 0.0;
		// 	double dScreenY = 0.0;

		if (!m_bIsFirst)
		{

			//pCoord->m_nDisplayViewWidth/2;
			//pCoord->m_nDisplayViewHeight/2
			m_bIsFirst = TRUE;
			//rect.bottom =922;
			rect.bottom = pCoord->m_nDisplayViewHeight;
			rect.left = 0;
			//rect.right =1276;
			rect.right = pCoord->m_nDisplayViewWidth;
			rect.top = 0;
			pCoord->ScreenXY2WorldXY(rect.right / 2, rect.bottom / 2, &dCenterWorldX, &dCenterWorldY);
			pCoord->ScreenXY2WorldXY(rect.left, rect.bottom / 2, &dLeftWorldX, &dLeftWorldY);
			pCoord->ScreenXY2WorldXY(rect.right, rect.bottom / 2, &dRightWorldX, &dRightWorldY);
			pCoord->ScreenXY2WorldXY(rect.right / 2, rect.top, &dTopWorldX, &dTopWorldY);
			pCoord->ScreenXY2WorldXY(rect.right / 2, rect.bottom, &dBottomWorldX, &dBottomWorldY);


			dCenterWorldX = 0.00000000000000;
			dCenterWorldY = 0.00000000000000;
			dLeftWorldX = -531.66666666667;
			dLeftWorldY = 0.00000000000000;
			dRightWorldX = 531.66666666667;
			dRightWorldY = 0.00000000000000;
			dTopWorldX = 0.00000000000000;
			dTopWorldY = 384.16666666667;
			dBottomWorldX = 0.00000000000000;
			dBottomWorldY = -384.16666666667;
		}


		double i = 0;
		UINT nXIndex = 0;
		UINT nYIndex = 0;
		int* pBufferX = new int[rect.right];
		int* pBufferY = new int[rect.bottom];
		//X축
		long nX = 0, nY = 0;

		for (i = dCenterWorldY; i <= dTopWorldY; i += nDetail)
		{
			pCoord->WorldXY2ScreenXY(dLeftWorldX, i, &nX, &nY);
			GRP.DrawLine(&ppen, 0, nY, rect.right, nY);
			pBufferY[nYIndex] = nY;
			nYIndex++;
		}

		for (i = dCenterWorldY; i > dBottomWorldY; i -= nDetail)
		{
			pCoord->WorldXY2ScreenXY(dLeftWorldX, i, &nX, &nY);
			GRP.DrawLine(&ppen, 0, nY, rect.right, nY);
			pBufferY[nYIndex] = nY;
			nYIndex++;
		}

		//Y축
		for (i = dCenterWorldX; i <= dRightWorldX; i += nDetail)
		{
			pCoord->WorldXY2ScreenXY(i, dCenterWorldY, &nX, &nY);
			GRP.DrawLine(&ppen, nX, 0, nX, rect.bottom);
			pBufferX[nXIndex] = nX;
			nXIndex++;
		}

		for (i = dCenterWorldX; i > dLeftWorldX; i -= nDetail)
		{
			pCoord->WorldXY2ScreenXY(i, dCenterWorldY, &nX, &nY);
			GRP.DrawLine(&ppen, nX, 0, nX, rect.bottom);
			pBufferX[nXIndex] = nX;
			nXIndex++;
		}

		// Draw Coord info
		Gdiplus::Font genericMonoFont(
			L"굴림",
			(Gdiplus::REAL)8,
			Gdiplus::FontStyleRegular,
			Gdiplus::UnitPoint
		);

		Gdiplus::SolidBrush solidbrush(Gdiplus::Color(255, 255, 127, 255));

		WCHAR coord_string[128];
		char coord_string_[128];
		UINT nStrLength = 0;// 수정 필요.

		double wx = 0;
		double wy = 0;
		double lat = 0, lng = 0;
		UINT pos_X = 0;
		UINT pos_Y = 0;

		for (pos_X = 0; pos_X < nXIndex; pos_X++)
		{
			pCoord->ScreenXY2WorldXY((int)pBufferX[pos_X], (int)pBufferY[0], &wx, &wy);

			memset(coord_string_, 0, sizeof(coord_string_));
			memset(coord_string, 0, sizeof(coord_string));

			if (m_nIsLatLong > 1)
			{
				coord.x_y_to_radians_lat_long(wx, wy, &lat, &lng);
				sprintf_s(coord_string_, "%0.2f", lat * 180 / 3.11592);
			}
			else
				sprintf_s(coord_string_, "%0.2f", wx);
			MultiByteToWideChar(CP_ACP, 0, coord_string_, strlen(coord_string_), coord_string, strlen(coord_string_));

			nStrLength = wcslen(coord_string);
			GRP.DrawString(
				coord_string,
				nStrLength,
				&genericMonoFont,
				Gdiplus::PointF((Gdiplus::REAL)pBufferX[pos_X], (Gdiplus::REAL)pBufferY[0]),
				&solidbrush
			);

		}

		for (pos_Y = 0; pos_Y < nYIndex; pos_Y++)
		{
			pCoord->ScreenXY2WorldXY((int)pBufferX[0], (int)pBufferY[pos_Y], &wx, &wy);

			memset(coord_string_, 0, sizeof(coord_string_));
			memset(coord_string, 0, sizeof(coord_string));

			if (m_nIsLatLong > 1)
			{
				coord.x_y_to_radians_lat_long(wx, wy, &lat, &lng);
				sprintf_s(coord_string_, "%0.2f", lng * 180 / 3.11592);
			}
			else
				sprintf_s(coord_string_, "%0.2f", wy);
			MultiByteToWideChar(CP_ACP, 0, coord_string_, strlen(coord_string_), coord_string, strlen(coord_string_));

			nStrLength = wcslen(coord_string);
			GRP.DrawString(
				coord_string,
				nStrLength,
				&genericMonoFont,
				Gdiplus::PointF((Gdiplus::REAL)pBufferX[0], (Gdiplus::REAL)pBufferY[pos_Y] + 9),
				&solidbrush
			);
		}

		SAFE_DELETE_ARRAY(pBufferX);
		SAFE_DELETE_ARRAY(pBufferY);
	}
}
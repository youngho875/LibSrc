#include "pch.h"
#include "DrawBasicField.h"

#include "../GeoDC/Coordmgr.h"
#include "../GeoDC/Coord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	DrawBasicField::DrawBasicField(Gdiplus::Graphics& g, long lw, long lh)
		:BMP(lw, lh, &g)
		, GRP(&BMP)
	{

	}

	DrawBasicField::~DrawBasicField()
	{

	}

	void DrawBasicField::Clear()
	{
		GRP.Clear(Gdiplus::Color(0, 0, 0, 0));
	}

	void DrawBasicField::ClearColor(Gdiplus::Color color)
	{
		GRP.Clear(color);
	}

	void DrawBasicField::DrawImages(Gdiplus::Bitmap& bitmap)
	{
		GRP.DrawImage(&bitmap, 0, 0);
	}

	Gdiplus::Bitmap* DrawBasicField::GetImage()
	{
		return &BMP;
	}

	HDC DrawBasicField::GetHDC()
	{
		return GRP.GetHDC();
	}
}
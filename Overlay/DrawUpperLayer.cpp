#include "pch.h"
#include "DrawUpperLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawUpperLayer::DrawUpperLayer(Gdiplus::Graphics& g, long lw, long lh)
		:DrawField(g, lw, lh)
	{
	}

	DrawUpperLayer::~DrawUpperLayer()
	{

	}

	BOOL DrawUpperLayer::Draw(OVERLAY::OBJECT* pObject)
	{
		return DrawField::Draw(pObject);
	}
}
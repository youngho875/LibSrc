#include "pch.h"
#include "ObjectRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectRect::ObjectRect()
		:OBJECT(RECT_TYPE, POINTS_OF_OBJECT_RECT)
	{
	}

	ObjectRect::~ObjectRect()
	{
	}
}
#include "pch.h"
#include "ObjectLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectLine::ObjectLine()
		:OBJECT(LINE, POINTS_OF_OBJECT_LINE)
	{
		Property.bIsFill = FALSE;
	}

	ObjectLine::~ObjectLine()
	{
	}
}
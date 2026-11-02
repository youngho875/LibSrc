#include "pch.h"
#include "ObjectPentagon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectPentagon::ObjectPentagon()
		:OBJECT(PENTAGON, POINTS_OF_OBJECT_PENTAGON)
	{
	}

	ObjectPentagon::~ObjectPentagon()
	{
	}
}
#include "pch.h"
#include "ObjectImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectImage::ObjectImage(char* strImageFileName, int nStrImageFileNameLen)
		:OBJECT(IMAGE, POINTS_OF_OBJECT_IMAGE)
	{
		memset(m_strImageFileName, 0, sizeof(m_strImageFileName));
		memcpy(m_strImageFileName, strImageFileName, nStrImageFileNameLen);
	}

	ObjectImage::~ObjectImage()
	{
	}
}
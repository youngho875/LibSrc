#include "pch.h"
#include "ObjectGroupMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectGroupMgr::CObjectGroupMgr()
	{
	}

	CObjectGroupMgr::~CObjectGroupMgr()
	{
	}

	BOOL CObjectGroupMgr::AddObjectInGroup(UINT nGroupID, UINT nObjectID)
	{
		if (nGroupID >= OBJECT_PER_LAYER)
			return FALSE;
		m_ObjectGroupVector[nGroupID].push_back(nObjectID);
		return TRUE;
	}
}
#include "pch.h"
#include "OverlayDefine.h"
#include "ObjectGroupInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	CObjectGroupInfo::CObjectGroupInfo()
	{
		m_MaxObjectInGroup = OBJECT_PER_LAYER;
		m_ObjectArray = new UINT[m_MaxObjectInGroup];
		if (m_ObjectArray == NULL)
		{
			m_MaxObjectInGroup = 0;
			AfxMessageBox(_T("도형 그룹정보 초기화 실패"));
		}
		else
			memset(m_ObjectArray, 0, sizeof(UINT) * m_MaxObjectInGroup);

		m_nIndex = 0;
	}


	CObjectGroupInfo::CObjectGroupInfo(UINT nMaxSize)
	{
		m_MaxObjectInGroup = nMaxSize;
		m_ObjectArray = new UINT[m_MaxObjectInGroup];
		if (m_ObjectArray == NULL)
		{
			m_MaxObjectInGroup = 0;
			AfxMessageBox(_T("도형 그룹정보 초기화 실패"));
		}
		else
			memset(m_ObjectArray, 0, sizeof(UINT) * m_MaxObjectInGroup);

		m_nIndex = 0;
	}

	CObjectGroupInfo::~CObjectGroupInfo()
	{
		if (m_ObjectArray != NULL)
		{
			delete[] m_ObjectArray;
			m_ObjectArray = NULL;
		}
	}

	UINT CObjectGroupInfo::Size()
	{
		return m_nIndex;
	}

	BOOL CObjectGroupInfo::AddObjectInGroup(UINT nID)
	{
		if (m_MaxObjectInGroup < 1)
			return FALSE;

		for (int i = 0; i < m_nIndex; i++)
		{
			if (m_ObjectArray[i] == nID)
				return TRUE;
		}
		m_ObjectArray[m_nIndex] = nID;
		m_nIndex++;
		return TRUE;
	}

	UINT CObjectGroupInfo::GetObjectID(UINT nIndex)
	{
		if (m_MaxObjectInGroup < 1)
			return 0;

		if (m_nIndex <= nIndex)
			return 0;
		else
			return m_ObjectArray[nIndex];
	}
}
#include "pch.h"
#include "memory.h"
#include "OverlayEventMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	COverlayEvent::COverlayEvent()
	{
		nFlags = 0;
		e_evnet = OVERLAY_MOUSE_INVALID;
		point.x = 0;
		point.y = 0;
		eObjectType = OVERLAY::OBJECT_TYPE::NULL_TYPE;
		nRepCnt = 0;
		memset(lpszFileName, 0, sizeof(lpszFileName));
		hDC = NULL;
		bGrid = FALSE;
		hWnd = NULL;
		nDetail = 0;
		nX = 0;
		nY = 0;
	}

	COverlayEvent::COverlayEvent(E_OVERLAY_EVENT event_in, CPoint pt)
	{
		e_evnet = event_in;
		point = pt;
	}

	COverlayEvent::COverlayEvent(UINT nFlags_, OVERLAY::OBJECT_TYPE eType, UINT nRepCnt_)
	{
		nRepCnt = nRepCnt_;
		eObjectType = eType;
		nFlags = nFlags_;
	}

	COverlayEvent::COverlayEvent(E_OVERLAY_EVENT event_in, CPoint pt, UINT nFlags_)
	{
		nFlags = nFlags_;
		e_evnet = event_in;
		point = pt;
	}

	COverlayEvent::~COverlayEvent()
	{
	}


	COverlayEventMgr::COverlayEventMgr()
	{
		clear();
	}

	COverlayEventMgr::~COverlayEventMgr()
	{
	}

	void COverlayEventMgr::clear()
	{
		memset(m_nMouseEventQueue, 0, sizeof(m_nMouseEventQueue));
		m_nPopPosition = 0;
		m_nPushPosition = 1;
		m_nSize = 0;
	}

	BOOL COverlayEventMgr::push(COverlayEvent OverlayEvent)
	{
		if (m_nSize >= MAX_OVERLAY_EVENT)
			return FALSE;

		m_nPushPosition %= MAX_OVERLAY_EVENT;
		m_nPopPosition %= MAX_OVERLAY_EVENT;
		if (m_nPushPosition == m_nPopPosition)
			return FALSE;

		m_nMouseEventQueue[m_nPushPosition] = OverlayEvent;
		m_nPushPosition++;
		m_nSize++;

		return TRUE;
	}

	COverlayEvent* COverlayEventMgr::pop()
	{
		m_nSize--;
		if (m_nSize < 0)
		{
			m_nSize = 0;
			clear();
			return NULL;
		}

		m_nPopPosition++;
		m_nPopPosition %= MAX_OVERLAY_EVENT;
		return &m_nMouseEventQueue[m_nPopPosition];
	}

	COverlayEvent* COverlayEventMgr::get_next()
	{
		if (m_nPopPosition + 1 < MAX_OVERLAY_EVENT)
			return &m_nMouseEventQueue[m_nPopPosition + 1];
		else
			return NULL;

	}

	INT COverlayEventMgr::size()
	{
		return m_nSize;
	}
}


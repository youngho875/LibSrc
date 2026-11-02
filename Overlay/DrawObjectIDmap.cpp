#include "pch.h"
#include "DrawObjectIdMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawObjectIdMap::DrawObjectIdMap(Gdiplus::Graphics& g, long lw, long lh)
		:DrawField(g, lw, lh)
	{
	}

	DrawObjectIdMap::~DrawObjectIdMap()
	{

	}

	BOOL DrawObjectIdMap::Draw(OVERLAY::OBJECT* pObject)
	{
		switch (pObject->GetType())
		{
		case OVERLAY::CURVE:
			m_ObjectCurveDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::POINT_TYPE:

			break;
		case OVERLAY::LINE:
			m_ObjectLineDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::TRIANGLE:
			m_ObjectTriangleDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::RECT_TYPE:
			m_ObjectRectDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::DIAMOND:
			m_ObjectDiamondDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::ELLIPSE:
			m_ObjectEllipseDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::POLYGON:
			m_ObjectPolygonDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::PIE:
			m_ObjectPieDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::POLYLINE:
			m_ObjectPolyLineDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::ROUND_RECT:
			m_ObjectRoundRectDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::TEXT:
			m_ObjectTextDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::PENTAGON:
			m_ObjectPentagonDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::SIX_ANGLE:
			m_ObjectSixAngleDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::EIGHT_ANGLE:
			m_ObjectEightAngleDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::IMAGE:
			m_ObjectImageDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::ARC:
			m_ObjectArcDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::LINKED_LINE:
			m_ObjectLinkedLineDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_LINE:
			m_ObjectLinkedMultiLineDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::LINKED_CURVE:
			m_ObjectLinkedCurveDraw.DrawMap(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_CURVE:
			m_ObjectLinkedMultiCurveDraw.DrawMap(pObject, &GRP);

			break;
		default:
			return FALSE;
		}

		return TRUE;
	}

	SHORT DrawObjectIdMap::GetObjectID(int x, int y)
	{
		SHORT nID = 0;
		Gdiplus::Color	object_selected_point_id;
		BMP.GetPixel(x, y, &object_selected_point_id);

		char chID[2];
		chID[0] = object_selected_point_id.GetB();
		chID[1] = object_selected_point_id.GetG();

		memcpy(&nID, chID, sizeof(chID));
		if (nID < 0)
			nID = INVALID_OBJECT_ID;
		return nID;
	}
}
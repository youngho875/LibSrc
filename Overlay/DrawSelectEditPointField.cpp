#include "pch.h"
#include "DrawSelectEditPointField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawSelectEditPointField::DrawSelectEditPointField(Gdiplus::Graphics& g, long lw, long lh)
		:DrawField(g, lw, lh)
	{
	}

	DrawSelectEditPointField::~DrawSelectEditPointField()
	{

	}

	BOOL DrawSelectEditPointField::Draw(OVERLAY::OBJECT* pObject)
	{
		switch (pObject->GetType())
		{
		case OVERLAY::CURVE:
			m_ObjectCurveDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::POINT_TYPE:

			break;
		case OVERLAY::LINE:
			m_ObjectLineDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::TRIANGLE:
			m_ObjectTriangleDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::RECT_TYPE:
			m_ObjectRectDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::DIAMOND:
			m_ObjectDiamondDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::ELLIPSE:
			m_ObjectEllipseDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::POLYGON:
			m_ObjectPolygonDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::PIE:
			m_ObjectPieDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::POLYLINE:
			m_ObjectPolyLineDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::ROUND_RECT:
			m_ObjectRoundRectDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::TEXT:
			m_ObjectTextDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::PENTAGON:
			m_ObjectPentagonDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::SIX_ANGLE:
			m_ObjectSixAngleDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::EIGHT_ANGLE:
			m_ObjectEightAngleDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::IMAGE:
			m_ObjectImageDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::ARC:
			m_ObjectArcDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::LINKED_LINE:
			m_ObjectLinkedLineDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_LINE:
			m_ObjectLinkedMultiLineDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::LINKED_CURVE:
			m_ObjectLinkedCurveDraw.DrawSelectPoint(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_CURVE:
			m_ObjectLinkedMultiCurveDraw.DrawSelectPoint(pObject, &GRP);

			break;
		default:
			return FALSE;
		}

		return TRUE;
	}

	SHORT DrawSelectEditPointField::GetObjectID(int x, int y)
	{
		SHORT nID = 0;
		Gdiplus::Color	object_selected_point_id;
		BMP.GetPixel(x, y, &object_selected_point_id);

		if (object_selected_point_id.GetR() != 255)
			return INVALID_OBJECT_ID;

		char chID[2];
		chID[1] = object_selected_point_id.GetG();
		chID[0] = object_selected_point_id.GetB();

		memcpy(&nID, chID, sizeof(chID));
		return nID;
	}
}
#include "pch.h"
#include "DrawField.h"

#include "../GeoDC/Coordmgr.h"
#include "../GeoDC/Coord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawField::DrawField(Gdiplus::Graphics& g, long lw, long lh)
		:DrawBasicField(g, lw, lh)
		, m_ObjectArcDraw()
		, m_ObjectPieDraw()
		, m_ObjectCurveDraw()
		, m_ObjectEllipseDraw()
		, m_ObjectLineDraw()
		, m_ObjectRectDraw()
		, m_ObjectRoundRectDraw()
		, m_ObjectTriangleDraw()
		, m_ObjectDiamondDraw()
		, m_ObjectPentagonDraw()
		, m_ObjectSixAngleDraw()
		, m_ObjectEightAngleDraw()
		, m_ObjectImageDraw()
		, m_ObjectTextDraw()
		, m_ObjectPolygonDraw()
		, m_ObjectPolyLineDraw()
		, m_ObjectLinkedLineDraw()
		, m_ObjectLinkedMultiLineDraw()
		, m_ObjectLinkedCurveDraw()
		, m_ObjectLinkedMultiCurveDraw()
	{

	}

	DrawField::~DrawField()
	{

	}

	BOOL DrawField::Draw(OVERLAY::OBJECT* pObject)
	{
		switch (pObject->GetType())
		{
		case OVERLAY::CURVE:
			m_ObjectCurveDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::POINT_TYPE:

			break;
		case OVERLAY::LINE:
			m_ObjectLineDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::TRIANGLE:
			m_ObjectTriangleDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::RECT_TYPE:
			m_ObjectRectDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::DIAMOND:
			m_ObjectDiamondDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::ELLIPSE:
			m_ObjectEllipseDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::POLYGON:
			m_ObjectPolygonDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::PIE:
			m_ObjectPieDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::POLYLINE:
			m_ObjectPolyLineDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::ROUND_RECT:
			m_ObjectRoundRectDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::TEXT:
			m_ObjectTextDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::PENTAGON:
			m_ObjectPentagonDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::SIX_ANGLE:
			m_ObjectSixAngleDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::EIGHT_ANGLE:
			m_ObjectEightAngleDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::IMAGE:
			m_ObjectImageDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::ARC:
			m_ObjectArcDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::LINKED_LINE:
			m_ObjectLinkedLineDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_LINE:
			m_ObjectLinkedMultiLineDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::LINKED_CURVE:
			m_ObjectLinkedCurveDraw.Draw(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_CURVE:
			m_ObjectLinkedMultiCurveDraw.Draw(pObject, &GRP);

			break;
		default:
			return FALSE;
		}

		return TRUE;
	}

	SHORT DrawField::GetObjectID(int x, int y)
	{
		SHORT nID = 0;
		Gdiplus::Color	object_selected_point_id;
		BMP.GetPixel(x, y, &object_selected_point_id);
		nID = object_selected_point_id.GetB();
		if (nID < 0)
			nID = INVALID_OBJECT_ID;
		return nID;
	}
}
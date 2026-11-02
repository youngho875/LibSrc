#include "pch.h"
#include "DrawEditField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	DrawEditField::DrawEditField(Gdiplus::Graphics& g, long lw, long lh)
		:DrawField(g, lw, lh)
	{
	}

	DrawEditField::~DrawEditField()
	{

	}

	BOOL DrawEditField::Draw(OVERLAY::OBJECT* pObject)
	{
		BOOL bRet = DrawField::Draw(pObject);
		switch (pObject->GetType())
		{
		case OVERLAY::CURVE:
			m_ObjectCurveDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::POINT_TYPE:

			break;
		case OVERLAY::LINE:
			m_ObjectLineDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::TRIANGLE:
			m_ObjectTriangleDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::RECT_TYPE:
			m_ObjectRectDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::DIAMOND:
			m_ObjectDiamondDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::ELLIPSE:
			m_ObjectEllipseDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::POLYGON:
			m_ObjectPolygonDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::PIE:
			m_ObjectPieDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::POLYLINE:
			m_ObjectPolyLineDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::ROUND_RECT:
			m_ObjectRoundRectDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::TEXT:
			m_ObjectTextDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::PENTAGON:
			m_ObjectPentagonDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::SIX_ANGLE:
			m_ObjectSixAngleDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::EIGHT_ANGLE:
			m_ObjectEightAngleDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::IMAGE:
			m_ObjectImageDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::ARC:
			m_ObjectArcDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::LINKED_LINE:
			m_ObjectLinkedLineDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_LINE:
			m_ObjectLinkedMultiLineDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::LINKED_CURVE:
			m_ObjectLinkedCurveDraw.DrawOutLine(pObject, &GRP);

			break;
		case OVERLAY::LINKED_MULTI_CURVE:
			m_ObjectLinkedMultiCurveDraw.DrawOutLine(pObject, &GRP);

			break;
		default:
			return FALSE;
		}

		return bRet;
	}
}
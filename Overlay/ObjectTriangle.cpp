#include "pch.h"
#include "ObjectTriangle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectTriangle::ObjectTriangle()
		:OBJECT(TRIANGLE, POINTS_OF_OBJECT_TRIANGLE)
	{
	}

	ObjectTriangle::~ObjectTriangle()
	{
	}

	void ObjectTriangle::Move(CPoint ptMove, OUT_LINE nKindOfSelectedPoint, UINT nIndex)
	{
		switch (nKindOfSelectedPoint)
		{
		case ROTATE_CENTER:
			MoveRotatePoint(ptMove);
			break;

		case LEFT_TOP:
			MoveTopLeftPoint(ptMove);

			break;
		case TOP_CENTER:
			MoveTopCenterPoint(ptMove);

			break;
		case RIGHT_TOP:
			MoveTopRightPoint(ptMove);

			break;
		case LEFT_CENTER:
			MoveLeftCenterPoint(ptMove);

			break;
		case RIGHT_CENTER:
			MoveRightCenterPoint(ptMove);

			break;
		case LEFT_BOTTOM:
			MoveBottomLeftPoint(ptMove);

			break;
		case BOTTOM_CENTER:
			MoveBottomCenterPoint(ptMove);

			break;
		case RIGHT_BOTTOM:
			MoveBottomRightPoint(ptMove);

			break;
		case SWEEP_POSITION:// 삼각형, 모서리가 둥근 사각형의 모양 변경점.
			MoveSweepPoint(ptMove);

			break;
		default:
			break;
		}
	}
}

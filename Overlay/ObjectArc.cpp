#include "pch.h"
#include "ObjectArc.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{

	ObjectArc::ObjectArc()
		:OBJECT(ARC, POINTS_OF_OBJECT_ARC)
	{
		m_dSweepAngle[0] = 0;
		m_dSweepAngle[1] = 45;
		Property.bIsFill = FALSE;
	}

	ObjectArc::~ObjectArc()
	{
	}

	void ObjectArc::SetSweepAngle(double Angle_1, double Angle_2)
	{
		m_dSweepAngle[0] = Angle_1;
		m_dSweepAngle[1] = Angle_2;
	}

	void ObjectArc::GetSweepAngle(double* Angle_1, double* Angle_2)
	{
		*Angle_1 = m_dSweepAngle[0];
		*Angle_2 = m_dSweepAngle[1];
	}

	double ObjectArc::GetSweepAngle(UINT nIndex)
	{
		if (nIndex >= 2)
			return 0;
		return m_dSweepAngle[nIndex];
	}

	void ObjectArc::GetSweepAngle(UINT nIndex, double* Angle)
	{
		if (nIndex >= 2)
			return;
		*Angle = m_dSweepAngle[nIndex];
	}

	void ObjectArc::Move(CPoint ptMove, OUT_LINE nKindOfSelectedPoint, UINT nIndex)
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
		case SWEEP_ANGLE_1:// 부채꼴의 모양 변경점1.
			MoveSweepAngle1(ptMove, m_dSweepAngle[0]);

			break;
		case SWEEP_ANGLE_2:// 부채꼴의 모양 변경점2.
			MoveSweepAngle2(ptMove, m_dSweepAngle[1]);

			break;
		default:
			break;
		}
	}

	void ObjectArc::SetOutLine()
	{
		// x
		pt_out_line[ROTATE_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt_out_line[ROTATE_CENTER].Y());
		pt_out_line[TOP_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt_out_line[TOP_CENTER].Y());
		pt_out_line[BOTTOM_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt_out_line[BOTTOM_CENTER].Y());

		if (pt[LEFT_UP].X() < pt[RIGHT_DOWN].X())
		{
			pt_out_line[LEFT_TOP].SetXY(pt[LEFT_UP].X(), pt_out_line[LEFT_TOP].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[LEFT_UP].X(), pt_out_line[LEFT_CENTER].Y());
			pt_out_line[LEFT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt_out_line[LEFT_BOTTOM].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_TOP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_CENTER].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_BOTTOM].Y());
		}
		else
		{
			pt_out_line[LEFT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[LEFT_TOP].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[LEFT_CENTER].Y());
			pt_out_line[LEFT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt_out_line[LEFT_BOTTOM].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[LEFT_UP].X(), pt_out_line[RIGHT_TOP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[LEFT_UP].X(), pt_out_line[RIGHT_CENTER].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt_out_line[RIGHT_BOTTOM].Y());
		}
		// y
		pt_out_line[RIGHT_CENTER].SetXY(pt_out_line[RIGHT_CENTER].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
		pt_out_line[LEFT_CENTER].SetXY(pt_out_line[LEFT_CENTER].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);

		if (pt[LEFT_UP].Y() < pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY(pt_out_line[ROTATE_CENTER].X(), pt[LEFT_UP].Y() - OFF_SET_FROM_TOP);

			pt_out_line[LEFT_TOP].SetXY(pt_out_line[LEFT_TOP].X(), pt[LEFT_UP].Y());
			pt_out_line[TOP_CENTER].SetXY(pt_out_line[TOP_CENTER].X(), pt[LEFT_UP].Y());
			pt_out_line[RIGHT_TOP].SetXY(pt_out_line[RIGHT_TOP].X(), pt[LEFT_UP].Y());

			pt_out_line[LEFT_BOTTOM].SetXY(pt_out_line[LEFT_BOTTOM].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[BOTTOM_CENTER].SetXY(pt_out_line[BOTTOM_CENTER].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt_out_line[RIGHT_BOTTOM].X(), pt[RIGHT_DOWN].Y());
		}
		else
		{
			pt_out_line[ROTATE_CENTER].SetXY(pt_out_line[ROTATE_CENTER].X(), pt[RIGHT_DOWN].Y() - OFF_SET_FROM_TOP);

			pt_out_line[LEFT_TOP].SetXY(pt_out_line[LEFT_TOP].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[TOP_CENTER].SetXY(pt_out_line[TOP_CENTER].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_TOP].SetXY(pt_out_line[RIGHT_TOP].X(), pt[RIGHT_DOWN].Y());

			pt_out_line[LEFT_BOTTOM].SetXY(pt_out_line[LEFT_BOTTOM].X(), pt[LEFT_UP].Y());
			pt_out_line[BOTTOM_CENTER].SetXY(pt_out_line[BOTTOM_CENTER].X(), pt[LEFT_UP].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt_out_line[RIGHT_BOTTOM].X(), pt[LEFT_UP].Y());
		}

		//----------------------------------------------
		// Sweep Angle 구하기
		double a = 0.0;	// 장축.
		double b = 0.0;	// 단축.
		double ax = 0.0;
		double ay = 0.0;

		double cx = 0.0;
		double cy = 0.0;

		a = abs((double)(pt[RIGHT_DOWN].X() - pt[LEFT_UP].X())) / 2;
		b = abs((double)(pt[RIGHT_DOWN].Y() - pt[LEFT_UP].Y())) / 2;

		cx = (pt[RIGHT_DOWN].X() + pt[LEFT_UP].X()) / 2;
		cy = (pt[RIGHT_DOWN].Y() + pt[LEFT_UP].Y()) / 2;

		//////////////////////////////////			  
		double dTempAngle = m_dSweepAngle[0] + 270;
		ax = cx + a * cos(dTempAngle * MATH_DEGREE_TO_RADIAN);
		ay = cy + b * sin(dTempAngle * MATH_DEGREE_TO_RADIAN);

		pt_out_line[SWEEP_ANGLE_1].SetXY(ax, ay);

		//////////////////////////////////
		dTempAngle = m_dSweepAngle[1] + 270;
		ax = cx + a * cos(dTempAngle * MATH_DEGREE_TO_RADIAN);
		ay = cy + b * sin(dTempAngle * MATH_DEGREE_TO_RADIAN);

		pt_out_line[SWEEP_ANGLE_2].SetXY(ax, ay);
		//----------------------------------------------


		if (Property.dAngle != 0)
			RotateOutLine();
	}
}

#include "pch.h"
#include "ObjectCurve.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectCurve::ObjectCurve()
		:OBJECT(CURVE, POINTS_OF_OBJECT_CURVE)
	{
		Property.bIsFill = FALSE;
	}

	ObjectCurve::~ObjectCurve()
	{
	}

	void ObjectCurve::SetOutLine()
	{
		if (nPointIndex <= 1 || nPointIndex >= MAX_POINTS)
			return;
		Point	temp_pt[2];

		temp_pt[LEFT_UP].SetXY(pt[0].X(), pt[0].Y());
		temp_pt[RIGHT_DOWN].SetXY(pt[0].X(), pt[0].Y());

		for (UINT i = 1; i < nPointIndex; i++)
		{
			if (temp_pt[LEFT_UP].X() > pt[i].X())
				temp_pt[LEFT_UP].SetXY(pt[i].X(), temp_pt[LEFT_UP].Y());
			if (temp_pt[LEFT_UP].Y() > pt[i].Y())
				temp_pt[LEFT_UP].SetXY(temp_pt[LEFT_UP].X(), pt[i].Y());

			if (temp_pt[RIGHT_DOWN].X() < pt[i].X())
				temp_pt[RIGHT_DOWN].SetXY(pt[i].X(), temp_pt[RIGHT_DOWN].Y());
			if (temp_pt[RIGHT_DOWN].Y() < pt[i].Y())
				temp_pt[RIGHT_DOWN].SetXY(temp_pt[RIGHT_DOWN].X(), pt[i].Y());

		}

		pt_out_line[ROTATE_CENTER].SetXY((temp_pt[LEFT_UP].X() + temp_pt[RIGHT_DOWN].X()) / 2, pt_out_line[ROTATE_CENTER].Y());
		pt_out_line[TOP_CENTER].SetXY((temp_pt[LEFT_UP].X() + temp_pt[RIGHT_DOWN].X()) / 2, pt_out_line[TOP_CENTER].Y());
		pt_out_line[BOTTOM_CENTER].SetXY((temp_pt[LEFT_UP].X() + temp_pt[RIGHT_DOWN].X()) / 2, pt_out_line[BOTTOM_CENTER].Y());

		if (temp_pt[LEFT_UP].X() < temp_pt[RIGHT_DOWN].X())
		{
			pt_out_line[LEFT_TOP].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[LEFT_TOP].Y());
			pt_out_line[LEFT_CENTER].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[LEFT_CENTER].Y());
			pt_out_line[LEFT_BOTTOM].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[LEFT_BOTTOM].Y());

			pt_out_line[RIGHT_TOP].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_TOP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_CENTER].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[RIGHT_BOTTOM].Y());
		}
		else
		{
			pt_out_line[LEFT_TOP].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[LEFT_TOP].Y());
			pt_out_line[LEFT_CENTER].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[LEFT_CENTER].Y());
			pt_out_line[LEFT_BOTTOM].SetXY(temp_pt[RIGHT_DOWN].X(), pt_out_line[LEFT_BOTTOM].Y());

			pt_out_line[RIGHT_TOP].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[RIGHT_TOP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[RIGHT_CENTER].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(temp_pt[LEFT_UP].X(), pt_out_line[RIGHT_BOTTOM].Y());
		}
		// y
		pt_out_line[RIGHT_CENTER].SetXY(pt_out_line[RIGHT_CENTER].X(), (temp_pt[LEFT_UP].Y() + temp_pt[RIGHT_DOWN].Y()) / 2);
		pt_out_line[LEFT_CENTER].SetXY(pt_out_line[LEFT_CENTER].X(), (temp_pt[LEFT_UP].Y() + temp_pt[RIGHT_DOWN].Y()) / 2);

		if (temp_pt[LEFT_UP].Y() < temp_pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY(pt_out_line[ROTATE_CENTER].X(), temp_pt[LEFT_UP].Y() - OFF_SET_FROM_TOP);

			pt_out_line[LEFT_TOP].SetXY(pt_out_line[LEFT_TOP].X(), temp_pt[LEFT_UP].Y());
			pt_out_line[TOP_CENTER].SetXY(pt_out_line[TOP_CENTER].X(), temp_pt[LEFT_UP].Y());
			pt_out_line[RIGHT_TOP].SetXY(pt_out_line[RIGHT_TOP].X(), temp_pt[LEFT_UP].Y());

			pt_out_line[LEFT_BOTTOM].SetXY(pt_out_line[LEFT_BOTTOM].X(), temp_pt[RIGHT_DOWN].Y());
			pt_out_line[BOTTOM_CENTER].SetXY(pt_out_line[BOTTOM_CENTER].X(), temp_pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt_out_line[RIGHT_BOTTOM].X(), temp_pt[RIGHT_DOWN].Y());
		}
		else
		{
			pt_out_line[ROTATE_CENTER].SetXY(pt_out_line[ROTATE_CENTER].X(), temp_pt[RIGHT_DOWN].Y() - OFF_SET_FROM_TOP);

			pt_out_line[LEFT_TOP].SetXY(pt_out_line[LEFT_TOP].X(), temp_pt[RIGHT_DOWN].Y());
			pt_out_line[TOP_CENTER].SetXY(pt_out_line[TOP_CENTER].X(), temp_pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_TOP].SetXY(pt_out_line[RIGHT_TOP].X(), temp_pt[RIGHT_DOWN].Y());

			pt_out_line[LEFT_BOTTOM].SetXY(pt_out_line[LEFT_BOTTOM].X(), temp_pt[LEFT_UP].Y());
			pt_out_line[BOTTOM_CENTER].SetXY(pt_out_line[BOTTOM_CENTER].X(), temp_pt[LEFT_UP].Y());
			pt_out_line[RIGHT_BOTTOM].SetXY(pt_out_line[RIGHT_BOTTOM].X(), temp_pt[LEFT_UP].Y());
		}

		//----------------------------------------------

		if (Property.dAngle != 0)
			RotateOutLine();
	}
}
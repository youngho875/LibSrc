#include "pch.h"
#include "Object.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	OBJECT_LINKED_LINE::OBJECT_LINKED_LINE()
	{
		bIsLinkedLine = FALSE;
		nCurrentLinkedLineCount = 0;
		memset(nLinkedLineObjectID, NULL, sizeof(nLinkedLineObjectID));
	}

	OBJECT_LINKED_LINE::~OBJECT_LINKED_LINE()
	{
	}

	BOOL OBJECT_LINKED_LINE::RemoveObjectID(UINT nID)
	{
		for (int i = 0; i < nCurrentLinkedLineCount; i++)
		{
			if (nID == nLinkedLineObjectID[i])
			{
				nCurrentLinkedLineCount--;
				if (nCurrentLinkedLineCount < 0)
					nCurrentLinkedLineCount = 0;

				else if (nCurrentLinkedLineCount >= OBJECT_PER_LAYER)
					nCurrentLinkedLineCount = OBJECT_PER_LAYER - 1;

				nLinkedLineObjectID[i] = nLinkedLineObjectID[nCurrentLinkedLineCount];
				nLinkedLineObjectID[nCurrentLinkedLineCount] = 0;

				if (nCurrentLinkedLineCount == 0)
					bIsLinkedLine = FALSE;

				return TRUE;
			}
		}

		return FALSE;
	}

	void OBJECT_LINKED_LINE::AddObjectID(UINT nID)
	{
		if (nCurrentLinkedLineCount >= OBJECT_PER_LAYER)
			return;
		nLinkedLineObjectID[nCurrentLinkedLineCount] = nID;
		nCurrentLinkedLineCount++;
		bIsLinkedLine = TRUE;
	}

	BOOL OBJECT_LINKED_LINE::ExistLinkedLine()
	{
		return bIsLinkedLine;
	}

	UINT OBJECT_LINKED_LINE::CurrentLinkedLineCount()
	{
		return nCurrentLinkedLineCount;
	}

	UINT OBJECT_LINKED_LINE::GetBack()
	{
		if (nCurrentLinkedLineCount > 0 && nCurrentLinkedLineCount < OBJECT_PER_LAYER)
			nCurrentLinkedLineCount--;
		else
			return 0;

		if (nCurrentLinkedLineCount <= 0)
		{
			nCurrentLinkedLineCount = 0;
			bIsLinkedLine = FALSE;
		}

		UINT nRet = nLinkedLineObjectID[nCurrentLinkedLineCount];
		nLinkedLineObjectID[nCurrentLinkedLineCount] = 0;

		return nRet;
	}

	UINT OBJECT_LINKED_LINE::GetID(UINT nIndex)
	{
		if (nCurrentLinkedLineCount <= nIndex)
			return 0;
		return nLinkedLineObjectID[nIndex];
	}

	void OBJECT_LINKED_LINE::RemoveAll()
	{
		nCurrentLinkedLineCount = 0;
		memset(nLinkedLineObjectID, 0, sizeof(nLinkedLineObjectID));
	}

	OBJECT::OBJECT(OBJECT_TYPE eType, UINT nNumberOfPoints)
		:Property()
	{
		Type = eType;
		nID = 0;
		pt = NULL;
		nPoints = nNumberOfPoints;
		nPointIndex = 0;
		nGroupID = 0;
		InitPoints();
		memset(pt_out_line, 0, sizeof(pt_out_line));
		memset(pt_old_out_line, 0, sizeof(pt_old_out_line));

		if (nPoints > 0)
			memset(pt, 0, sizeof(Point) * nPoints);
		Property.InitProperty();

	}

	OBJECT::~OBJECT()
	{
		if (pt != NULL)
			delete[] pt;
	}

	double OBJECT::GetDividedFace(double x1, double y1, int x2, int y2)
	{
		if (x1 < x2 && y1 < y2)
		{
			return 90.0;
		}
		else if (x1<x2 && y1>y2)
		{
			return 90.0;
		}
		else if (x1 > x2 && y1 < y2)
		{
			return 270.0;
		}
		else if (x1 > x2 && y1 > y2)
		{
			return 270.0;
		}

		if (x1 == x2)
		{
			if (y1 > y2)
				return 270.0;
			if (y2 > y1)
				return 270.0;
		}
		if (y1 == y2)
		{
			if (x1 > x2)
				return 270.0;
			if (x2 > x1)
				return 90.0;
		}
		return 0.0;
	}

	void OBJECT::MoveRotatePoint(CPoint ptMove)
	{//도형의 회전축을 계산한다.
		double dDistance = 0.0;
		double cx = 0.0;
		double cy = 0.0;

		cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		Property.dAngle = (atan((cy - ptMove.y) / (cx - ptMove.x)) * MATH_RADIAN_TO_DEGREE) + GetDividedFace(cx, cy, ptMove.x, ptMove.y);
	}

	void OBJECT::MoveTopLeftPoint(CPoint ptMove)
	{
		pt_out_line[LEFT_TOP].SetXY((double)ptMove.x, (double)ptMove.y);
		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;

		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		if (Type == ROUND_RECT)
		{
			//TRACE("%6.2f    %6.2f\n",pt[0].X(),(pt[1].X()+pt[0].X())/2);

			if (pt[0].X() >= (pt[1].X() + pt[0].X()) / 2) {
				//pt[2].SetXY(pt[2].X()-1.0,pt[2].Y());

				return;
			}
		}

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_out_line[LEFT_TOP].X(), pt_out_line[LEFT_TOP].Y(), pt[0]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_BOTTOM].X(), pt_old_out_line[RIGHT_BOTTOM].Y(), pt[1]);


		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[RIGHT_BOTTOM].X();
		double Gap_y = pt_old_out_line[RIGHT_BOTTOM].Y();

		SetOutLine();

		Gap_x = pt_out_line[RIGHT_BOTTOM].X() - Gap_x;
		Gap_y = pt_out_line[RIGHT_BOTTOM].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//	TRACE("%f, %f \n",Gap_x,Gap_y);
		//	TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveTopRightPoint(CPoint ptMove)
	{
		pt_out_line[RIGHT_TOP].SetXY((double)ptMove.x, (double)ptMove.y);
		double cx = (pt_out_line[RIGHT_TOP].X() + pt_out_line[LEFT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[RIGHT_TOP].Y() + pt_out_line[LEFT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		if (Type == ROUND_RECT)
		{
			//TRACE("%6.2f    %6.2f\n",pt[0].X(),(pt[1].X()+pt[0].X())/2);

			if (pt[0].X() >= (pt[1].X() + pt[0].X()) / 2) {
				//pt[2].SetXY(pt[2].X()-1.0,pt[2].Y());

				return;
			}
		}
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_out_line[RIGHT_TOP].X(), pt_out_line[RIGHT_TOP].Y(), pt_out_line[RIGHT_TOP]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_BOTTOM].X(), pt_old_out_line[LEFT_BOTTOM].Y(), pt_out_line[LEFT_BOTTOM]);

		pt[0].SetXY(pt_out_line[LEFT_BOTTOM].X(), pt_out_line[RIGHT_TOP].Y());
		pt[1].SetXY(pt_out_line[RIGHT_TOP].X(), pt_out_line[LEFT_BOTTOM].Y());

		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[LEFT_BOTTOM].X();
		double Gap_y = pt_old_out_line[LEFT_BOTTOM].Y();

		SetOutLine();

		Gap_x = pt_out_line[LEFT_BOTTOM].X() - Gap_x;
		Gap_y = pt_out_line[LEFT_BOTTOM].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveBottomLeftPoint(CPoint ptMove)
	{
		pt_out_line[LEFT_BOTTOM].SetXY((double)ptMove.x, (double)ptMove.y);
		double cx = (pt_out_line[RIGHT_TOP].X() + pt_out_line[LEFT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[RIGHT_TOP].Y() + pt_out_line[LEFT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		if (Type == ROUND_RECT)
		{
			//TRACE("%6.2f    %6.2f\n",pt[0].X(),(pt[1].X()+pt[0].X())/2);

			if (pt[0].X() >= (pt[1].X() + pt[0].X()) / 2) {
				//pt[2].SetXY(pt[2].X()-1.0,pt[2].Y());

				return;
			}
		}

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_TOP].X(), pt_old_out_line[RIGHT_TOP].Y(), pt_out_line[RIGHT_TOP]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_out_line[LEFT_BOTTOM].X(), pt_out_line[LEFT_BOTTOM].Y(), pt_out_line[LEFT_BOTTOM]);

		pt[0].SetXY(pt_out_line[LEFT_BOTTOM].X(), pt_out_line[RIGHT_TOP].Y());
		pt[1].SetXY(pt_out_line[RIGHT_TOP].X(), pt_out_line[LEFT_BOTTOM].Y());

		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[RIGHT_TOP].X();
		double Gap_y = pt_old_out_line[RIGHT_TOP].Y();

		SetOutLine();

		Gap_x = pt_out_line[RIGHT_TOP].X() - Gap_x;
		Gap_y = pt_out_line[RIGHT_TOP].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveBottomRightPoint(CPoint ptMove)
	{
		pt_out_line[RIGHT_BOTTOM].SetXY((double)ptMove.x, (double)ptMove.y);
		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		// 	if(Type==ROUND_RECT)
		// 	{
		// 		if( pt[2].X()>= (pt[1].X()+pt[0].X())/2){
		// 			return;
		// 		}
		// 	}
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_TOP].X(), pt_old_out_line[LEFT_TOP].Y(), pt[0]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_out_line[RIGHT_BOTTOM].X(), pt_out_line[RIGHT_BOTTOM].Y(), pt[1]);



		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[LEFT_TOP].X();
		double Gap_y = pt_old_out_line[LEFT_TOP].Y();

		SetOutLine();

		Gap_x = pt_out_line[LEFT_TOP].X() - Gap_x;
		Gap_y = pt_out_line[LEFT_TOP].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveTopCenterPoint(CPoint ptMove)
	{
		//return;
		OVERLAY::Point pt_temp;

		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		RotatePoint(cx, cy, 360 - Property.dAngle, ptMove.x, ptMove.y, pt_temp);

		//	double dx = pt_temp.X() - pt[0].X();
		double dy = pt_temp.Y() - pt[0].Y();

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_TOP].X(), pt_old_out_line[LEFT_TOP].Y(), pt_temp);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_BOTTOM].X(), pt_old_out_line[RIGHT_BOTTOM].Y(), pt[1]);

		// 	if(Type==ROUND_RECT)
		// 	{				
		// 		
		// 		if( pt[2].X()>= (pt[1].X()+pt[0].X())/2){
		// 			return;
		// 		}
		// 	}
		pt[0].SetXY(pt_temp.X(), pt[0].Y() + dy);

		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[RIGHT_BOTTOM].X();
		double Gap_y = pt_old_out_line[RIGHT_BOTTOM].Y();

		SetOutLine();

		Gap_x = pt_out_line[RIGHT_BOTTOM].X() - Gap_x;
		Gap_y = pt_out_line[RIGHT_BOTTOM].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveLeftCenterPoint(CPoint ptMove)
	{
		//return;
		OVERLAY::Point pt_temp;

		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());
		if (Type == ROUND_RECT)
		{
			//TRACE("%6.2f    %6.2f\n",pt[0].X(),(pt[1].X()+pt[0].X())/2);

			if (pt[0].X() >= (pt[1].X() + pt[0].X()) / 2) {
				//pt[2].SetXY(pt[2].X()-1.0,pt[2].Y());

				return;
			}
		}

		RotatePoint(cx, cy, 360 - Property.dAngle, ptMove.x, ptMove.y, pt_temp);

		double dx = pt_temp.X() - pt[0].X();
		//double dy = pt_temp.Y() - pt[0].Y();

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_TOP].X(), pt_old_out_line[LEFT_TOP].Y(), pt_temp);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_BOTTOM].X(), pt_old_out_line[RIGHT_BOTTOM].Y(), pt[1]);

		pt[0].SetXY(pt[0].X() + dx, pt_temp.Y());



		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[RIGHT_BOTTOM].X();
		double Gap_y = pt_old_out_line[RIGHT_BOTTOM].Y();

		SetOutLine();

		Gap_x = pt_out_line[RIGHT_BOTTOM].X() - Gap_x;
		Gap_y = pt_out_line[RIGHT_BOTTOM].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveRightCenterPoint(CPoint ptMove)
	{
		//return;
		OVERLAY::Point pt_temp;

		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		RotatePoint(cx, cy, 360 - Property.dAngle, ptMove.x, ptMove.y, pt_temp);

		double dx = pt_temp.X() - pt[1].X();
		//double dy = pt_temp.Y() - pt[0].Y();

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_TOP].X(), pt_old_out_line[LEFT_TOP].Y(), pt[0]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_BOTTOM].X(), pt_old_out_line[RIGHT_BOTTOM].Y(), pt_temp);

		// 	if(Type==ROUND_RECT)
		// 	{				
		// 		
		// 		if( pt[2].X()>= (pt[1].X()+pt[0].X())/2){
		// 			return;
		// 		}
		// 	}

		pt[1].SetXY(pt[1].X() + dx, pt_temp.Y());

		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[LEFT_TOP].X();
		double Gap_y = pt_old_out_line[LEFT_TOP].Y();

		SetOutLine();

		Gap_x = pt_out_line[LEFT_TOP].X() - Gap_x;
		Gap_y = pt_out_line[LEFT_TOP].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveBottomCenterPoint(CPoint ptMove)
	{
		//return;
		OVERLAY::Point pt_temp;

		double cx = (pt_out_line[LEFT_TOP].X() + pt_out_line[RIGHT_BOTTOM].X()) / 2;
		double cy = (pt_out_line[LEFT_TOP].Y() + pt_out_line[RIGHT_BOTTOM].Y()) / 2;
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		double dPosition_x = (double)(pt[2].X() - pt[0].X()) / (double)(pt[1].X() - pt[0].X());

		RotatePoint(cx, cy, 360 - Property.dAngle, ptMove.x, ptMove.y, pt_temp);

		// 	if(Type==ROUND_RECT)
		// 	{				
		// 		
		// 		if( pt[2].X()>= (pt[1].X()+pt[0].X())/2){
		// 			return;
		// 		}
		// 	}

		//	double dx = pt_temp.X() - pt[0].X();
		double dy = pt_temp.Y() - pt[1].Y();

		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[LEFT_TOP].X(), pt_old_out_line[LEFT_TOP].Y(), pt[0]);
		RotatePoint(cx, cy, 360 - Property.dAngle, pt_old_out_line[RIGHT_BOTTOM].X(), pt_old_out_line[RIGHT_BOTTOM].Y(), pt_temp);

		pt[1].SetXY(pt_temp.X(), pt[1].Y() + dy);

		// 도형 편집 오차 수정.
		double Gap_x = pt_old_out_line[LEFT_TOP].X();
		double Gap_y = pt_old_out_line[LEFT_TOP].Y();

		SetOutLine();

		Gap_x = pt_out_line[LEFT_TOP].X() - Gap_x;
		Gap_y = pt_out_line[LEFT_TOP].Y() - Gap_y;

		CPoint ptMoveGap;
		ptMoveGap.x = (int)(Gap_x + 0.5);
		ptMoveGap.y = (int)(Gap_y + 0.5);

		//TRACE("%f, %f \n",Gap_x,Gap_y);
		//TRACE("%d, %d \n",ptMoveGap.x,ptMoveGap.y);
		Move(ptMoveGap);
		//------------------------------------
		RepositonSweepPoint(dPosition_x);
	}

	void OBJECT::MoveSweepPoint(CPoint ptMove)
	{
		OVERLAY::Point pt_temp;
		//도형의 중간점 cx,cy를 구한다. 	
		double cx = (pt[0].X() + pt[1].X()) / 2;
		double cy = (pt[0].Y() + pt[1].Y()) / 2;

		RotatePoint(cx, cy, 360 - Property.dAngle, (double)ptMove.x, (double)ptMove.y, pt_temp);

		double position_x = pt_temp.X();

		if (position_x < pt[0].X())
		{
			position_x = pt[0].X();
		}
		else if (position_x > pt[1].X())
		{
			position_x = pt[1].X();
		}

		if (Type == ROUND_RECT)
		{
			if (position_x - pt[0].X() >= (pt[1].Y() - pt[0].Y()) / 2) {
				TRACE("%6.2f %6.2f \n", position_x - pt[0].X(), (pt[1].Y() - pt[0].Y()) / 2);
				return;
			}
			// 		if(position_x-pt[0].X() >= (pt[1].X()-pt[0].X())/2)
			// 			return;

					//TRACE("positionX = %6.2f\n",position_x);

			pt[2].SetXY(position_x, pt[0].Y());
			// 		while(1)
			// 		{
			// 			position_x++;
			// 			if( position_x-pt[0].X()>= (pt[1].Y()-pt[0].Y())/2 )
			// 				break;
			// 
			// 		}
			// 			
			// 		pt[2].SetXY(position_x, pt[0].Y());

		}
		else if (Type == TRIANGLE)
		{
			pt[2].SetXY(position_x, pt[0].Y());
		}
	}

	void OBJECT::MoveSweepAngle1(CPoint ptMove, double& dSweepAngle)
	{
		double cx = 0;
		double cy = 0;
		cx = (pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2;
		cy = (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2;

		dSweepAngle = (atan((cy - ptMove.y) / (cx - ptMove.x)) * MATH_RADIAN_TO_DEGREE) + GetDividedFace(cx, cy, ptMove.x, ptMove.y) - Property.dAngle;
		while (dSweepAngle < 0)
		{
			dSweepAngle += 360;
		}
		int nSweepAngle = (double)dSweepAngle;
		nSweepAngle %= 360;
		dSweepAngle = (double)nSweepAngle;
	}

	void OBJECT::MoveSweepAngle2(CPoint ptMove, double& dSweepAngle)
	{
		double cx = 0;
		double cy = 0;
		cx = (pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2;
		cy = (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2;

		dSweepAngle = (atan((cy - ptMove.y) / (cx - ptMove.x)) * MATH_RADIAN_TO_DEGREE) + GetDividedFace(cx, cy, ptMove.x, ptMove.y) - Property.dAngle;
		int nSweepAngle = (double)dSweepAngle;
		nSweepAngle %= 360;
		dSweepAngle = (double)nSweepAngle;
	}

	void OBJECT::MoveGeneralPoint(CPoint ptMove, UINT nIndex)
	{
		if (nIndex >= MAX_POINTS)
		{
			return;
		}
		pt[nIndex].SetXY(ptMove.x, ptMove.y);
	}

	void OBJECT::EditPoint(CPoint ptMove, INT nIndex)
	{
		if ((UINT)nIndex >= nPoints)
			return;
		pt[nIndex].SetXY(ptMove.x, ptMove.y);
	}

	// 도형의 중점을 기준으로 특정 지점의 각을 얻어냄.
	double OBJECT::Angle(Point reference_point)
	{
		double cx, cy;
		cx = (pt[0].X() + pt[1].X()) / 2;
		cy = (pt[0].Y() + pt[1].Y()) / 2;

		return (atan((cy - reference_point.Y()) / (cx - reference_point.X()) * MATH_RADIAN_TO_DEGREE) + GetDividedFace(cx, cy, (INT)reference_point.X(), (INT)reference_point.Y()));
	}

	void OBJECT::RotatePoint(double cx, double cy, double dRotateAngle, double in_x, double in_y, OVERLAY::Point& out_pt)
	{
		double dDistance = 0.0;
		double dAngle = 0.0;
		double dx = 0, dy = 0;

		dDistance = Distance(cx, cy, in_x, in_y);

		if (cy - in_y > 0)
		{
			if (cy - in_y > dDistance)
				dDistance = cy - in_y;
		}
		else if (cy - in_y == 0 && abs(cx - in_x) < 1)
		{
			out_pt.SetXY(in_x, cy);
			return;
		}
		else
		{
			if (in_y - cy > dDistance)
				dDistance = in_y - cy;
		}

		dAngle = acos((cy - in_y) / dDistance);
		dAngle *= MATH_RADIAN_TO_DEGREE;

		if (cx > in_x)
			dAngle *= -1;
		ReferencePositon_Distance_Angle2TargetPosition(dDistance, dRotateAngle + (dAngle)-90, 0, 0, &dx, &dy);
		out_pt.SetXY(dx + cx, dy + cy);
	}

	void OBJECT::RotateOutLine()
	{
		double cx, cy;
		//cx = (pt_out_line[LEFT_TOP].X()+pt_out_line[RIGHT_BOTTOM].X())/2;
		//cy = (pt_out_line[LEFT_TOP].Y()+pt_out_line[RIGHT_BOTTOM].Y())/2;
		cx = (pt[0].X() + pt[1].X()) / 2;
		cy = (pt[0].Y() + pt[1].Y()) / 2;

		for (int i = 0; i < POINTS_OF_OBJECT_OUTLINE; i++)
		{
			RotatePoint(cx, cy, Property.dAngle, pt_out_line[i].X(), pt_out_line[i].Y(), pt_out_line[i]);
		}
	}

	void OBJECT::SetOldOutLinePosition()
	{// 도형 회전 편집시 연산오차 보정을 위한 편집전 도형 외곽선 저장
		memcpy(pt_old_out_line, pt_out_line, sizeof(pt_old_out_line));
	}

	void OBJECT::RepositonSweepPoint(double dPosition_x)
	{
		// 도형의 편집영역(사각 영역)의 삼각형,부채꼴,모서리가 둥근 사각형의 도형 모양변경점의 위치(비율)
		if (Type == ROUND_RECT || Type == TRIANGLE)
		{
			dPosition_x = (pt[1].X() - pt[0].X()) * dPosition_x;
			dPosition_x = pt[0].X() + dPosition_x;

			if (Type == ROUND_RECT)
			{
				if (dPosition_x < pt[0].X())
				{
					dPosition_x = pt[0].X();
				}
				else if (dPosition_x > pt[1].X())
				{
					dPosition_x = pt[1].X();
				}

				if (dPosition_x - pt[0].X() >= (pt[1].Y() - pt[0].Y()) / 2)
				{
					dPosition_x = pt[0].X() + (pt[1].Y() + pt[0].Y()) / 2;
				}

				pt[2].SetXY(dPosition_x, pt[0].Y());
			}
			else
			{
				if (dPosition_x < pt[0].X())
				{
					dPosition_x = pt[0].X();
				}
				else if (dPosition_x > pt[1].X())
				{
					dPosition_x = pt[1].X();
				}

				pt[2].SetXY(dPosition_x, pt[0].Y());
			}
		}
	}

	void OBJECT::Move(CPoint ptMove)
	{
		UINT i = 0;
		if (Type == POLYGON ||
			Type == POLYLINE ||
			Type == CURVE)
		{
			for (i = 0; i < nPointIndex; i++)
			{
				pt[i].SetXY(pt[i].X() - ptMove.x, pt[i].Y() - ptMove.y);
			}
		}
		else
		{
			for (i = 0; i < nPoints; i++)
			{
				pt[i].SetXY(pt[i].X() - ptMove.x, pt[i].Y() - ptMove.y);
			}
		}
	}

	void OBJECT::Move(CPoint ptMove, OUT_LINE eKindOfSelectedPoint, UINT nIndex)
	{
		switch (eKindOfSelectedPoint)
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

		case GENERAL_POINTS:
			MoveGeneralPoint(ptMove, nIndex);// Polygon의 점의 선택

			break;
		default:
			break;
		}
	}

	void OBJECT::RearrangePoints()
	{
		if (Type == OVERLAY::LINE ||
			Type == OVERLAY::LINKED_LINE ||
			Type == OVERLAY::LINKED_MULTI_LINE ||
			Type == OVERLAY::LINKED_CURVE ||
			Type == OVERLAY::LINKED_MULTI_CURVE ||
			Type == OVERLAY::POLYLINE ||
			Type == OVERLAY::POLYGON ||
			Type == OVERLAY::CURVE ||
			Type == OVERLAY::POINT_TYPE)
			return;
		if (nPoints < 2)
			return;

		double temp = 0;
		if (pt[0].X() > pt[1].X())
		{
			temp = pt[0].X();
			pt[0].SetXY(pt[1].X(), pt[0].Y());
			pt[1].SetXY(temp, pt[1].Y());
		}

		if (pt[0].Y() > pt[1].Y())
		{
			temp = pt[0].Y();
			pt[0].SetXY(pt[0].X(), pt[1].Y());
			pt[1].SetXY(pt[1].X(), temp);
		}
	}

	void OBJECT::SetOutLine()
	{
		memset(pt_out_line, 0, sizeof(pt_out_line));

		if (pt[LEFT_UP].X() <= pt[RIGHT_DOWN].X() && pt[LEFT_UP].Y() <= pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y() - OFF_SET_FROM_TOP);
			pt_out_line[TOP_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y());
			pt_out_line[BOTTOM_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y());

			pt_out_line[LEFT_TOP].SetXY(pt[LEFT_UP].X(), pt[LEFT_UP].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[LEFT_UP].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[LEFT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt[RIGHT_DOWN].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt[LEFT_UP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[RIGHT_DOWN].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt[RIGHT_DOWN].Y());
		}
		else if (pt[LEFT_UP].X() >= pt[RIGHT_DOWN].X() && pt[LEFT_UP].Y() <= pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y() - OFF_SET_FROM_TOP);
			pt_out_line[TOP_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y());
			pt_out_line[BOTTOM_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y());

			pt_out_line[LEFT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt[LEFT_UP].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[RIGHT_DOWN].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[LEFT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt[RIGHT_DOWN].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[LEFT_UP].X(), pt[LEFT_UP].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[LEFT_UP].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt[RIGHT_DOWN].Y());
		}
		else if (pt[LEFT_UP].X() >= pt[RIGHT_DOWN].X() && pt[LEFT_UP].Y() >= pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y() - OFF_SET_FROM_TOP);
			pt_out_line[TOP_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y());
			pt_out_line[BOTTOM_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y());

			pt_out_line[LEFT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[RIGHT_DOWN].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[LEFT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt[LEFT_UP].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[LEFT_UP].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[LEFT_UP].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt[LEFT_UP].Y());
		}
		else if (pt[LEFT_UP].X() <= pt[RIGHT_DOWN].X() && pt[LEFT_UP].Y() >= pt[RIGHT_DOWN].Y())
		{
			pt_out_line[ROTATE_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y() - OFF_SET_FROM_TOP);
			pt_out_line[TOP_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[RIGHT_DOWN].Y());
			pt_out_line[BOTTOM_CENTER].SetXY((pt[LEFT_UP].X() + pt[RIGHT_DOWN].X()) / 2, pt[LEFT_UP].Y());

			pt_out_line[LEFT_TOP].SetXY(pt[LEFT_UP].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[LEFT_CENTER].SetXY(pt[LEFT_UP].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[LEFT_BOTTOM].SetXY(pt[LEFT_UP].X(), pt[LEFT_UP].Y());

			pt_out_line[RIGHT_TOP].SetXY(pt[RIGHT_DOWN].X(), pt[RIGHT_DOWN].Y());
			pt_out_line[RIGHT_CENTER].SetXY(pt[RIGHT_DOWN].X(), (pt[LEFT_UP].Y() + pt[RIGHT_DOWN].Y()) / 2);
			pt_out_line[RIGHT_BOTTOM].SetXY(pt[RIGHT_DOWN].X(), pt[LEFT_UP].Y());
		}

		if (Type == OVERLAY::ROUND_RECT || Type == OVERLAY::TRIANGLE)
		{
			pt_out_line[SWEEP_POSITION].SetXY(pt[ROUND_RECT_VERTEX].X(), pt[ROUND_RECT_VERTEX].Y());
		}

		if (Property.dAngle != 0)
			RotateOutLine();
	}

	void OBJECT::InitPoints()
	{
		if (nPoints == 0)
			return;
		if (pt != NULL)
		{
			delete[] pt;
			pt = new Point[nPoints + 1];
		}
		else
		{
			pt = new Point[nPoints + 1];
		}
	}

	int	OBJECT::GetPoints()
	{
		return nPoints;
	}

	void OBJECT::SetPoints(UINT input_nPoints)
	{
		nPoints = input_nPoints;
	}

	int OBJECT::GetAddedPointsNum()
	{
		return nPointIndex;
	}

	void OBJECT::AddPoints()
	{
		nPointIndex++;
		if (nPointIndex > nPoints)
			nPointIndex = nPoints;
	}

	OBJECT_TYPE OBJECT::GetType()
	{
		return Type;
	}
	/*
	BOOL OBJECT::IsGroup()
	{
		return GroupID_Stack.empty();
	}

	UINT OBJECT::GetGroupID()
	{
		return GroupID_Stack.top();
	}

	void OBJECT::AddGroupID(UINT nGroupID)
	{
		GroupID_Stack.push(nGroupID);
	}

	BOOL OBJECT::ReleaseGroup()
	{
		if(GroupID_Stack.empty())
			return FALSE;
		else
			GroupID_Stack.pop();
		return TRUE;
	}*/

	void OBJECT::SetGroup(UINT nGroup_ID)
	{
		nGroupID = nGroup_ID;
	}

	void OBJECT::ReleaseGroup()
	{
		nGroupID = 0;
	}


	UINT OBJECT::GetGroupID()
	{
		return nGroupID;
	}

}

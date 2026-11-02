#include "pch.h"
#include "math.h"
#include "Mathematics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//const double OVERLAY::MATH_PI = M_PI_;
//const double OVERLAY::MATH_DEGREE_TO_RADIAN = M_PI_ / 180;
//const double OVERLAY::MATH_RADIAN_TO_DEGREE = 180 / M_PI_;


namespace OVERLAY
{
	const double MATH_PI = M_PI_;
	const double MATH_DEGREE_TO_RADIAN = M_PI_ / 180;
	const double MATH_RADIAN_TO_DEGREE = 180 / M_PI_;

	CMathematics::CMathematics()
	{
		m_dDegreeToRadian = MATH_DEGREE_TO_RADIAN;
		m_dRadianToDegree = MATH_RADIAN_TO_DEGREE;
	}

	CMathematics::~CMathematics()
	{
	}

	double CMathematics::Radian2Degree(double dRadian)
	{
		return dRadian * m_dRadianToDegree;
	}

	double CMathematics::Degree2Radian(double dRadian)
	{
		return dRadian * m_dDegreeToRadian;
	}

	double CMathematics::Speed(double xVel, double yVel)
	{
		return sqrt(xVel * xVel + yVel * yVel);
	}

	double CMathematics::CourseSpeedToX(double dCourse, double dSpeed)
	{
		return cos(Degree2Radian(dCourse)) * dSpeed;
	}

	double CMathematics::CourseSpeedToY(double dCourse, double dSpeed)
	{
		return sin(Degree2Radian(dCourse)) * dSpeed;
	}

	void CMathematics::CourseSpeedToXY(double dCourse, double dSpeed, double* dVelx, double* dVely)
	{
		*dVelx = CourseSpeedToX(dCourse, dSpeed);
		*dVely = CourseSpeedToY(dCourse, dSpeed);
	}

	double CMathematics::Course(double xVel, double yVel)
	{
		double dSpeed = sqrt(xVel * xVel + yVel * yVel);

		return Radian2Degree(asin(yVel / dSpeed));
	}

	double CMathematics::CourseRadian(double xVel, double yVel)
	{
		double dSpeed = sqrt(xVel * xVel + yVel * yVel);

		return asin(yVel / dSpeed);
	}

	double CMathematics::Distance(double x1, double y1, double x2, double y2)
	{
		double x = abs(x2 - x1);
		double y = abs(y2 - y1);

		return sqrt(x * x + y * y);
	}

	void CMathematics::ReferencePositon_Distance_Angle2TargetPosition(double dDistance, double dAngle, double dReferencePositionX, double dReferencePositionY, double* dTargetX, double* dTargetY)
	{
		*dTargetX = cos(Degree2Radian(dAngle)) * dDistance + dReferencePositionX;
		*dTargetY = sin(Degree2Radian(dAngle)) * dDistance + dReferencePositionY;
		return;
	}
	//
	//void CMathematics::ReferencePositon_Distance_Angle2TargetPosition(double dDistance, double dAngle, double dReferencePositionX, double dReferencePositionY, double  dTargetX, double * dTargetY)
	//{
	//	*dTargetX = (Gdiplus::REAL)cos(Degree2Radian(dAngle))*dDistance+dReferencePositionX ;
	//	*dTargetY = (Gdiplus::REAL)sin(Degree2Radian(dAngle))*dDistance+dReferencePositionY ;
	//	return ;
	//}
	//
	//void CMathematics::ReferencePositon_Distance_Angle2TargetPosition(Gdiplus::REAL dDistance, Gdiplus::REAL dAngle, Gdiplus::REAL dReferencePositionX, Gdiplus::REAL dReferencePositionY, int* dTargetX, int* dTargetY)
	//{
	//	*dTargetX = (int)(cos(Degree2Radian(dAngle))*dDistance+dReferencePositionX) ;
	//	*dTargetY = (int)(sin(Degree2Radian(dAngle))*dDistance+dReferencePositionY) ;
	//	return ;
	//}

	void CMathematics::ReferencePositon_Radius_Angle2TargetPosition(double dRadius, double dAngle, double dReferencePositionX, double dReferencePositionY, double* dTargetX, double* dTargetY)
	{
		ReferencePositon_Distance_Angle2TargetPosition(dRadius, dAngle, dReferencePositionX, dReferencePositionY, dTargetX, dTargetY);
		return;
	}
	//
	//void CMathematics::ReferencePositon_Radius_Angle2TargetPosition(Gdiplus::REAL dRadius, Gdiplus::REAL dAngle, Gdiplus::REAL dReferencePositionX, Gdiplus::REAL dReferencePositionY, Gdiplus::REAL* dTargetX, Gdiplus::REAL* dTargetY )
	//{
	//	ReferencePositon_Distance_Angle2TargetPosition(dRadius, dAngle, dReferencePositionX, dReferencePositionY, dTargetX, dTargetY );
	//	return;
	//}
}
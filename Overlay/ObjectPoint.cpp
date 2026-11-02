#include "pch.h"
#include "ObjectPoint.h"
#include "../GeoDC/Coord.h"
#include "../GeoDC/CoordMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



namespace OVERLAY
{
	CPointInt::CPointInt(double* p)
	{
		pData = p;
		Data = 0;
	}

	CPointInt::~CPointInt()
	{
	}


	CPointDouble::CPointDouble(int* p)
	{
		pData = p;
		Data = 0;
	}
	CPointDouble::~CPointDouble()
	{
	}


	CCoordMgr*  Point::g_pCoord = NULL;
	Point::Point()
	{
		x = 0;
		y = 0;
		lat = 0;
		lng = 0;
	}
	Point::~Point()
	{

	}

	double Point::X()
	{
		return x;
	}
	double Point::Y()
	{
		return y;
	}
	double Point::Lat()
	{
		return lat;
	}
	double Point::Lng()
	{
		return lng;
	}
	void Point::SetLatLong(double dlat, double dlng)
	{
		lat = dlat;
		lng = dlng;
		CCoord	coord;
		coord.radians_lat_long_to_x_y(lat, lng, &x, &y);

		if (g_pCoord != NULL)
		{
			CCoordMgr* pCoord = g_pCoord;
			pCoord->WorldXY2ScreenXY(x, y, &x, &y);
		}
	}

	void Point::SetXY(double dx, double dy)
	{
		x = dx;
		y = dy;
		double wx;
		double wy;

		if (g_pCoord != NULL)
		{
			CCoordMgr* pCoord = g_pCoord;
			pCoord->ScreenXY2WorldXY((int)x, (int)y, &wx, &wy);
			CCoord	coord;
			coord.x_y_to_radians_lat_long(wx, wy, &lat, &lng);
		}
	}

	void Point::operator=(Point& point)
	{
		this->x = point.X();
		this->y = point.Y();
		this->lat = point.Lat();
		this->lng = point.Lng();
	}

	void Point::ReSetPoint()
	{
		CCoord	coord;
		coord.radians_lat_long_to_x_y(lat, lng, &x, &y);
		if (g_pCoord != NULL)
		{
			CCoordMgr* pCoord = g_pCoord;
			pCoord->WorldXY2ScreenXY(x, y, &x, &y);
		}
	}

	void Point::ReSetLatLong(double _lng, double _lat)
	{
		lat = _lat;
		lng = _lng;
	}
}
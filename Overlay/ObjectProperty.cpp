#include "pch.h"
#include "ObjectProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OVERLAY
{
	ObjectProperty::ObjectProperty()
		:LineColor(0xffff7fff)
		, FillColor(0xffff7fff)
		, eLineStyle(Solid)
		, FillStyle(0)
		, dLineWidth(4)
		, dAngle(0)
		, bIsFill(TRUE)
		, bIsLine(TRUE)
		, dTension(0.5)
	{
		InitProperty();
	}

	ObjectProperty::~ObjectProperty()
	{
	}

	void ObjectProperty::InitProperty()
	{
		//LineColor 	= 0x7f008A45;
		//FillColor 	= 0x7f008A45;
		LineColor = 0xff008A45;
		FillColor = 0x00000000;

		eLineStyle = Solid;
		FillStyle = 0;

		dLineWidth = 2;
		dAngle = 0;

		eStartLineStyle = LineCapFlat;
		eEndLineStyle = LineCapFlat;
		eDashStyle = DashCapFlat;
		bIsFill = TRUE;
		bIsLine = TRUE;

		dTension = 0.5;

		nStartLineArrowStyle = 0;
		nEndLineArrowStyle = 0;
	}

	void ObjectProperty::SetStartLineStyle(LineCap eLineStyle)
	{
		eStartLineStyle = eLineStyle;
	}

	void ObjectProperty::SetEndLineStyle(LineCap eLineStyle)
	{
		eEndLineStyle = eLineStyle;
	}

	void ObjectProperty::SetTension(double dTension_)
	{
		dTension = dTension_;
	}
}
// GDatum.cpp: implementation of the CGDatum class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"

#include "GDatum.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGDatum::CGDatum(int nDatum)
{
	this->SetDatum(nDatum);
}


CGDatum::~CGDatum()
{
}

void CGDatum::SetDatum(int nDatum)
{
	ASSERT(DATUM_FIRST <= nDatum && nDatum <= DATUM_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(DATUM_FIRST <= nDatum && nDatum <= DATUM_LAST)) return;

	switch(nDatum)
	{
	case DWGE :	//	116	// World Geodetic System 1984
		m_Name = "World Geodetic System 1984";
		m_Ellipsoid = EWGE;
		m_DX =    0.0;
		m_DY =    0.0;
		m_DZ =    0.0;
		break;
	case DTOK :	//	112	// Tokyo
		m_Name = "Tokyo";
		m_Ellipsoid = EBES;	
		m_DX = -148.0;
		m_DY =  507.0;
		m_DZ =  685.0;
//		m_DX = -128.0;
//		m_DY =  481.0;
//		m_DZ =  664.0;
		break;
	case DADI :	//	101	// Adindan
		m_Name = "Adindan";
		m_Ellipsoid = ECLJ;
		m_DX = -162.0;
		m_DY =  -12.0;
		m_DZ =  206.0;
		break;
	case DARF :	//	102	// Arc1950
		m_Name = "Arc1950";
		m_Ellipsoid = ECLJ;
		m_DX = -143.0;
		m_DY =  -90.0;
		m_DZ = -294.0;
		break;
	case DCAZ :	//	103	// Camp Area Astro
		m_Name = "Camp Area Astro";
		m_Ellipsoid = EINT;
		m_DX = -104.0;
		m_DY = -129.0;
		m_DZ =  239.0;
		break;
	case DEUR :	//	104	// European Datum 1950
		m_Name = "European Datum 1950";
		m_Ellipsoid = EINT;
		m_DX =  -87.0;
		m_DY =  -98.0;
		m_DZ = -121.0;
		break;
	case DENB :	//	105	// European Datum 1979
		m_Name = "European Datum 1979";
		m_Ellipsoid = EINT;
		m_DX =  -86.0;
		m_DY =  -98.0;
		m_DZ = -119.0;
		break;
	case DGEO :	//	106	// Geodetic Datum 1849
		m_Name = "Geodetic Datum 1849";
		m_Ellipsoid = EINT;
		m_DX =   84.0;
		m_DY =  -22.0;
		m_DZ =  209.0;
		break;
	case DHTN :	//	107	// Hu-Tzu-Shan
		m_Name = "Hu-Tzu-Shan";
		m_Ellipsoid = EINT;
		m_DX = -634.0;
		m_DY = -549.0;
		m_DZ = -201.0;
		break;
	case DIND :	//	108	// Indian
		m_Name = "Indian";
		m_Ellipsoid = EEVE;	
		m_DX =  289.0;
		m_DY =  737.0;
		m_DZ =  257.0;
		break;
	case DNAS :	//	109	// North American Datum 1927
		m_Name = "North American Datum 1927";
		m_Ellipsoid = ECLK;
		m_DX =   -8.0;
		m_DY =  160.0;
		m_DZ =  176.0;
		break;
	case DNAX :	//	110	// North American Datum 1983
		m_Name = "North American Datum 1983";
		m_Ellipsoid = EGRU;	
		m_DX =    0.0;
		m_DY =    0.0;
		m_DZ =    0.0;
		break;
	case DPRP :	//	111	// Provisional American 1969
		m_Name = "Provisional American 1969";
		m_Ellipsoid = EINT;
		m_DX = -288.0;
		m_DY =  175.0;	
		m_DZ = -376.0;
		break;
	case DWGC :	//	115	// World Geodetic System 1972
		m_Name = "World Geodetic System 1972";
		m_Ellipsoid = EWGC;	
		m_DX =    0.0;
		m_DY =    0.0;
		m_DZ =    5.0;
		break;
	}
}

int CGDatum::GetDatum() const
{
	int i;
	CGDatum tmpDatum;

	for (i = DATUM_FIRST; i <= DATUM_LAST; i++)
	{
		tmpDatum.SetDatum(i);
		if (this->m_Name == tmpDatum.GetName())
			return i;
	}
	return -1;
}


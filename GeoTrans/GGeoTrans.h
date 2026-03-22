// GGeoTrans.h: interface for the CGGeoTrans class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class CGDatum;
class CGEllipsoid;

class CGPointGP;
class CGPointGK;
class CGPointUPS;
class CGPointUTM;

class CGStringGeo;

class CGStringGP;
class CGStringGEOREF;
class CGStringUTM;
class CGStringMGRS;
class CGStringUPS;
class CGStringNE;
class CGStringGK;

/**
* 투영 변환 관리(경위도,UTM등 좌표변환) class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGGeoTrans
{
public:
	CGGeoTrans(int eInx = EWGE);	// default : WGS84
	virtual ~CGGeoTrans();

// Operations
public:
	double SetValidityGP(double gpValidity = 0.0000001);
	double SetValidityUTM(double utmValidity = 0.1);
	void   SetEllipsoid(int eInx);
	int    GetEllipsoid() const;

	// Coordinate Transformation
	void GeoConvert(CGPointGeo *pDesPtGeo, CGPointGeo *pSrcPtGeo, int nCount = 1);
	void GeoConvert(CGPointGeo *pDesPtGeo, CGStringGeo *pSrcStrGeo, int nCount = 1);
	void GeoConvert(CGStringGeo *pDesStrGeo, CGPointGeo *pSrcPtGeo, int nCount = 1);
	void GeoConvert(CGStringGeo *pDesStrGeo, CGStringGeo *pSrcStrGeo, int nCount = 1);
	
	//
	// CGPointGP(지리좌표 포인트)로 변환
	void toPointGP(CGPointGP *pGP, CGPointGP *pSrcGP, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGPointUTM *pUTM, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGPointGK *pGK, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGPointUPS *pUPS, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringGP *pStrGP, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringUPS *pStrUPS, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringGK *pStrGK, int nCount = 1);
	void toPointGP(CGPointGP *pGP, CGStringNE *pStrNE, int nCount = 1);

	//
	// CGPointUTM(UTM 좌표 포인트)로 변환
	void toPointUTM(CGPointUTM *pUTM, CGPointGP *pGP, int nCount = 1, int nZone = -1);
	void toPointUTM(CGPointUTM *pUTM, CGPointUTM *pSrcUTM, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGPointGK *pGK, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGPointUPS *pUPS, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringGP *pStrGP, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringUPS *pStrUPS, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringGK *pStrGK, int nCount = 1);
	void toPointUTM(CGPointUTM *pUTM, CGStringNE *pStrNE, int nCount = 1);

	//
	// CGPointGK(GK 좌표 포인트)로 변환
	void toPointGK(CGPointGK *pGK, CGPointGP *pGP, int nCount = 1, int nZone = -1);
	void toPointGK(CGPointGK *pGK, CGPointUTM *pUTM, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGPointGK *pSrcGK, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGPointUPS *pUPS, int nCount = 1);	// 같은 범위 없음
	void toPointGK(CGPointGK *pGK, CGStringGP *pStrGP, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGStringUPS *pStrUPS, int nCount = 1);	// 같은 범위 없음
	void toPointGK(CGPointGK *pGK, CGStringGK *pStrGK, int nCount = 1);
	void toPointGK(CGPointGK *pGK, CGStringNE *pStrNE, int nCount = 1);

	//
	// CGPointUPS(UPS 좌표 포인트)로 변환
	void toPointUPS(CGPointUPS *pUPS, CGPointGP *pGP, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGPointUTM *pUTM, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGPointGK *pGK, int nCount = 1);	// 같은 범위 없음
	void toPointUPS(CGPointUPS *pUPS, CGPointUPS *pSrcUPS, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGStringGP *pStrGP, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGStringUPS *pStrUPS, int nCount = 1);
	void toPointUPS(CGPointUPS *pUPS, CGStringGK *pStrGK, int nCount = 1);	// 같은 범위 없음
	void toPointUPS(CGPointUPS *pUPS, CGStringNE *pStrNE, int nCount = 1);

	//
	// CGStringGP(지리좌표 문자열)로 변환
	void toStringGP(CGStringGP *pStrGP, CGPointGP *pGP, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGPointUTM *pUTM, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGPointGK *pGK, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGPointUPS *pUPS, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringGP *pSrcStrGP, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringUPS *pStrUPS, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringGK *pStrGK, int nCount = 1);
	void toStringGP(CGStringGP *pStrGP, CGStringNE *pStrNE, int nCount = 1);

	//
	// GEOREF 좌표 문자열로 변환
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointGP *pGP, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointUTM *pUTM, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointGK *pGK, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointUPS *pUPS, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGP *pStrGP, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGEOREF *pSrcStrGEOREF, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringUPS *pStrUPS, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGK *pStrGK, int nCount = 1);
	void toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringNE *pStrNE, int nCount = 1);

	//
	// MGRS 좌표 문자열로 변환
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGPointGP *pGP, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGPointUTM *pUTM, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGPointGK *pGK, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGPointUPS *pUPS, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGP *pStrGP, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringMGRS *pSrcStrMGRS, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringUPS *pStrUPS, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGK *pStrGK, int nCount = 1);
	void toStringMGRS(CGStringMGRS *pStrMGRS, CGStringNE *pStrNE, int nCount = 1);

	//
	// UPS 좌표 문자열로 변환
	void toStringUPS(CGStringUPS *pStrUPS, CGPointGP *pGP, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGPointUTM *pUTM, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGPointGK *pGK, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGPointUPS *pUPS, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringGP *pStrGP, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringUPS *pSrcStrUPS, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringGK *pStrGK, int nCount = 1);
	void toStringUPS(CGStringUPS *pStrUPS, CGStringNE *pStrNE, int nCount = 1);

	//
	// GK 좌표 문자열로 변환
	void toStringGK(CGStringGK *pStrGK, CGPointGP *pGP, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGPointUTM *pUTM, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGPointGK *pGK, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGPointUPS *pUPS, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringGP *pStrGP, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringUPS *pStrUPS, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringGK *pSrcStrGK, int nCount = 1);
	void toStringGK(CGStringGK *pStrGK, CGStringNE *pStrNE, int nCount = 1);

	//
	// NE 좌표 문자열로 변환
	void toStringNE(CGStringNE *pStrNE, CGPointGP *pGP, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGPointUTM *pUTM, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGPointGK *pGK, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGPointUPS *pUPS, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringGP *pStrGP, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringGEOREF *pStrGEOREF, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringMGRS *pStrMGRS, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringUPS *pStrUPS, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringGK *pStrGK, int nCount = 1);
	void toStringNE(CGStringNE *pStrNE, CGStringNE *pSrcStrNE, int nCount = 1);

	void StdMolodensky(CGPointGP *toGP, CGDatum *toDatum,
								CGPointGP *fromGP, CGDatum *fromDatum,
								double *toHeight = NULL, double *fromHeight = NULL);


private:
	CGEllipsoid m_ellipsoid;
	double		m_gpValidity;
	double		m_utmValidity;

private:
	void RoundValidity(CGPointGP *pGP, int nCount = 1);
	void RoundValidity(CGPointUTM *pUTM, int nCount = 1);
};

#undef	AFX_DATA
#define	AFX_DATA



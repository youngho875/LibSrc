
#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGeoObjUTM

class CGGeoObject;

class CGPointGeo;
class CGSizeGeo;
class CGRectGeo;

class CGPointUTM;
class CGSizeUTM;
class CGRectUTM;

/**
* 투영 관리(UTM좌표계) class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGGeoObjUTM : public CGGeoObject
{
public:
	CGGeoObjUTM();
	virtual ~CGGeoObjUTM();

// Operations
public:
	virtual CGPointGeo	*Point(int nPoints = 1)
		{ return ((nPoints==1) ? new CGPointUTM
				: new CGPointUTM[nPoints]); };
	virtual CGSizeGeo	*Size() { return new CGSizeUTM; };
	virtual CGRectGeo	*Rect() { return new CGRectUTM; };

	// projection operator
	virtual void Forward(LPPOINT, CGPointGeo *, int nPoints = 1);
	virtual void Forward(LPSIZE, CGSizeGeo *);
	virtual void Forward(LPRECT, CGRectGeo *);
	virtual void Inverse(CGPointGeo *, LPPOINT, int nPoints = 1);
	virtual void Inverse(CGSizeGeo *, LPSIZE);
	virtual void Inverse(CGRectGeo *, LPRECT);
};

#undef	AFX_DATA
#define	AFX_DATA



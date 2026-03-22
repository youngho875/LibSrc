// GGeoGK.h: interface for the CGGeoGK class.
//
//////////////////////////////////////////////////////////////////////
#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

class CGPointGeo;

/**
* GK 좌표계 point 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGPointGK : public CGPointGeo
{
public:
	CGPointGK();
	CGPointGK(char zone, double easting, double northing,
			   VERHEMISPHERE hemisphere = NORTH);
	virtual ~CGPointGK();

// Attributes
public:
	POINTGK		m_P;

// Operations
public:
	void Empty();
	BOOL IsEmpty() const;

	virtual void SetPoint(CGPointGeo *pPoint);
	virtual void SetPoint(POINTGK *gk);
	virtual void SetPoint(CGPointGK *pGK);
	virtual void SetPoint(char zone, double easting, double northing,
						   VERHEMISPHERE hemisphere = NORTH);
//	virtual void	Offset(CGSizeGeo *);
//	virtual void	Offset(SIZEUTM *);

	BOOL operator==(CGPointGK& right) const;
	BOOL operator!=(CGPointGK& right) const;

	void Offset(double x, double y, int nUnit = GUNIT_M);

// Operators
public:
	const CGPointGK& operator = (const CGPointGK &right);

protected:
	CString GetString() const;


};


#undef	AFX_DATA
#define	AFX_DATA



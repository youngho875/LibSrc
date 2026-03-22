// GGeoUPS.h: interface for the CGGeoUPS class.
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
* UPS 좌표계 point 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGPointUPS : public CGPointGeo
{
public:
	CGPointUPS();
	virtual ~CGPointUPS();
	CGPointUPS(double easting, double northing,
						VERHEMISPHERE hemisphere = NORTH);

// Attributes
public:
	POINTUPS	m_P;

// Operations
public:
	void Empty();
	BOOL IsEmpty() const;

	virtual void	SetPoint(CGPointGeo *pPoint);
	virtual void	SetPoint(POINTUPS *ups);
	virtual void	SetPoint(double easting, double northing,
							 VERHEMISPHERE hemisphere = NORTH);
//	virtual void	Offset(CGSizeGeo *);
//	virtual void	Offset(SIZEUPS *);

	BOOL operator==(CGPointUPS& right) const;
	BOOL operator!=(CGPointUPS& right) const;

// Operators
public:
	const CGPointUPS& operator = (const CGPointUPS &right);

protected:
	CString GetString() const;


};

#undef	AFX_DATA
#define	AFX_DATA



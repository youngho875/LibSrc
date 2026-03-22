
#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

//
// define geometric mapping mode
#define	GMM_FIRST	9	// MM_ANISOTROPIC + 1
#define	GMM_GP		9	// 경위도 좌표계
#define	GMM_UTM		10	// UTM
#define	GMM_UPS		11	// UPS
#define GMM_TM		12	// 
#define	GMM_LCC		13	// Lambert Comfirmal Conic Projection
#define	GMM_MER		14	// Mercator Peojection
#define	GMM_LAST	14

class CGPointGeo;
class CGSizeGeo;
class CGRectGeo;

class CGPointGP;
class CGSizeGP;
class CGRectGP;

class CGGeoProj;

class CGGeoObject;
class CGPointPlane;

/////////////////////////////////////////////////////////////////////////////
// CGGeoObject

//class CGGeoObject;

/**
* 투영 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGGeoObject
{
protected:
	CGGeoObject(int nPointType); // protected constructor for abstract class

// Attributes
public:
	const int	m_nPointType;	// point type
	int	SetEllipsoid(int eInx);
	int	GetEllipsoid() const;
	double	SetValidity(double validity);

protected:
	int m_nMapMode;	// mapping mode (투영법)
	CGGeoProj	m_GeoProj;	// projection method
	double		m_Validity;	// 유효숫자 갯수
							// POINTOLANE 값을 POINT로 변환할 때
							// 이 값을 나눈다.

// Operations
public:
	double		SetProjScaleFactor(double dScaleFactor);
	double		GetProjScaleFactor() const;
	CGPointGP	SetProjBase(CGPointGP gp);
	CGPointGP	SetProjBase(POINTGP gpBase);
	CGPointGP	SetProjBase(double lonBase, double latBase);
	CGPointGP	GetProjBase() const;

	// Lambert Conformal Conic Projection
	void		SetStdParallel(double dStdParallel1, double dStdParallel2);

	int GetMapMode() const;
	int SetMapMode(int nMapMode);
	virtual ~CGGeoObject();
	virtual CGPointGeo	*Point(int nPoint = 1);
	virtual CGSizeGeo	*Size() {ASSERT(FALSE); return NULL;};
	virtual CGRectGeo	*Rect() {ASSERT(FALSE); return NULL;};

	// projection operator
	virtual void Forward(LPPOINT, CGPointGeo *, int nPoints = 1);
	virtual void Inverse(CGPointGeo *, LPPOINT, int nPoints = 1);
};

/////////////////////////////////////////////////////////////////////////////
// CGPointPlane

/**
* 평면 좌표계 point 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGPointPlane : public tagPOINTPLANE
{
public:
	CGPointPlane() :m_nPointType(GSTYPE_PLANE) {};	// protected constructor for abstract class
	virtual ~CGPointPlane() {};

// Attributes
public:
	const int	m_nPointType;	// point type

	double x; // Example data member
	double y; // Example data member

// operator
public:
//	virtual const CGRectGeo& operator = (const CGRectGeo &right) = NULL;
	virtual void	SetPoint(CGPointPlane *) {};
	virtual void	Offset(CGPointPlane *) {};

// Operators
public:
	const CGPointPlane& operator = (const CGPointPlane &right) 
	{	
		//*this = right; 
		//return *this;	

		if (this != &right) // Self-assignment check
		{
			// Copy data members from right to this instance
			this->x = right.x; // Example member
			this->y = right.y; // Example member
		}
		return *this;

	};
};

#undef	AFX_DATA
#define	AFX_DATA



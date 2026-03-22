// GStringGK.h: interface for the CGStringGK class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define GUNIT_GK_LAST	12
#define GUNIT_GK_12		12	// 42190000 224252000	: 0.1  미터 단위
#define GUNIT_GK_10		10	// 4219000 22425200		: 1    미터 단위
#define GUNIT_GK_8		8	// 421900 2242520		: 10   미터 단위
#define GUNIT_GK_6		6	// 42190 224252			: 100  미터 단위
#define GUNIT_GK_4		4	// 4219 22425			: 1000 미터 단위
#define GUNIT_GK_2		2	// 421 2242				: 1만  미터 단위
#define GUNIT_GK_0		0	// 42 224				: 10만 미터 단위
#define GUNIT_GK_FIRST  0

class CGStringGeo;
class CGPointGK;

/**
* GK 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringGK : public CGStringGeo
{
public:
	CGStringGK();
	CGStringGK(CString str, VERHEMISPHERE hemisphere  = NORTH);
	virtual ~CGStringGK();

	const CGStringGK &operator = (const CGStringGK &right);

public:
	void	Empty();
	BOOL	IsEmpty() const;

	int	IsValid(CString &str, BOOL bSet = FALSE);
	BOOL	IsValidZone(BYTE zone);

	BOOL	SetPoint(CString &str, VERHEMISPHERE hemisphere  = NORTH);
	BOOL	SetPoint(CGPointGK *pGK, int nUnit = GUNIT_GK_10);

	void	GetPoint(CGPointGK *pGK);

	CString	GetString(int nUnit = GUNIT_GK_10) const;

	BOOL operator == (CGStringGK right) const;
	BOOL operator != (CGStringGK right) const;

// Attributes
private:
	VERHEMISPHERE	m_hemisphere;	
	BYTE			m_zone;
	double			m_x;
	double			m_y;

	BYTE			m_unit;
};

#undef	AFX_DATA
#define	AFX_DATA



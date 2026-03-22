// GStringUPS.h: interface for the CGStringUPS class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif


#define GUNIT_UPS_LAST	12
#define GUNIT_UPS_12	12	// Y CJ 712435 091945	: 0.1  미터 단위(12단계)
#define GUNIT_UPS_10	10	// Y CJ 71243 09194		: 1    미터 단위(10단계)
#define GUNIT_UPS_8		8	// Y CJ 7124 0919		: 10   미터 단위(8 단계)
#define GUNIT_UPS_6		6	// Y CJ 712 091			: 100  미터 단위(6 단계)
#define GUNIT_UPS_4		4	// Y CJ 71 09			: 1000 미터 단위(4 단계)
#define GUNIT_UPS_2		2	// Y CJ 7 0				: 1만  미터 단위(2 단계)
#define GUNIT_UPS_0		0	// Y CJ					: 10만 미터 단위(0 단계)
#define GUNIT_UPS_FIRST 0

#define UPS_EASTING		1
#define UPS_NORTHING	2

class CGStringGeo;
class CGPointUPS;

/**
* UPS 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringUPS : public CGStringGeo
{
public:
	CGStringUPS();
	CGStringUPS(CString str);
	virtual ~CGStringUPS();

	const CGStringUPS &operator = (const CGStringUPS &right);

public:
	void	Empty();
	BOOL	IsEmpty() const;

	int	IsValid(CString &str, BOOL bSet = TRUE);

	BOOL	SetPoint(CString &str);
	BOOL	SetPoint(CGPointUPS *pUPS, int nUnit = GUNIT_UPS_10);
	void	GetPoint(CGPointUPS *pUPS);

	CString	GetString(int nUnit = -1) const;

	BOOL operator == (CGStringUPS right) const;
	BOOL operator != (CGStringUPS right) const;

private:
	BOOL IsValid100KmChar(BYTE ch, int nAxis);

private:
	BYTE	m_zone;	// 북반구(Y, Z), 남반구(A, B)
	BYTE	m_xch;	// 100,000 Meter UPS Square의 East  Designator
	double	m_x;	// 100,000 Meter를 더 세분하여 나눈 East  Meter
	BYTE	m_ych;	// 100,000 Meter UPS Square의 North Designator
	double	m_y;	// 100,000 Meter를 더 세분하여 나눈 North Meter

	BYTE	m_nUnit;	// MGRS좌표의 단계(정밀도)를 정의한다.
};

#undef	AFX_DATA
#define	AFX_DATA



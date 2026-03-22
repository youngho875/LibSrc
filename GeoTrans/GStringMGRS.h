
#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CGStringMGRS

#define MGRS_EASTING	1
#define MGRS_NORTHING	2

#define GUNIT_MGRS_LAST 12
#define GUNIT_MGRS_12	12	// 52S CJ 712431 091941	: 0.1  미터 단위(12단계)
#define GUNIT_MGRS_10	10	// 52S CJ 71243 09194	: 1    미터 단위(10단계)
#define GUNIT_MGRS_8	8	// 52S CJ 7124 0919		: 10   미터 단위(8 단계)
#define GUNIT_MGRS_6	6	// 52S CJ 712 091		: 100  미터 단위(6 단계)
#define GUNIT_MGRS_4	4	// 52S CJ 71 09			: 1000 미터 단위(4 단계)
#define GUNIT_MGRS_2	2	// 52S CJ 7 0			: 1만  미터 단위(2 단계)
#define GUNIT_MGRS_0	0	// 52S CJ				: 10만 미터 단위(0 단계)
#define GUNIT_MGRS_FIRST 0
 
class CGStringGeo;
class CGPointUTM;

/**
* MGRS 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringMGRS : public CGStringGeo
{
public:
	CGStringMGRS();
	CGStringMGRS(CString str, int eInx = EWGE /* WGS84 */);
	virtual ~CGStringMGRS();

	const CGStringMGRS &operator = (const CGStringMGRS &right);

public:
	void	Empty();
	BOOL	IsEmpty() const;
	BOOL	IsValidDesignator(BYTE dgn);
	int	IsValid(CString &str, int eInx = EWGE /* WGS84 */, BOOL bSet = FALSE);

	BOOL	SetPoint(CString &str, int eInx = EWGE /* WGS84 */);
	BOOL	SetPoint(CGPointUTM *pUtm, int eInx = EWGE /* WGS84 */);
	void	GetPoint(CGPointUTM *pUTM, int eInx = EWGE);

	CString	GetString(int nUnit = -1) const;
	CString	Get100kmSquare() const;

	BOOL operator == (CGStringMGRS right) const;
	BOOL operator != (CGStringMGRS right) const;

	// by Peter 2005-11-21 함수 추가
	void  GetMGRS10(unsigned char* szZone, unsigned char* szXY, unsigned* pnX, unsigned* pnY)
	{
		double dblRest = 0.0f;

		*szZone = *szXY = 0x00;

		sprintf((char *)szZone, "%02d", m_zone);
		++szZone;
		*(++szZone) = m_dgn;
		*(++szZone) = 0x00;

		*szXY = m_xch;
		*(++szXY) = m_ych;
		*(++szXY) = 0x00;

		dblRest = m_x - (int)(m_x);
		*pnX = (dblRest >= 0.6) ? (int)m_x + 1 : (int)m_x;
		
		dblRest = m_y - (int)(m_y);
		*pnY = (dblRest >= 0.6) ? (int)m_y + 1 : (int)m_y;

		return;
	}


protected:
	BOOL	IsValid100KmChar(BYTE ch, int nAxis, int nZone);

// Attributes
protected:
	BYTE	m_zone;	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	BYTE	m_dgn;	// // 위도 : 80도S - 84도N 구간을 8도 간격으로 구분하는 문자
	BYTE	m_xch;	// 100,000 Meter MGRS Square의 East  Designator
	double	m_x;	// 100,000 Meter를 더 세분하여 나눈 East  Meter
	BYTE	m_ych;	// 100,000 Meter MGRS Square의 North Designator
	double	m_y;	// 100,000 Meter를 더 세분하여 나눈 North Meter

	BYTE	m_nUnit;	// MGRS좌표의 단계(정밀도)를 정의한다.
	int		m_ellipse;	// 설정된 타원체 값을 가진다.


//friend class CGPointUTM;

};

#undef	AFX_DATA
#define	AFX_DATA



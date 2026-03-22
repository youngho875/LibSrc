// GStringGEOREF.h: interface for the CGStringGEOREF class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define MIN_RANGE_MIN	0
#define MAX_RANGE_MIN	59

#define GUNIT_GEOREF_FIRST 0
#define GUNIT_GEOREF_0	0	// WJ HJ 308 553	: 1/10분 격자(6초)
#define GUNIT_GEOREF_1	1	// WJ HJ 30 55		: 1   분 격자
#define GUNIT_GEOREF_2	2	// WJ HJ 3 5		: 1/6 도 격자(10분)
#define GUNIT_GEOREF_3	3	// WJ HJ			: 1   도 격자
#define GUNIT_GEOREF_4	4	// WJ				: 15  도 격자
#define GUNIT_GEOREF_LAST 4

class CGStringGeo;
class CGPointGP;

/**
* GEOREF 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringGEOREF : public CGStringGeo
{
public:
	CGStringGEOREF();
	CGStringGEOREF(CString str);
	virtual ~CGStringGEOREF();

	const CGStringGEOREF &operator = (const CGStringGEOREF &right);

public:
	int	IsValid(CString &str, BOOL nSet = FALSE);
	void	Empty();
	BOOL	IsEmpty() const;

	BOOL	SetPoint(CString &str);
	BOOL	SetPoint(CGPointGP *pGP);
	void	GetPoint(CGPointGP *pGP);

	CString	GetString(int nUnit = GUNIT_GEOREF_0) const;

	// by Peter 2005-11-21 함수 추가
	void   	GetGEOREF(unsigned char* psz15D, unsigned char* psz1D, 
                                  int* pn60SLon, int* pn60SLat)	{
		*psz15D = *psz1D = 0x00;

		*psz15D = m_15DLon;
		*(++psz15D) = m_15DLat;
		*(++psz15D) = 0x00;

		*psz1D = m_1DLon;
		*(++psz1D) = m_1DLat;
		*(++psz1D) = 0x00;

		*pn60SLon = (int)( ((long)m_1MLon * 10) + (m_60SLon * 10.0f) );
		*pn60SLat = (int)( ((long)m_1MLat * 10) + (m_60SLat * 10.0f) );
		
		return;
	}

protected:
	int  Is1MinSecQuardrange(CString &str, BOOL bSet = FALSE);
	int  Is15DegQuardrange(CString &str, BOOL bSet = FALSE);
	int  IsOneDegQuardrange(CString &str, BOOL bSet = FALSE);
	double GetLongitude();
	double GetLatitude();

// Attributes
private:
	BYTE	m_15DLon; // 경도를 15도 간격으로 나눈 INDEX값(24개 문자)
	BYTE	m_1DLon;  // 경도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	BYTE	m_1MLon;  // 00 ~ 59 사이의 값 (경도 1도를 1분 단위로 분할한 값)
	double	m_60SLon; // 초(second) : 0.0 <= m_60SLon < 60.0

	BYTE	m_15DLat; // 위도를 15도 간격으로 나눈 INDEX값(12개 문자)
	BYTE	m_1DLat;  // 위도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	BYTE	m_1MLat;  // 00 ~ 59 사이의 값 (위도 1도를 1분 단위로 분할한 값)
	double	m_60SLat; // 초(second) : 0.0 <= m_60SLon < 60.0
};

#undef	AFX_DATA
#define	AFX_DATA



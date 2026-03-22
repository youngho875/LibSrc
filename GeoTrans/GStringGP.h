// GStringGP.h: interface for the CGStringGP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define MIN_RANGE	0
#define MAX_RANGE_LON	180
#define MAX_RANGE_LAT	90
#define MAX_RANGE_MIN	59
#define MAX_RANGE_SEC	59

#define MIN_LEN_LON 5
#define MAX_LEN_LON 7

#define MIN_LEN_LAT 5
#define MAX_LEN_LAT 6

#define GLONGITUDE	11
#define GLATITUDE	12
#define GBOTH		13

#define GDEG	21
#define GMIN	22
#define	GSEC	23

class CGStringGeo;
class CGPointGP;

/**
* GP 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringGP : public CGStringGeo
{
public:
	CGStringGP();
	CGStringGP(CString str);
	virtual ~CGStringGP();

	const CGStringGP &operator = (const CGStringGP &right);

public:
	void	Empty();
	BOOL	IsEmpty() const;
	int	IsValid(CString &str, BOOL nSet = FALSE);

	BOOL	SetPoint(CString &str);
	BOOL	SetPoint(CGPointGP *pGP);
	void	GetPoint(CGPointGP *pGP);

	CString	GetString(int nDecPoint = 3) const;
	CString	GetLongitude(int nDecPoint = 3) const;
	CString	GetLatitude(int nDecPoint = 3) const;

	BOOL	Add(int nDeg, int nMin, double dSec, UINT nLonLat);
	BOOL	Add(int nUnit, UINT nPart, UINT nLonLat);
	void	Clear(int nUnit, UINT nPart, UINT nLonLat);

	BOOL operator==(CGStringGP right) const;
	BOOL operator!=(CGStringGP right) const;

// by Peter 2005-11-21 함수 추가
	void	GetLongLat(unsigned* pnDegLon, unsigned* pnMinLon, double* pdlbSecLon,
                               unsigned* pnDegLat, unsigned* pnMinLat, double* pdlbSecLat)
	{
		unsigned nDegLon = m_degLon;		unsigned nDegLat = m_degLat;
		unsigned nMinLon = m_minLon;		unsigned nMinLat = m_minLat;
		double   dblSecLon = m_secLon;		double   dblSecLat = m_secLat;
	
		// Longitude
		if (dblSecLon >= 60.0f) {
			++nMinLon;
			dblSecLon -= 60.0f;
		}
		*pdlbSecLon = dblSecLon;

		if (nMinLon >= 60) {
			++nDegLon;
			nMinLon -= 60;
		}
		*pnMinLon = nMinLon;
		*pnDegLon = nDegLon;

		// Latitude
		if (dblSecLat >= 60.0f) {
			++nMinLat;
			dblSecLat -= 60.0f;
		}
		*pdlbSecLat = dblSecLat;

		if (nMinLat >= 60) {
			++nDegLat;
			nMinLat -= 60;
		}
		*pnMinLat = nMinLat;
		*pnDegLat = nDegLat;

		return;
	}

protected:
	int SetLonHemisphere(CString &str, BOOL bSet = FALSE);
	int SetLatHemisphere(CString &str, BOOL bSet = FALSE);
	BOOL IsInRangeLatitude(BYTE nDeg, BYTE nMin, BYTE nSec);
	BOOL IsInRangeLongitude(BYTE nDeg, BYTE nMin, BYTE nSec);
	int  SetDegMinSec(CString &str, BOOL bIsLon, BOOL bSet = FALSE);

// Attributes
private:
	HORHEMISPHERE	m_EasWes;   // EAST : 동경, WEST : 서경
	UINT			m_degLon;	// 도
	UINT			m_minLon;	// 분
	double			m_secLon;	// 초

	VERHEMISPHERE	m_NorSou;   // NORTH : 북위, SOUTH : 남위
	UINT			m_degLat;	// 도
	UINT			m_minLat;	// 분
	double			m_secLat;	// 초


};

#undef	AFX_DATA
#define	AFX_DATA



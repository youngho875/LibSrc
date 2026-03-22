// GStringNE.h: interface for the CGStringNE class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif

#define GMAX_NE_N	46	// N 구역번호의 최고값
#define GMAX_NE_E	61	// E 구역번호의 최고값
#define GMAX_MONTH	12
#define ZERO		0

typedef struct tagNETABLECOMPONENT {
	char	Nchar[3];
	char	Echar[3];
} tagNETABLECOMPONENT, TABLECOMPONENT;
typedef  tagNETABLECOMPONENT NETABLE;

#define GEXP_NE_POS			701	// 위치
#define GEXP_NE_AREA_BASE	702	// 기본구역
#define GEXP_NE_AREA_RADIUS	703	// 반경
#define GEXP_NE_AREA_RECT	704	// 영역
#define GEXP_NE_REFLINE		705	// 참조선

class CGStringGeo;

class CGPointGeo;
class CGPointGP;

/**
* NE 좌표계 TEXT 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGStringNE : public CGStringGeo
{
public:
	CGStringNE();
	virtual ~CGStringNE();

	void Empty();
	BOOL IsEmpty() const;
	int IsValid(CString &str, BOOL bSet = FALSE);

	BOOL SetPoint(CGPointGP *pGP);
	BOOL SetPoint(CString &str);

	void GetPoint(CGPointGP *pGP);
	CString GetString();

	const CGStringNE& operator = (const CGStringNE &right);

	CGPointGP GetBasisGP() const;
	CGPointGP SetBasisGP(CGPointGP gpBasis);

	int SetMonth(int nMonth = 0);
	int GetMonth() { return m_nMonth; }

	NETABLE* GetNeTable();

	int FindNEtable(CString str, BOOL bN, int month = 0);

	void InitializeNeTable();
	BOOL ReadNeTableFormFile(CString *psAppDate, CString *psBaseGP, int *pnCntN, int *pnCntE, CStringArray *psArrN, CStringArray *psArrE);

private:
	int IsValidPos(CString &str, BOOL bSet = FALSE);
	int IsValidArea(CString &str, BOOL bSet = FALSE);
	int IsValidRefLine(CString &str, BOOL bSet = FALSE);

private:
	BOOL		m_bRead;

	CGPointGP	m_gpBasis;
	UINT		m_nMonth;

private:
	NETABLE	m_NeTable[GMAX_NE_E][GMAX_MONTH];

	BYTE	m_Narea;	// N 구역번호
	UINT	m_Nsec;		// N 초
	BYTE	m_Earea;	// E 구역번호
	UINT	m_Esec;		// E 초

	// NE 좌표 표기 방법
	UINT	m_expNE;
	
	UINT	m_mile;		// 마일
	UINT	m_Ncount;	// N 구역의 개수
	UINT	m_Ecount;	// E 구역의 개수
};

#undef	AFX_DATA
#define	AFX_DATA



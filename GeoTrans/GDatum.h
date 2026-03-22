// GDatum.h: interface for the CGDatum class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#undef	AFX_DATA
#ifdef	GEOTRANS_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif


/**
* 좌표 기준점 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class AFX_DATA CGDatum
{
public:
	CGDatum(int nDatum = DWGE);
	virtual ~CGDatum();

	CString	GetName() const { return this->m_Name; };
	int		GetEllipsoid() const { return this->m_Ellipsoid; };
	double	GetDX() const { return this->m_DX; };
	double	GetDY() const { return this->m_DY; };
	double	GetDZ() const { return this->m_DZ; };

	void SetDatum(int nDatum);
	int  GetDatum() const;

private:
	CString m_Name;			// 데이텀 명
	int		m_Ellipsoid;	// 타원체의 인덱스
	double	m_DX;
	double	m_DY;
	double	m_DZ;
};

#undef	AFX_DATA
#define	AFX_DATA



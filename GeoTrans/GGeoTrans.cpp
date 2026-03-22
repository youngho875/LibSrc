// GGeoTrans.cp	p: implementation of the CGGeoTrans class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GGeoObjStruct.h"
#include "GDatum.h"
#include "GEllipsoid.h"

#include "GGeoAbst.h"
#include "GGeoGP.h"
#include "GGeoGK.h"
#include "GGeoUPS.h"
#include "GGeoUTM.h"

#include "GStringGeo.h"
#include "GStringGP.h"
#include "GStringGEOREF.h"
#include "GStringGK.h"
#include "GStringMGRS.h"
#include "GStringUPS.h"
#include "GStringNE.h"

#include "GGeoProj.h"

#include "GGeoTrans.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGGeoTrans::CGGeoTrans(int eInx)
{
	this->m_ellipsoid.SetEllipsoid(eInx); // default WGS84 Ellipsoid
	this->m_gpValidity  = 0.0000001;	// default 소수점 이하 7자리까지
	this->m_utmValidity = 0.1;			// default 소수점 이하 1자리까지
}

CGGeoTrans::~CGGeoTrans()
{
}

double CGGeoTrans::SetValidityGP(double gpValidity)
{
	double prevGpValidity = this->m_gpValidity;
	this->m_gpValidity = gpValidity;
	return prevGpValidity;
}

double CGGeoTrans::SetValidityUTM(double utmValidity)
{
	double prevUtmValidity = this->m_utmValidity;
	this->m_utmValidity = utmValidity;
	return prevUtmValidity;
}

void CGGeoTrans::RoundValidity(CGPointGP *pGP, int nCount)
{
	int i = 0;

	if (this->m_gpValidity != 1.0)	{
		for ( i = 0; i < nCount; i++ )	{
			pGP[i].m_P.lon /= this->m_gpValidity;
			pGP[i].m_P.lat /= this->m_gpValidity;
		}
	}

	for ( i = 0; i < nCount; i++ )	{
		pGP[i].m_P.lon = (int) (pGP[i].m_P.lon + 0.5);
		pGP[i].m_P.lat = (int) (pGP[i].m_P.lat + 0.5);
	}

	if (this->m_gpValidity != 1.0)	{
		for ( i = 0; i < nCount; i++ )	{
			pGP[i].m_P.lon *= this->m_gpValidity;
			pGP[i].m_P.lat *= this->m_gpValidity;
		}
	}
}

void CGGeoTrans::RoundValidity(CGPointUTM *pUTM, int nCount)
{
	int i = 0;

	if (this->m_utmValidity != 1.0)	{
		for ( i = 0; i < nCount; i++ )	{
			pUTM[i].m_P.x /= this->m_utmValidity;
			pUTM[i].m_P.y /= this->m_utmValidity;
		}
	}

	for ( i = 0; i < nCount; i++ )	{
		pUTM[i].m_P.x = (int) (pUTM[i].m_P.x + 0.5);
		pUTM[i].m_P.y = (int) (pUTM[i].m_P.y + 0.5);
	}

	if (this->m_utmValidity != 1.0)	{
		for ( i = 0; i < nCount; i++ )	{
			pUTM[i].m_P.x *= this->m_utmValidity;
			pUTM[i].m_P.y *= this->m_utmValidity;
		}
	}
}

void CGGeoTrans::SetEllipsoid(int eInx)
{
	this->m_ellipsoid.SetEllipsoid(eInx);
}

int CGGeoTrans::GetEllipsoid() const
{
	return this->m_ellipsoid.GetEllipsoid();
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGPointGP *pSrcGP, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pSrcGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcGP)) return;

	for (int i = 0; i < nCount; i++)
		pGP[i] = pSrcGP[i];
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGGeoProj utmProj;
    //utmProj.SetMapMode(GMM_UTM);	// GMM_UTM : UTM투영을 한 경우
	utmProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid());		// 타원체를 WGS84로 지정함
	utmProj.SetProjBase(129.0, 0.0);	// 52 zone을 기준점으로 설정
	utmProj.SetProjScaleFactor(UTM_SCALE_FACTOR);	// UTM 투영의 Scale Factor

	// CGPointUTM 좌표를  CGPointGP 좌표로 변환한다.
	utmProj.InverseUTM(pGP, pUTM, nCount);

	// 유효숫자의 이하에서 반올림한다.
	this->RoundValidity(pGP, nCount);
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGPointGK *pGK, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGGeoProj gkProj;
	gkProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid()); // 타원체 설정
	gkProj.SetProjBase(129.0, 0.0);	// 52 zone을 기준점으로 설정
	gkProj.SetProjScaleFactor(GK_SCALE_FACTOR);	// GK 투영의 Scale Factor

	// CGPointGK 좌표를  CGPointGP 좌표로 변환한다.
	gkProj.InverseGK(pGP, pGK, nCount);

	// 유효숫자의 이하에서 반올림한다.
	this->RoundValidity(pGP, nCount);
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	CGGeoProj upsProj;
	upsProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid()); // 타원체 설정
	upsProj.SetProjBase(0.0, 90.0);	// 북극을 기준점으로 설정
	upsProj.SetProjScaleFactor(UPS_SCALE_FACTOR);	// GK 투영의 Scale Factor

	// CGPointGK 좌표를  CGPointGP 좌표로 변환한다.
	upsProj.InverseUPS(pGP, pUPS, nCount);

	// 유효숫자의 이하에서 반올림한다.
	this->RoundValidity(pGP, nCount);
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	// CGSrtingGP 좌표를  CGPointGP 좌표로 변환한다.
	for ( int i = 0; i < nCount; i++ )
	{
		pStrGP[i].GetPoint(&pGP[i]);
	}
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	// CGSrtingGEOREF 좌표를  CGPointGP 좌표로 변환한다.
	for ( int i = 0; i < nCount; i++ )
	{
		pStrGEOREF[i].GetPoint(&pGP[i]);
	}
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointUTM 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrMGRS[i].GetPoint(&pUTM[i], this->m_ellipsoid.GetEllipsoid());
	}

	// CGPointUTM 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointUTM 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrUPS[i].GetPoint(&pUPS[i]);
	}

	// CGPointUTM 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGStringGK 좌표를 CGPointGK 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrGK[i].GetPoint(&pGK[i]);
	}

	// CGPointUTM 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toPointGP(CGPointGP *pGP, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	// CGStringNE 좌표를 CGPointGP 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrNE[i].GetPoint(&pGP[i]);
	}
}

//
// CGPointUTM(UTM 좌표 포인트)로 변환
void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGPointGP *pGP, int nCount, int nZone)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGGeoProj utmProj;
//	utmProj.SetMapMode(GMM_UTM);	// GMM_UTM : UTM투영을 한 경우
	utmProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid());		// 타원체를 WGS84로 지정함
	utmProj.SetProjBase(129.0, 0.0);	// 52 zone을 기준점으로 설정
	utmProj.SetProjScaleFactor(UTM_SCALE_FACTOR);	// UTM 투영의 Scale Factor

	// CGPointGP 좌표를  CGPointUTM 좌표로 변환한다.
	utmProj.ForwardUTM(pUTM, pGP, nCount, nZone);

	// 유효숫자 이하에서 반올림한다.
	this->RoundValidity(pUTM, nCount);
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGPointUTM *pSrcUTM, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pSrcUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcUTM)) return;

	for (int i = 0; i < nCount; i++)
		pUTM[i].SetPoint(&pSrcUTM[i]);
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGPointGK *pGK, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pGK, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUPS 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUPS, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGP, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGEOREF, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	// CGStringMGRS 좌표를 CGPointUTM 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrMGRS[i].GetPoint(&pUTM[i], this->m_ellipsoid.GetEllipsoid());
	}
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringUPS 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrUPS, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGK 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGK, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUTM(CGPointUTM *pUTM, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	delete[] pGP;
}

// CGPointGK(GK 좌표 포인트)로 변환
void CGGeoTrans::toPointGK(CGPointGK *pGK, CGPointGP *pGP, int nCount, int nZone)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGGeoProj gkProj;
	gkProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid()); // 타원체를 WGS84로 지정함
	gkProj.SetProjBase(0.0, 0.0);	// 좌표변환과는 관계 없음
	gkProj.SetProjScaleFactor(GK_SCALE_FACTOR);	// GK 투영의 Scale Factor

	// CGPointGP 좌표를  CGPointUPS 좌표로 변환한다.
	gkProj.ForwardGK(pGK, pGP, nCount, nZone);

	// 유효숫자 이하에서 반올림한다.
//	this->RoundValidity(pUTM, nCount);
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUTM, nCount);

	// CGPointGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGPointGK *pSrcGK, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pSrcGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcGK)) return;

	for (int i = 0; i < nCount; i++)
		pGK[i].SetPoint(&pSrcGK[i]);
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pGK[i].Empty();
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGP, nCount);

	// CGPointGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGEOREF, nCount);

	// CGPointGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrMGRS, nCount);

	// CGPointGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	// CGStringGK 좌표를 CGPointGK 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrGK[i].GetPoint(&pGK[i]);
	}
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pGK[i].Empty();
}

void CGGeoTrans::toPointGK(CGPointGK *pGK, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	delete[] pGP;
}


//
// CGPointUPS(UPS 좌표 포인트)로 변환
void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGPointGP *pGP, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGGeoProj utmProj;
	utmProj.SetEllipsoid(this->m_ellipsoid.GetEllipsoid());		// 타원체를 WGS84로 지정함
	utmProj.SetProjBase(0.0, 90.0);	// 북극을 기준점으로 설정 : 영향을 안준다.
	utmProj.SetProjScaleFactor(UPS_SCALE_FACTOR);	// UPS 투영의 Scale Factor

	// CGPointGP 좌표를  CGPointUPS 좌표로 변환한다.
	utmProj.ForwardUPS(pUPS, pGP, nCount);

	// 유효숫자 이하에서 반올림한다.
//	this->RoundValidity(pUTM, nCount);
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUTM, nCount);

	// CGPointGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGPointGK *pGK, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pUPS[i].Empty();
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGPointUPS *pSrcUPS, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pSrcUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcUPS)) return;

	for (int i = 0; i < nCount; i++)
		pUPS[i].SetPoint(&pSrcUPS[i]);
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGP, nCount);

	// CGPointGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGEOREF, nCount);

	// CGPointGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrMGRS, nCount);

	// CGPointGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	// CGStringUPS 좌표를 CGPointUPS 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrUPS[i].GetPoint(&pUPS[i]);
	}
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pUPS[i].Empty();
}

void CGGeoTrans::toPointUPS(CGPointUPS *pUPS, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	// 변환범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pUPS[i].Empty();
}


//
// 지리좌표 문자열로 변환
void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	// CGPointGP 좌표를 CGStringGP 좌표로 변환
	for ( int i = 0; i < nCount; i++)
	{
		pStrGP[i].SetPoint(&pGP[i]);
	}
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를  CGPointGP 좌표로 변환한다.
	this->toPointGP(pGP, pUTM, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환한다.
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를  CGPointGP 좌표로 변환한다.
	this->toPointGP(pGP, pGK, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환한다.
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUPS 좌표를  CGPointGP 좌표로 변환한다.
	this->toPointGP(pGP, pUPS, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환한다.
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringGP *pSrcStrGP, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pSrcStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrGP)) return;

	for (int i =0; i < nCount; i++)
		pStrGP[i] = pSrcStrGP[i];
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// GEOREF 좌표를 CGPointGP 로 변환
	for ( int i = 0; i < nCount; i++)
	{
		pStrGEOREF[i].GetPoint(&pGP[i]);
	}
	
	// CGPointGP 좌표롤 CGStringGP 좌표로 변환
	this->toStringGP(pStrGP, pGP, nCount);
	
	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrMGRS, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringUPS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrUPS, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGK 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGK, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGP(CGStringGP *pStrGP, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGStringGP 좌표로 변환
	this->toStringGP(pStrGP, pGP, nCount);

	delete[] pGP;
}


//
// GEOREF 좌표 문자열로 변환
void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	// CGStringGEOREF 좌표를 CGPointGP 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrGEOREF[i].SetPoint(&pGP[i]);
	}
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUTM, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pGK, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUPS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUPS, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGP, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGEOREF *pSrcStrGEOREF, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pSrcStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrGEOREF)) return;

	for (int i = 0; i < nCount; i++)
		pStrGEOREF[i] = pSrcStrGEOREF[i];
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrMGRS, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringUPS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrUPS, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGK 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGK, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringGEOREF(CGStringGEOREF *pStrGEOREF, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGStringGEOREF 좌표로 변환
	this->toStringGEOREF(pStrGEOREF, pGP, nCount);

	delete[] pGP;
}


//
// MGRS 좌표 문자열로 변환
void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGP, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	for ( int i = 0; i < nCount; i++ )
	{
		pStrMGRS[i].SetPoint(&pUTM[i], this->m_ellipsoid.GetEllipsoid());
	}
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pGK, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointUPS 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pUPS, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pStrGP, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointGP 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pStrGEOREF, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringMGRS *pSrcStrMGRS, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pSrcStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrMGRS)) return;

	for (int i = 0; i < nCount; i++)
		pStrMGRS[i] = pSrcStrMGRS[i];
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointUPS 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pStrUPS, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointUTM *pUTM = new CGPointUTM[nCount];
	if (!(pUTM))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를 CGPointUTM 좌표로 변환
	this->toPointUTM(pUTM, pStrGK, nCount);

	// CGPointUTM 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pUTM, nCount);

	delete[] pUTM;
}

void CGGeoTrans::toStringMGRS(CGStringMGRS *pStrMGRS, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGStringMGRS 좌표로 변환
	this->toStringMGRS(pStrMGRS, pGP, nCount);

	delete[] pGP;
}

//
// UPS 좌표 문자열로 변환
void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGPointGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pGP, nCount);

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	this->toStringUPS(pStrUPS, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pUTM, nCount);

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	this->toStringUPS(pStrUPS, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pStrUPS[i].Empty();
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	for (int i = 0; i < nCount; i++)
	{
		pStrUPS[i].SetPoint(&pUPS[i]);
	}
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pStrGP, nCount);

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	this->toStringUPS(pStrUPS, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pStrGEOREF, nCount);

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	this->toStringUPS(pStrUPS, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointUPS *pUPS = new CGPointUPS[nCount];
	if (!(pUPS))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointUPS 좌표로 변환
	this->toPointUPS(pUPS, pStrMGRS, nCount);

	// CGPointUPS 좌표를 CGStringUPS 좌표로 변환
	this->toStringUPS(pStrUPS, pUPS, nCount);

	delete[] pUPS;
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringUPS *pSrcStrUPS, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pSrcStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrUPS)) return;

	for (int i = 0; i < nCount; i++)
		pStrUPS[i] = pSrcStrUPS[i];
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pStrUPS[i].Empty();
}

void CGGeoTrans::toStringUPS(CGStringUPS *pStrUPS, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	// 같은 범위는 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pStrUPS[i].Empty();
}


//
// GK 좌표 문자열로 변환
void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGPoitnGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pGP, nCount);

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGPoitnUTM 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pUTM, nCount);

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	for (int i = 0; i < nCount; i++)
	{
		pStrGK[i].SetPoint(&pGK[i]);
	}
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pStrGK[i].Empty();
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pStrGP, nCount);

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pStrGEOREF, nCount);

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGK *pGK = new CGPointGK[nCount];
	if (!(pGK))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를 CGPointGK 좌표로 변환
	this->toPointGK(pGK, pStrMGRS, nCount);

	// CGPointGK 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGK, nCount);

	delete[] pGK;
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	// 같은 범위가 존재하지 않습니다.
	for (int i = 0; i < nCount; i++)
		pStrGK[i].Empty();
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringGK *pSrcStrGK, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pSrcStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrGK)) return;

	for (int i = 0; i < nCount; i++)
		pStrGK[i] = pSrcStrGK[i];
}

void CGGeoTrans::toStringGK(CGStringGK *pStrGK, CGStringNE *pStrNE, int nCount)
{
	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringNE 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrNE, nCount);

	// CGPointGP 좌표를 CGStringGK 좌표로 변환
	this->toStringGK(pStrGK, pGP, nCount);

	delete[] pGP;
}


//
// NE 좌표 문자열로 변환
void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGPointGP *pGP, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGP)) return;

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	for (int i = 0; i < nCount; i++)
	{
		pStrNE[i].SetPoint(&pGP[i]);
	}
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGPointUTM *pUTM, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pUTM);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUTM)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointUTM 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pUTM, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGPointGK *pGK, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGPointGK 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pGK, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGPointUPS *pUPS, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pUPS)) return;

	// 같은 범위가 존재하지 않는다.
	for (int i = 0; i < nCount; i++)
		pStrNE[i].Empty();
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringGP *pStrGP, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pStrGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGP)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGP 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGP, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringGEOREF *pStrGEOREF, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pStrGEOREF);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGEOREF)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGEOREF 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGEOREF, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringMGRS *pStrMGRS, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pStrMGRS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrMGRS)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringMGRS 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrMGRS, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringUPS *pStrUPS, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pStrUPS);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrUPS)) return;

	// 같은 범위가 존재하지 않는다.
	for (int i = 0; i < nCount; i++)
		pStrNE[i].Empty();
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringGK *pStrGK, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pStrGK);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrGK)) return;

	CGPointGP *pGP = new CGPointGP[nCount];
	if (!(pGP))	// 메모리가 할당되지 않음
		return;

	// CGStringGK 좌표를  CGPointGP 좌표로 변환
	this->toPointGP(pGP, pStrGK, nCount);

	// CGPointGP 좌표를 CGStringNE 좌표로 변환
	this->toStringNE(pStrNE, pGP, nCount);

	delete[] pGP;
}

void CGGeoTrans::toStringNE(CGStringNE *pStrNE, CGStringNE *pSrcStrNE, int nCount)
{
	ASSERT(pStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pStrNE)) return;

	ASSERT(pSrcStrNE);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrNE)) return;

	for (int i = 0; i < nCount; i++)
		pStrNE[i] = pSrcStrNE[i];
}



void CGGeoTrans::StdMolodensky(CGPointGP *toGP, CGDatum *toDatum,
							   CGPointGP *fromGP, CGDatum *fromDatum,
							   double *toHeight, double *fromHeight)
{
	ASSERT(toGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(toGP)) return;

	ASSERT(toDatum);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(toDatum)) return;

	ASSERT(fromGP);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(fromGP)) return;

	ASSERT(fromDatum);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(fromDatum)) return;

	double pi = 3.1415926535;
	double toRadian = pi / 180.0;
	double toDegree = 180.0 / pi;
		
	// Test : NAD27 CONUS ( Clarke 1866 Ellipsoid )
	double	from_latitude, from_longitude, from_height;
	from_longitude	= fromGP->m_P.lon;	// Test : -100 degree
	from_latitude	= fromGP->m_P.lat;	// Test :   30 degree
	if (fromHeight != NULL) from_height	= *fromHeight;	// Test :  232 meter
	else from_height = 0.0;

	// 좌표를 라디안 단위로 계산
	from_latitude	= from_latitude  * toRadian;
	from_longitude	= from_longitude * toRadian;
	from_height		= from_height;

	// Test : NAD27 CONUS ( Clarke 1866 Ellipsoid )
	double	from_a, from_f, from_es;
	CGEllipsoid from_Ellipsoid(fromDatum->GetEllipsoid());

	from_a	= from_Ellipsoid.GetSemiMajorAxis();
	from_f	= from_Ellipsoid.GetFlattening();
	from_es	= from_Ellipsoid.GetFirstEccentricitySquared();
	
	// Test : WGS 84 Ellipsoid
	double to_a, to_f;
	CGEllipsoid to_Ellipsoid(toDatum->GetEllipsoid());
	to_a	= to_Ellipsoid.GetSemiMajorAxis();
	to_f	= to_Ellipsoid.GetFlattening();

	double delta_X, delta_Y, delta_Z;
	delta_X = fromDatum->GetDX() - toDatum->GetDX();
	delta_Y = fromDatum->GetDY() - toDatum->GetDY();
	delta_Z = fromDatum->GetDZ() - toDatum->GetDZ();

	double	temp1, temp2, temp3, temp4, temp5;
	temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;

	// Calculate Geodetic position shifts
	double	delta_a, delta_f;
	delta_a = to_a - from_a;
	delta_f = to_f - from_f;

	double	s_latitude, s_longitude, c_latitude, c_longitude;
	s_latitude	= ::sin(from_latitude );
	s_longitude	= ::sin(from_longitude);
	c_latitude	= ::cos(from_latitude );
	c_longitude	= ::cos(from_longitude);

	double	Rn, Rm, bda;
	Rn	= from_a / ::sqrt( 1.0 - from_es * ::sin( from_latitude * from_latitude ) );
	temp1	= from_a * ( 1.0 - from_es );
	temp2	= ::pow( ( 1.0 - from_es * ::sin(from_latitude * from_latitude) ), 1.5 ); // 1.5는 3/2
	Rm	= temp1 / temp2;
	bda	= 1.0 - from_f;

	temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;

	double	delta_latitude, delta_longitude, delta_height;
	temp1 = ( (-1.0) * delta_X * s_latitude * c_longitude - delta_Y * s_latitude * s_longitude ) + delta_Z * c_latitude;
	temp2 = ( delta_a * ( Rn * from_es * s_latitude * c_latitude ) ) / from_a;
	temp3 = ( delta_f * ( Rm / bda + Rn * bda ) * s_latitude * c_latitude );
	temp4 = ( Rm + from_height );
	temp5 = ( temp1 + temp2 ) + temp3;
	delta_latitude = temp5 / temp4;

	temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;
	temp1 = (-1.0) * delta_X * s_longitude + delta_Y * c_longitude;
	temp2 = ( Rn + from_height ) * c_latitude;
	delta_longitude = temp1 / temp2;

	temp1 = temp2 = temp3 = temp4 = temp5 = 0.0;
	temp1 = delta_X * c_latitude * c_longitude;
	temp2 = delta_Y * c_latitude * s_longitude;
	temp3 = delta_Z * s_latitude;
	temp4 = (-1.0) * delta_a * ( from_a / Rn );
	temp5 = delta_f * bda * Rn * s_latitude * s_latitude;
	delta_height = temp1 + temp2 + temp3 + temp4 + temp5;

	// Result of Coputation to position
	double	to_latitude, to_longitude, to_height;
	to_latitude		= from_latitude  + delta_latitude;
	to_longitude	= from_longitude + delta_longitude;
	to_height		= from_height    + delta_height;

	to_latitude		= to_latitude  * toDegree;	//   30.000223889 degree
	to_longitude	= to_longitude * toDegree;	// -100.000369588 degree
	to_height		= to_height;				//  194.816

	toGP->m_P.lat = to_latitude;
	toGP->m_P.lon = to_longitude;
	if (toHeight != NULL) *toHeight = to_height;
}


void CGGeoTrans::GeoConvert(CGPointGeo *pDesPtGeo, CGPointGeo *pSrcPtGeo, int nCount)
{	// CGPointGeo 좌표간의 변환
	ASSERT(pDesPtGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDesPtGeo)) return;

	ASSERT(pSrcPtGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcPtGeo)) return;

	ASSERT(GPTYPE_FIRST <= pDesPtGeo->m_nPointType &&
						   pDesPtGeo->m_nPointType <= GPTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GPTYPE_FIRST <= pDesPtGeo->m_nPointType &&
						   pDesPtGeo->m_nPointType <= GPTYPE_LAST)) return;

	ASSERT(GPTYPE_FIRST <= pSrcPtGeo->m_nPointType &&
						   pSrcPtGeo->m_nPointType <= GPTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GPTYPE_FIRST <= pSrcPtGeo->m_nPointType &&
						   pSrcPtGeo->m_nPointType <= GPTYPE_LAST)) return;


	switch(pDesPtGeo->m_nPointType)
	{
	case GPTYPE_GP :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			toPointGP((CGPointGP*)pDesPtGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			toPointGP((CGPointGP*)pDesPtGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			toPointGP((CGPointGP*)pDesPtGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			toPointGP((CGPointGP*)pDesPtGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GPTYPE_GK :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			toPointGK((CGPointGK*)pDesPtGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			toPointGK((CGPointGK*)pDesPtGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			toPointGK((CGPointGK*)pDesPtGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			toPointGK((CGPointGK*)pDesPtGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GPTYPE_UPS :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GPTYPE_UTM :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	}
}

void CGGeoTrans::GeoConvert(CGPointGeo *pDesPtGeo, CGStringGeo *pSrcStrGeo, int nCount)
{	// CGPointGeo <== CGStringGeo
	ASSERT(pDesPtGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDesPtGeo)) return;

	ASSERT(pSrcStrGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrGeo)) return;

	ASSERT(GPTYPE_FIRST <= pDesPtGeo->m_nPointType &&
						   pDesPtGeo->m_nPointType <= GPTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GPTYPE_FIRST <= pDesPtGeo->m_nPointType &&
						   pDesPtGeo->m_nPointType <= GPTYPE_LAST)) return;

	ASSERT(GSTYPE_FIRST <= pSrcStrGeo->m_nPointType &&
						   pSrcStrGeo->m_nPointType <= GSTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GSTYPE_FIRST <= pSrcStrGeo->m_nPointType &&
						   pSrcStrGeo->m_nPointType <= GSTYPE_LAST)) return;

	switch(pDesPtGeo->m_nPointType)
	{
	case GPTYPE_GP :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			toPointGP((CGPointGP*)pDesPtGeo, (CGStringUPS*)pSrcStrGeo, nCount);
			break;
		}
		break;
	case GPTYPE_GK :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			toPointGK((CGPointGK*)pDesPtGeo, (CGStringUPS*)pSrcStrGeo, nCount);
			break;
		}
		break;
	case GPTYPE_UPS :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			toPointUPS((CGPointUPS*)pDesPtGeo, (CGStringUPS*)pSrcStrGeo, nCount);
			break;
		}
		break;
	case GPTYPE_UTM :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			toPointUTM((CGPointUTM*)pDesPtGeo, (CGStringUPS*)pSrcStrGeo, nCount);
			break;
		}
		break;
	}
}

void CGGeoTrans::GeoConvert(CGStringGeo *pDesStrGeo, CGPointGeo *pSrcPtGeo, int nCount)
{	// CGStringGeo <== CGPointGeo
	ASSERT(pDesStrGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDesStrGeo)) return;

	ASSERT(pSrcPtGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcPtGeo)) return;

	ASSERT(GSTYPE_FIRST <= pDesStrGeo->m_nPointType &&
						   pDesStrGeo->m_nPointType <= GSTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GSTYPE_FIRST <= pDesStrGeo->m_nPointType &&
						   pDesStrGeo->m_nPointType <= GSTYPE_LAST)) return;

	ASSERT(GPTYPE_FIRST <= pSrcPtGeo->m_nPointType &&
						   pSrcPtGeo->m_nPointType <= GPTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GPTYPE_FIRST <= pSrcPtGeo->m_nPointType &&
						   pSrcPtGeo->m_nPointType <= GPTYPE_LAST)) return;

	switch(pDesStrGeo->m_nPointType)
	{
	case GSTYPE_GP :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GSTYPE_GEOREF :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GSTYPE_GK :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GSTYPE_MGRS :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GSTYPE_NE :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	case GSTYPE_UPS :
		switch(pSrcPtGeo->m_nPointType)
		{
		case GPTYPE_GP :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGPointGP*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_GK :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGPointGK*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UPS :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGPointUPS*)pSrcPtGeo, nCount);
			break;
		case GPTYPE_UTM :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGPointUTM*)pSrcPtGeo, nCount);
			break;
		}
		break;
	}
}

void CGGeoTrans::GeoConvert(CGStringGeo *pDesStrGeo, CGStringGeo *pSrcStrGeo, int nCount)
{	// CGStringGeo 좌표간의 변환
	ASSERT(pDesStrGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pDesStrGeo)) return;

	ASSERT(pSrcStrGeo);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(pSrcStrGeo)) return;

	ASSERT(GSTYPE_FIRST <= pDesStrGeo->m_nPointType &&
						   pDesStrGeo->m_nPointType <= GSTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GSTYPE_FIRST <= pDesStrGeo->m_nPointType &&
						   pDesStrGeo->m_nPointType <= GSTYPE_LAST)) return;

	ASSERT(GSTYPE_FIRST <= pSrcStrGeo->m_nPointType &&
						   pSrcStrGeo->m_nPointType <= GSTYPE_LAST);
	// Release에서 Down되지 않도록 if문으로 처리
	if (!(GSTYPE_FIRST <= pSrcStrGeo->m_nPointType &&
						   pSrcStrGeo->m_nPointType <= GSTYPE_LAST)) return;

	switch(pDesStrGeo->m_nPointType)
	{
	case GSTYPE_GP :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringGP((CGStringGP*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	case GSTYPE_GEOREF :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringGEOREF((CGStringGEOREF*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	case GSTYPE_GK :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringGK((CGStringGK*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	case GSTYPE_MGRS :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringMGRS((CGStringMGRS*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	case GSTYPE_NE :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringNE((CGStringNE*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	case GSTYPE_UPS :
		switch(pSrcStrGeo->m_nPointType)
		{
		case GSTYPE_GP :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringGP*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GEOREF :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringGEOREF*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_GK :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringGK*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_MGRS :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringMGRS*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_NE :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringNE*)pSrcStrGeo, nCount);
			break;
		case GSTYPE_UPS :
			this->toStringUPS((CGStringUPS*)pDesStrGeo, (CGStringUPS*)pSrcStrGeo, nCount);
		break;
		}
		break;
	}
}

	


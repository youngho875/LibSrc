#include "pch.h"
#include "CoordMgr.h"
#include "Coord.h"
#include "../../generalinclude/Useful.h"

#include <math.h>

#define M_PI        3.14159265358979323846

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//CCoordMgr* CCoordMgr::inst_ = NULL;

CCoordMgr* CCoordMgr::GetInst(void)
{
	static CCoordMgr instance;
	return &instance;

}

void CCoordMgr::ReleaseInst(void)
{
	//SAFE_DELETE(inst_);
}

CCoordMgr::CCoordMgr()
{
	m_nScreenWidth=GetSystemMetrics(SM_CXSCREEN); 
	m_nScreenHeight=GetSystemMetrics(SM_CYSCREEN); 
	m_fCenterX=(double)m_nScreenWidth/2.0;
	m_fCenterY=(double)m_nScreenHeight/2.0;
	m_fScale=(double)1.0; 
}

//void CCoordMgr::SetInst(CCoordMgr* instance)
//{
//	inst_->m_fCenterX = instance->m_fCenterX;
//	inst_->m_fCenterY = instance->m_fCenterY;
//	inst_->m_fHorVerRatio = instance->m_fHorVerRatio;
//	inst_->m_fScale = instance->m_fScale;
//	inst_->m_nDisplayViewHeight = instance->m_nDisplayViewHeight;
//	inst_->m_nDisplayViewWidth = instance->m_nDisplayViewWidth;
//	inst_->m_nScreenHeight = instance->m_nScreenHeight;
//	inst_->m_nScreenWidth = instance->m_nScreenWidth;
//}

void CCoordMgr::SetInst(CCoordMgr* instance)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (instance == nullptr) return;

	// 주요 좌표 및 스케일 정보 동기화
	this->m_fCenterX = instance->m_fCenterX;
	this->m_fCenterY = instance->m_fCenterY;
	this->m_fScale = instance->m_fScale;

	// 화면 해상도 정보 동기화
	this->m_nScreenWidth = instance->m_nScreenWidth;
	this->m_nScreenHeight = instance->m_nScreenHeight;
	this->m_nDisplayViewWidth = instance->m_nDisplayViewWidth;
	this->m_nDisplayViewHeight = instance->m_nDisplayViewHeight;

	// 기타 상태 플래그
	this->m_bFlat = instance->m_bFlat;
}

void CCoordMgr::InitDisplayParameter(CRect rect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_nDisplayViewWidth=rect.Width(); 
	m_nDisplayViewHeight=rect.Height(); 

	m_fCenterX=(double)m_nDisplayViewWidth/2.0; 
	m_fCenterY=(double)m_nDisplayViewHeight/2.0;

	m_fHorVerRatio=(double)(m_nDisplayViewWidth)/(double)(m_nDisplayViewHeight);
}

void CCoordMgr::InitDisplay(int nWidth, int nHeight)
{
	//
	// 함수 시작부분에 넣는 것이 좋음 assertion error 방지를 위해
	//
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 0으로 나누기 방지
	if (nWidth <= 0)  nWidth = 1;
	if (nHeight <= 0) nHeight = 1;

	m_nScreenWidth = nWidth;
	m_nScreenHeight = nHeight;

	// 화면의 물리적 중심점 계산 (픽셀 단위)
	m_fCenterX = (double)nWidth / 2.0;
	m_fCenterY = (double)nHeight / 2.0;

	// 가로세로 비율 계산 (지도 왜곡 방지용)
	m_fHorVerRatio = (double)nWidth / (double)nHeight;

	// 뷰포트 크기 업데이트
	m_nDisplayViewWidth = nWidth;
	m_nDisplayViewHeight = nHeight;

	// 비고: 이후 WorldXY2ScreenXY 호출 시
	// 여기서 계산된 m_fCenterX/Y를 기준으로 좌표 오프셋이 결정됩니다.
	//m_fHorVerRatio = (double)(m_nDisplayViewWidth) / (double)(m_nDisplayViewHeight);
}


void CCoordMgr::SetDisplayCenter(double fCenterX, double fCenterY)
{
	m_fCenterX=fCenterX; 
	m_fCenterY=fCenterY; 
}	

void CCoordMgr::GetDisplayCenter(double *fCenterX, double *fCenterY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*fCenterX=m_fCenterX; 
	*fCenterY=m_fCenterY; 
}

/*
void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
									long* nXScreenCoord, long* nYScreenCoord,
									double fCenterX, double fCenterY,
									double fWorldCenterX, double fWorldCenterY, // 추가됨
									double fScale)
{
	// 타겟 좌표와 중심 경위도의 차이(Delta) 계산
	double dLon = fXWorldCoord - fWorldCenterX;
	double dLat = fYWorldCoord - fWorldCenterY;

	// 경도 날짜변경선(-180~180) 보정
	// 이 로직이 있어야 179도 옆에 -179도가 나란히 그려집니다.
	if (dLon > 180.0)      dLon -= 360.0;
	else if (dLon < -180.0) dLon += 360.0;

	// 스크린 좌표 변환
	// 중심 스크린 좌표(fCenterX_Screen)에서 픽셀 오차만큼 이동
	*nXScreenCoord = (long)(fCenterX + (dLon * fScale));
	*nYScreenCoord = (long)(fCenterY - (dLat * fScale)); // 위도는 부호 반전
}

void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
									long* nXScreenCoord, long* nYScreenCoord)
{
	// 상대 거리 계산
	double dLon = fXWorldCoord - m_fWorldCenterX; // 현재 화면 중심 경도
	double dLat = fYWorldCoord - m_fWorldCenterY; // 현재 화면 중심 위도

	// 경도 Wrap-around 보정
	if (dLon > 180.0)       dLon -= 360.0;
	else if (dLon < -180.0) dLon += 360.0;

	// 결과 대입 (m_fCenterX/Y는 화면의 절반 크기: Width/2, Height/2)
	*nXScreenCoord = (long)(m_fCenterX + (dLon * m_fScale));
	*nYScreenCoord = (long)(m_fCenterY - (dLat * m_fScale));
}

// double 버전도 동일한 로직 적용
void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord,
									double* fXScreenCoord, double* fYScreenCoord)
{
	double dLon = fXWorldCoord - m_fWorldCenterX;
	double dLat = fYWorldCoord - m_fWorldCenterY;

	if (dLon > 180.0)       dLon -= 360.0;
	else if (dLon < -180.0) dLon += 360.0;

	*fXScreenCoord = m_fCenterX + (dLon * m_fScale);
	*fYScreenCoord = m_fCenterY - (dLat * m_fScale);
}

void CCoordMgr::ScreenXY2WorldXY(long nXScreenCoord, long nYScreenCoord,
									double* fXWorldCoord, double* fYWorldCoord,
									double fCenterX, double fCenterY,
									double fWorldCenterX, double fWorldCenterY,
									double fScale)
{
	// 화면 중심 픽셀로부터의 거리(Pixel Delta) 계산
	double dx = (double)nXScreenCoord - fCenterX;
	double dy = (double)nYScreenCoord - fCenterY;

	// 픽셀 거리를 경위도 단위(Degree Delta)로 변환
	// m_fScale이 0일 경우를 대비한 방어 코드 추가
	if (fScale != 0.0) {
		double dLon = dx / fScale;
		double dLat = -dy / fScale; // 화면 좌표(Y+)와 위도(N+) 방향은 반대이므로 부호 반전

		// 중심 경위도에 변화량을 더함
		double resLon = fWorldCenterX + dLon;
		double resLat = fWorldCenterY + dLat;

		//경도 정규화 (-180 ~ 180 범위를 유지하도록)
		while (resLon > 180.0)  resLon -= 360.0;
		while (resLon < -180.0) resLon += 360.0;

		// 위도 제한 (90도 ~ -90도)
		if (resLat > 90.0)  resLat = 90.0;
		if (resLat < -90.0) resLat = -90.0;

		*fXWorldCoord = resLon;
		*fYWorldCoord = resLat;
	}
}

void CCoordMgr::ScreenXY2WorldXY(long nXScreenCoord, long nYScreenCoord,
									double* fXWorldCoord, double* fYWorldCoord)
{
	// m_fCenterX/Y: 화면 중심 픽셀 (Width/2, Height/2)
	// m_fCenterX_W/Y_W: 현재 지도의 중심 경위도

	double dx = (double)nXScreenCoord - m_fCenterX;
	double dy = (double)nYScreenCoord - m_fCenterY;

	if (m_fScale != 0.0) {
		double dLon = dx / m_fScale;
		double dLat = -dy / m_fScale;

		double resLon = m_fWorldCenterX + dLon;
		double resLat = m_fWorldCenterY + dLat;

		// 경도 Wrap-around (-180~180)
		while (resLon > 180.0)  resLon -= 360.0;
		while (resLon < -180.0) resLon += 360.0;

		// 위도 Clamp
		if (resLat > 90.0)  resLat = 90.0;
		if (resLat < -90.0) resLat = -90.0;

		*fXWorldCoord = resLon;
		*fYWorldCoord = resLat;
	}
}
*/


void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord, 
										long	*nXScreenCoord, long *nYScreenCoord, 
										double fCenterX, double fCenterY, 
										double fScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*nXScreenCoord=(long)(  fXWorldCoord*fScale  + fCenterX); 
	*nYScreenCoord=(long)(-(fYWorldCoord*fScale) + fCenterY); 
}

void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord, 
										double *fXScreenCoord, double *fYScreenCoord, 
										double fCenterX, double fCenterY, 
										double fScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*fXScreenCoord=fXWorldCoord*fScale  + fCenterX; 
	*fYScreenCoord=-(fYWorldCoord*fScale) + fCenterY; 
}

void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord, 
										long	*nXScreenCoord, long *nYScreenCoord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*nXScreenCoord=(long)(  fXWorldCoord*m_fScale  + m_fCenterX); 
	*nYScreenCoord=(long)(-(fYWorldCoord*m_fScale) + m_fCenterY); 
}

void CCoordMgr::WorldXY2ScreenXY(double fXWorldCoord, double fYWorldCoord, 
										double *fXScreenCoord, double *fYScreenCoord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*fXScreenCoord=fXWorldCoord*m_fScale  + m_fCenterX; 
	*fYScreenCoord=-(fYWorldCoord*m_fScale) + m_fCenterY; 
}

void CCoordMgr::ScreenXY2WorldXY(int nXScreenCoord, int nYScreenCoord, 
								 double *fXWorldCoord, double *fYWorldCoord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*fXWorldCoord=((double)nXScreenCoord-(double)m_fCenterX)/(double)m_fScale; 
	*fYWorldCoord=-((double)nYScreenCoord-(double)m_fCenterY)/(double)m_fScale; 
}


void CCoordMgr::ScreenXY2WorldXY(int nXScreenCoord, int nYScreenCoord, 
										double *fXWorldCoord, double *fYWorldCoord, 								
										double fCenterX, double fCenterY, 
										double fScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*fXWorldCoord=((double)nXScreenCoord-(double)fCenterX)/(double)fScale; 
	*fYWorldCoord=-((double)nYScreenCoord-(double)fCenterY)/(double)fScale; 
}


void CCoordMgr::SetCenterCoord(double lon, double lat)
{
	m_fWorldCenterX = lon;
	m_fWorldCenterY = lat;
}


double CCoordMgr::DeltaXYToBearing(double fDeltaX, double fDeltaY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//한국은 자북을 사용함. 그래서 7.0을 각각 더해줌.081016
	double fRadian;
	double degree = 0.0;

	if (fDeltaY != 0.0) {
		fRadian = atan2((double)fDeltaX,(double)fDeltaY);
		if (fRadian > 0.0)
//			return((fRadian * 180.0) / M_PI+7.0);
			degree=(fRadian * 180.0) / M_PI+7.0;
//		else if(fRadian == 0.0) return 360.0;
//			else if(fRadian == 0.0) degree=7.0; 270도 일때 0도가 되는 초기화가 막기 위해 주석처리
		else 
//			return(((fRadian * 180.0) / M_PI) + 360.0+7.0);
			degree=((fRadian * 180.0) / M_PI) + 360.0+7.0;

	}
//	else 
//		return(360.0);
	if(degree>360.0)
		degree=degree-360.0;
	return degree;
}

//double CCoordMgr::DeltaXYToRange(double fDeltaX, double fDeltaY)
//{
//	return (sqrt(fDeltaX*fDeltaX+fDeltaY*fDeltaY)); 
//}

double CCoordMgr::DeltaVxVyToHeading(double fVelFactorX, double fVelFactorY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//한국은 자북을 사용하므로 각기 7.0값을 더해줌. 081016
	double fRadian=0.0;
	double fDegree=0.0;

	if (fVelFactorY != 0.0) {
		fRadian = atan2((double)fVelFactorX,(double)fVelFactorY);
		if (fRadian > 0.0) 
			fDegree = (fRadian * 180.0) / M_PI+7.0;
		else 
			fDegree = ((fRadian * 180.0) / M_PI) + 360.0+7.0;
	}
	//06.07.18 방위 수정 -유혁재 
	else if (fVelFactorY == 0  && fVelFactorX > 0) fDegree = 90.0+7.0 ;
	else if (fVelFactorY == 0  && fVelFactorX < 0) fDegree = 270.0+7.0 ;

	else if (fDegree>360) fDegree=fDegree-360;

	else fDegree = 0.0;
		return(fDegree);

}

double CCoordMgr::DeltaVxVyToSpeed(double fVelFactorX, double fVelFactorY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double fDeltaX, fDeltaY;

	fDeltaX = fVelFactorX * 60.;
	fDeltaY = fVelFactorY * 60.;
	
	return(sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY) * 60.0);
}

void CCoordMgr::SetNewCenter(double fNewCenterX, double fNewCenterY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double fDiffX, fDiffY; 

	fDiffX=m_nDisplayViewWidth/2.0-fNewCenterX; 
	fDiffY=m_nDisplayViewHeight/2.0-fNewCenterY; 

	m_fCenterX=m_fCenterX+fDiffX; 
	m_fCenterY=m_fCenterY+fDiffY; 
}

void CCoordMgr::CalcNewCenterAndScale(CRect rect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double	fDiffX, fDiffY; 
	double	fScaleX, fScaleY; 
	double	fNewScale; 


	if(rect.Width() < 30 || rect.Height() < 30)
		return; 

	m_fCenterX += (double)m_nDisplayViewWidth/2.0 - (double)rect.CenterPoint().x;
	m_fCenterY += (double)m_nDisplayViewHeight/2.0 - (double)rect.CenterPoint().y;

	fDiffX = m_fCenterX - (double)m_nDisplayViewWidth/2.0; 
	fDiffY = m_fCenterY - (double)m_nDisplayViewHeight/2.0; 

	fScaleX = (double)((double)m_nDisplayViewWidth/(double)rect.Width()); 
	fScaleY = (double)((double)m_nDisplayViewHeight/(double)rect.Height()); 
	fNewScale = m_fScale *((fScaleX > fScaleY) ? fScaleY : fScaleX); 

	if(fNewScale > 100.0)
		fNewScale = 100.0; 
	if(fNewScale < 1.0)
		fNewScale=1.0; 

	fDiffX = fDiffX / m_fScale * fNewScale - fDiffX; 
	fDiffY = fDiffY / m_fScale * fNewScale - fDiffY; 

	m_fCenterX += fDiffX; 
	m_fCenterY += fDiffY; 

	m_fScale = fNewScale; 
}

void CCoordMgr::HeadingSpeedToVxVy(int nSpeed, int nHeading, double *fVx, double *fVy)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double fSpeed; 
	double		fRadian; 
	if (nHeading==90 || nHeading == 270)
		*fVy=0.0;
	if (nHeading==180 || nHeading == 360)
		*fVx=0.0; 

	fRadian=double(nHeading)*M_PI/180.0;
	fSpeed=double(nSpeed)/60.0; 
	*fVy=fSpeed*cos(fRadian)/60.0;
	*fVx=fSpeed*sin(fRadian)/60.0; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Name :	void CCoordinateManager::UTM_TO_WGP(double x, double y, int izone, double &lon, double &lat)
// Desc :	Converts UTM to WGS Geodetic Position.
//##ModelId=3DCCEEEB037B
// mbc 7.19
void CCoordMgr::UTM_TO_WGP(double x, double y, int izone, double &lon, double &lat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double a = double(6378137.);
	double f = 1/298.257223563;
	double fe = double(500000.);
	double ok = double(0.9996);
	double degrad = atan(1.0)/45.0;

	double recf = 1/f;
	double b = a*(recf-1)/recf;
	double es = (pow(a,2)-pow(b,2))/(pow(a,2));
	double ebs = (pow(a,2)-pow(b,2))/(pow(b,2));
	double tn = (a-b)/(a+b);
	double ap = a*(double(1)-tn+5.*(pow(tn,2)-pow(tn,3))/4. + 81.*(pow(tn,4)-pow(tn,5))/64.);
	double bp = 3.*a*(tn-pow(tn,2)+7*(pow(tn,3)-pow(tn,4))/8.+55.*pow(tn,5)/64.)/2.;

	double cp = 15.*a*(pow(tn,2)-pow(tn,3)+3.*(pow(tn,4)-pow(tn,5))/4.)/16.;
	double dp = 35.*a*(pow(tn,3)-pow(tn,4)+11.*pow(tn,5)/16.)/48.;
	double ep = 315.*a*(pow(tn,4)-pow(tn,5))/512.;

	double nfn = (y < 0 )*10000000;
	y = fabs(y);

	double tmd = (y-nfn)/ok;

	double sphi = 0;
	double sr = a*(1.-es)/pow((sqrt(1.-es*pow(sin(sphi),2))),3);

	double ftphi = tmd/sr;

	double t10;

	for ( int i = 0 ; i < 5 ; i++ ) {

		t10 = ap*ftphi-bp*sin(2.0*ftphi)+cp*sin(4.0*ftphi)-dp*sin(6.0*ftphi)+ep*sin(8.0*ftphi);
		sr = a*(1.-es)/pow((sqrt(1.-es*pow(sin(ftphi),2))),3);
		ftphi = ftphi + (tmd-t10)/sr;
	}

	sr = a*(1.-es)/pow((sqrt(1.-es*pow(sin(ftphi),2))),3);

	double sn = a / sqrt(1.0-es*sin(ftphi)*sin(ftphi));

	double s = sin(ftphi);
	double c = cos(ftphi);
	double t = s/c;
	double eta = ebs*c*c;

	double de = x-fe;

	t10 = t/(2.0*sr*sn*ok*ok);

	double t11 = t*(5.0+3.0*pow(t,2)+eta-4.0*pow(eta,2)-9.0*pow(t,2)*eta)/(24.0*sr*pow(sn,3)*pow(ok,4));

	double t12 = t*(61.0+90.0*pow(t,2)+46.0*eta+45.0*pow(t,4)-252.0*pow(t,2)*eta-3.0*pow(eta,2)+100.0*pow(eta,3 )
			    -66.0*pow(t,2)*pow(eta,2)-90.0*pow(t,4)*eta+88.0*pow(eta,4)+225.0*pow(t,4)*pow(eta,2 )
				 +84.0*pow(t,2)*pow(eta,3)-192.0*pow(t,2)*pow(eta,4)) 
				/(720.0*sr*pow(sn,5)*pow(ok,6));

	double t13 = t*(1385.0+3633.0*pow(t,2)+4095.*pow(t,4)+1575*pow(t,6)) / (40320.0*sr*pow(sn,7)*pow(ok,8));

	sphi = ftphi - pow(de,2)*t10 + pow(de,4)*t11 - pow(de,6)*t12 + pow(de,8)*t13;

	double t14 = 1.0/(sn*c*ok);

	double t15 = (1.0+2.0*pow(t,2)+eta)/(6.0*pow(sn,3)*c*pow(ok,3));

	double t16 = (5.0+6.0*eta+28.0*pow(t,2)-3.0*pow(eta,2)+8.0*pow(t,2)*eta+24.0*pow(t,4)-4.0*pow(eta,3)+4.0*pow(t,2)*pow(eta,2 )
		   +24.0*pow(t,2)*pow(eta,3))/(120.0*pow(sn,5)*c*pow(ok,5));

	double t17 = (61.0+662.*pow(t,2)+1320.0*pow(t,4)+720.0*pow(t,6))/(5040.0*pow(sn,7)*c*pow(ok,7));

	double dlam = de*t14-pow(de,3)*t15+pow(de,5)*t16-pow(de,7)*t17;
	double olam = (izone*6-183)*degrad;
	double slam = olam+dlam;

	sphi = double(sphi)*180/M_PI;
	slam = double(slam)*180/M_PI;
	lon = slam;
	lat = sphi;

}

void CCoordMgr::PowerSet(CCoordMgr* inst)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (inst == nullptr) return;

	// 1. 화면 및 뷰포트 물리 정보 복사
	this->m_nScreenWidth = inst->m_nScreenWidth;
	this->m_nScreenHeight = inst->m_nScreenHeight;
	this->m_nDisplayViewWidth = inst->m_nDisplayViewWidth;
	this->m_nDisplayViewHeight = inst->m_nDisplayViewHeight;
	this->m_fHorVerRatio = inst->m_fHorVerRatio;

	// 2. 좌표 변환 핵심 파라미터 복사
	this->m_fCenterX = inst->m_fCenterX;
	this->m_fCenterY = inst->m_fCenterY;
	this->m_fScale = inst->m_fScale;

	// 3. 상태 플래그 복사
	this->m_bFlat = inst->m_bFlat;
}


void CCoordMgr::WorldLonLat2ScreenXY(double fLonWorldCoord, double fLatWorldCoord,
	double* fXScreenCoord, double* fYScreenCoord,
	double fCenterX, double fCenterY,
	double fScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_bFlat)
	{
		*fXScreenCoord = ((fLonWorldCoord - 127.031) * 47.5 * fScale + fCenterX);
		*fYScreenCoord = (-(fLatWorldCoord - 37.0861) * 60 * fScale + fCenterY);

		return;
	}

	CCoord coord;
	double rexcoord, reycoord;
	coord.degrees_lat_long_to_x_y(fLatWorldCoord, fLonWorldCoord, &rexcoord, &reycoord);

	*fXScreenCoord = rexcoord * fScale + fCenterX;
	*fYScreenCoord = -(reycoord * fScale) + fCenterY;
}

void CCoordMgr::WorldLonLat2ScreenXY(double fLonWorldCoord, double fLatWorldCoord,
	long* fXScreenCoord, long* fYScreenCoord,
	double fCenterX, double fCenterY,
	double fScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_bFlat)
	{
		*fXScreenCoord = ((fLonWorldCoord - 127.031) * 47.5 * fScale + fCenterX);
		*fYScreenCoord = (-(fLatWorldCoord - 37.0861) * 60 * fScale + fCenterY);

		return;
	}

	CCoord coord;
	double rexcoord, reycoord;
	coord.degrees_lat_long_to_x_y(fLatWorldCoord, fLonWorldCoord, &rexcoord, &reycoord);

	*fXScreenCoord = rexcoord * fScale + fCenterX;
	*fYScreenCoord = -(reycoord * fScale) + fCenterY;
}


#pragma once



// ELLIPSOID : 타원체를 정의하는 구조체
//	참고 : 타원체의 정의
//	     1) 장축(a), 단축(b)
//	     2) 장축(a), 편평도(f)
//	     3) 장축(a),편심(e제곱)
//	공식(equation) : f = 1 - b/a, e**2 = 2f - f**2)
typedef struct tagELLIPSOID {
	double		semiMajor;	// 타원체의 장축( a, f = 1 - b/a )
	double		rcpFlat;		// 편평도의 역수( 1/f )
					// Inverse Flattening 또는 Reciprocal Flattening
	double		eSquare;	// 이심률의 제곱
					// ( e**2 = 2*f - f**2 : First Eccentricity Square )
} ELLIPSOID;



//
// 타원체 정의
#define	EFIRST		1	// first ellipsoid index
//#define	EAAY	1	//Airy 1830
#define	EBES		2	//Bessel1841
#define	ECLK		3	//Clarke1866
#define	ECLJ		4	//clarke1880
#define	EEVE		5	//Everest1830
//#define	EFIS	6	//Fischer1960(Mercury)
//#define	EFIT	7	//Fischer1968
#define	EGRS		8	//GRS1967
//#define	EGRT	9	//GRS1975
#define	EGRU		10	//GRS1980
//#define	EHGH	11	//Hough1956
#define	EINT	12	//International
//#define	EKRA	13	//Krassovsky1940
//#define	ESUA	14	//South American1969
//#define	EWGA	15	//WGS60
//#define	EWGB	16	//WGS66
#define	EWGC		17	//WGS72
#define	EWGE		18	//WGS84
#define	ELAST		18	// last ellipsoid index

// 데이텀 정의
#define DATUM_FIRST 101
#define DADI		101	// Adindan
#define DARF		102	// Arc1950
#define DCAZ		103	// Camp Area Astro
#define DEUR		104	// European Datum 1950
#define DENB		105	// European Datum 1979
#define DGEO		106	// Geodetic Datum 1849
#define DHTN		107	// Hu-Tzu-Shan
#define DIND		108	// Indian
#define DNAS		109	// North American Datum 1927
#define DNAX		110	// North American Datum 1983
#define DPRP		111	// Provisional American 1969
#define DTOK		112	// Tokyo
#define	DWGC		113	// World Geodetic System 1972
#define	DWGE		114	// World Geodetic System 1984
#define DATUM_LAST	114

typedef enum { NORTH, SOUTH } VERHEMISPHERE;
typedef enum { EAST,  WEST  } HORHEMISPHERE;

//
// 평면 좌표 구조체
typedef struct tagPOINTPLANE {
	double		x;
	double		y;
} POINTPLANE;

//
// 경위도 좌표 구조체
typedef struct tagPOINTGP {
	double	lon;	// 경도
	double	lat;	// 위도
	double	ht;		// 높이
} POINTGP;

typedef struct tagSIZEGP
{
	double	clon;	// 경도
	double	clat;	// 위도
} SIZEGP;

typedef struct tagRECTGP {
	double left;	// 경도
	double top;		// 위도
	double right;	// 경도
	double bottom;	// 위도
	double ht;		// 높이
} RECTGP;

// UPS 좌표 구조체 : 북반구(Y또는Z), 남반구(A또는B)
typedef struct tagPOINTUPS {
	VERHEMISPHERE hemisphere;	// NORTH : 북반구, SOUTH : 남반구
	double		x;		// 단위 : 미터
	double		y;		// 단위 : 미터
} POINTUPS;

// POINTGK은 GK 좌표를 나타는 구조체
typedef struct tagPOINTGK {
	char	zone;		// 범위 : 1 - 60 (경도 6도 간격)
	double	x;	// 단위 : 미터
	double	y;	// 단위 : 미터
} POINTGK;
	
//
// UTM 좌표 구조체
typedef struct tagPOINTUTM {
	char		zone;	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	char		dgn;	// designator : 구간 구분 문자
				//		80'S ~ 84'N 위도 구간을 8도 간격으로 구분
	double		x;	// 단위 : 미터
	double		y;	// 단위 : 미터 ( + :북반구, - :남반구 ) : UTM이나 MGRS를 표시할 경우만 10,000,000m를 계산해 준다
} POINTUTM;

typedef struct tagSIZEUTM {
	double		cx;	// 단위 : 미터
	double		cy;	// 단위 : 미터 ( + :북반구, - :남반구 )
	char		zone;	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	char		dgn;	// designator : 구간 구분 문자
				//		80'S ~ 84'N 위도 구간을 8도 간격으로 구분
} SIZEUTM;

typedef struct tagRECTUTM {
	double		left;	// 단위 : 미터
	double		top;	// 단위 : 미터 ( + :북반구, - :남반구 )
	double		right;	// 단위 : 미터
	double		bottom;	// 단위 : 미터 ( + :북반구, - :남반구 )
	char		zone;	// 범위 (zone) : 1 - 60 (경도 6도 간격)
	char		dgn;	// designator : 구간 구분 문자
				//		80'S ~ 84'N 위도 구간을 8도 간격으로 구분
} RECTUTM;

// POINTGEOREF은 GEOREF 좌표를 나타내는 구조체
typedef struct tagPOINTGEOREF {
	BYTE	idxlon;   // 경도를 15도 간격으로 나눈 INDEX값(24개 문자)
	BYTE	deglon;   // 경도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	BYTE	minlon;   // 0 ~ 59 사이의 값 (경도 1도를 1분 단위로 분할한 값)
	BYTE	seclon;   // 0 ~ 9 사이의 값 (경도 1분을 6초 단위로 분할한 값)
	BYTE	inxlat;   // 위도를 15도 간격으로 나눈 INDEX값(12개 문자)
	BYTE	deglat;   // 위도 15도 간격을 1도 단위로 나눈 값(15개 문자)
	BYTE	minlat;   // 0 ~ 59 사이의 값 (위도 1도를 1분 단위로 분할한 값)
	BYTE	seclat;   // 0 ~ 9 사이의 값 (위도 1분을 6초 단위로 분할한 값)
} POINTGEOREF;

// POINTNE는 NE 좌표를 나타내는 구조체
typedef struct tagPOINTNE {
	BYTE	xArea;
	UINT	xSec;
	BYTE	yArea;
	UINT	ySec;
} POINTNE;

// UTM Grid Zone Designations : 위도 80도S - 84도N 구간
typedef enum { C = 'C', D = 'D', E = 'E', F = 'F', G = 'G',
			   H = 'H', J = 'J', K = 'K', L = 'L', M = 'M',
			   N = 'N', P = 'P', Q = 'Q', R = 'R', S = 'S',
			   T = 'T', U = 'U', V = 'V', W = 'W', X = 'X' } UTMZONEDGN;
typedef unsigned int UTMZONENUM;

#define GUNIT_MM	1
#define GUNIT_CM	2
#define GUNIT_M		3
#define GUNIT_KM	4
#define GUNIT_SEC	5
#define	GUNIT_MIN	6
#define GUNIT_DEG	7

#define EQUATOR			  0.0
#define PRIME_MERIDIAN	  0.0
#define NORTH_POLE		 90.0
#define SOUTH_POLE		-90.0

#define GK_SCALE_FACTOR		      1.0
#define GK_FALSE_EASTING	 500000.0
#define GK_FALSE_NORTHING  10000000.0

#define UPS_SCALE_FACTOR	0.994
#define UPS_FALSE_EASTING	2000000.0
#define UPS_FALSE_NORTHING	2000000.0

#define UTM_SCALE_FACTOR	0.9996
#define UTM_FALSE_EASTING	  500000.0
#define UTM_FALSE_NORTHING  10000000.0

#define ERR_GP_RANGE -999

#define SEPARATOR_NONE	0
#define SEPARATOR_BLANK 1
#define SEPARATOR_COMMA 2

#define TYPE_GP_1	1	// 127:30:53E 38:55:20N
#define TYPE_GP_2	2	// 127:30:53.5E 38:55:20.5N
#define TYPE_GP_3	3	// 1273053E 385520N
#define TYPE_GP_4	4	// 12730535E 3855205N
#define TYPE_GP_5	5	// 127.514722E 38.922222N
#define TYPE_GP_6	6	// +127.514722 +38.922222
#define TYPE_GP_7	7	// 동경:127.005147 북위:38.014722
#define	TYPE_GP_8	8	// 동경:127도30분53초 북위:38도55분20초
#define TYPE_GP_9	9	// 동경:127도30분53.5초 북위:38도55분20.5초

#define GPTYPE_FIRST	51
#define GPTYPE_GP		51
#define GPTYPE_GK		52
#define GPTYPE_UPS		53
#define GPTYPE_UTM		54
#define GPTYPE_LAST		55

#define GSTYPE_FIRST	61
#define GSTYPE_GP		61
#define GSTYPE_GEOREF	62
#define GSTYPE_GK		63
#define GSTYPE_MGRS		64
#define GSTYPE_NE		65
#define GSTYPE_UPS		66
#define GSTYPE_PLANE	67
#define GSTYPE_LAST		67


// Coord.cpp: implementation of the CCoord class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Coord.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoord::CCoord()
{
						 
	/* earth ellipsoid major axis */
	/* WGS 84        */
	/* WGS 72        */
	/* Krasovsky     */
	/* International */
	/* Clarke 1866   */
	/* Clarke 1880   */
	/* Bessel        */
	/* Everest       */
	/* Airy          */
	double array_a_temp[9]={6378137.1,
							6378135.0,
							6378245.0,
							6378388.0,
							6378206.4,
							6378249.145,
							6377397.155,
							6377276.3452,
							6377563.396};
	memcpy(array_a, array_a_temp,sizeof(array_a));
								 
	
	/* earth ellipsoid minor axis */

	double array_b_temp[9]={6356752.4,    /* WGS 84        */
							6356750.5,    /* WGS 72        */
							6356863.0,    /* Krasovsky     */
							6356911.9462, /* International */
							6356583.8,    /* Clarke 1866   */
							6356514.87,   /* Clarke 1880   */
							6356078.693,  /* Bessel        */
							6356075.415,  /* Everest       */
							6356256.909}; /* Airy          */
	memcpy(array_b,array_b_temp,sizeof(array_b));
								 
	/* earth ellipsoid flattening */
	double array_f_temp[9]={298.2572,     /* WGS 84        */
							298.26,       /* WGS 72        */
							298.3,        /* Krasovsky     */
							297.0,        /* International */
							294.978698,   /* Clarke 1866   */
							293.465,      /* Clarke 1880   */
							299.1528128,  /* Bessel        */
							300.8017,     /* Everest       */
							299.324965};  /* Airy          */
	memcpy(array_f,array_f_temp,sizeof(array_f)); 
	
	char alpha_number_temp[10]= {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	memcpy(alpha_number, alpha_number_temp, sizeof(alpha_number));

	char georef_label_temp[26]= 
		{	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M',
			'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    memcpy(georef_label, georef_label_temp, sizeof(georef_label));

	int m_num_temp[26] = 
		{	0,    1,   2,   3,   4,   5,   6,   7,   7,   8,   9,   10,  11, 
			12,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,   22,  23};
	memcpy(m_num, m_num_temp, sizeof(m_num));
	initialize_adaptation_data();
	initialize_coord_conversions();
}


CCoord::~CCoord()
{

}


/*****************************************************************************\
-------------------------->  COMPUTE COEFFICIENTS  <---------------------------
\*****************************************************************************/
void CCoord::compute_coefficients (double earth_eccentricity,
                             double *a_con,
                             double *b_con,
                             double *c_con,
                             double *d_con,
                             double *e_con,
                             double *f_con,
                             double *g_con,
                             double *h_con) 
{
	double a;
	double b;
	double c;
	double d;
	double ee_squared = earth_eccentricity * earth_eccentricity;
	double ee_to_the_4th = pow(earth_eccentricity, 4.0); 
	double ee_to_the_6th = pow(earth_eccentricity, 6.0);

	a = 1.0 - ee_squared;

	b = (1.0 / 3.0) * ((1.0 - ee_to_the_4th) -  (pow((1.0 - ee_squared),3.0)));
    
	c = (1.0 / 5.0) * (1.0 - ee_to_the_6th) -
		(1.0 / 3.0) * (pow((1.0 - ee_squared),2.0)) * (1.0 - ee_to_the_4th) +
		(2.0 / 15.0)* (pow((1.0 - ee_squared),5.0));

	d = (1.0 / 7.0) * (1.0 - ee_squared * ee_to_the_6th) -
		(1.0 / 9.0) * (1.0 - ee_squared) * (pow((1.0 - ee_to_the_4th),2.0)) -
		(1.0 / 5.0) * (pow((1.0 - ee_squared),2.0)) * (1.0 - ee_to_the_6th) +
		(2.0 / 9.0) * (pow((1.0 - ee_squared),4.0)) * (1.0 - ee_to_the_4th) -
		   (17.0 / 315.0) * (pow((1.0 - ee_squared),7.0));

	*a_con = a;

	*b_con = b;

	*c_con = c;

	*d_con = d;

	*e_con = 1.0 / a;

	*f_con = -b / pow(a, 4.0);

	*g_con = ((3.0 * b * b) - (a * c))/ pow(a,7.0);

	*h_con = ((8.0 * a * b * c) - (d * a * a) -  
					(12.0 * b * b * b)) / pow(a,10.0);

} 

/*****************************************************************************\
------------------------->  COMPUTE EARTH MODEL DATA  <------------------------
\*****************************************************************************/
void CCoord::compute_earth_model_data (double equatorial_radius,
									double flatness,
									double *earth_eccentricity,
									double *polar_earth_radius,
									double *a_con,
									double *b_con,
									double *c_con,
									double *d_con,
									double *e_con,
									double *f_con,
									double *g_con,
									double *h_con)
{
	double eccentricity;

	*polar_earth_radius = compute_polar_earth_radius (equatorial_radius_nm,
													 flatness);

	eccentricity = compute_eccentricity (flatness);

	*earth_eccentricity = eccentricity;

	compute_coefficients (eccentricity,
						a_con,
						b_con,
						c_con,
						d_con,
						e_con,
						f_con,
						g_con,
						h_con);

} 

/*****************************************************************************\
------------------------>  COMPUTE ECCENTRICITY  <-----------------------------
\*****************************************************************************/
double CCoord::compute_eccentricity (double flatness)
{			
   return (sqrt ((2.0 * flatness) - (flatness * flatness)));

} 

/*****************************************************************************\
-------------------->  COMPUTE ELLIPSOID CONSTANTS  <--------------------------
\*****************************************************************************/								
void CCoord::compute_ellipsoid_constants ()
{
	utm_k0	  = 0.9996; 	        /* utm scale factor.               */

	ellip_d_a = array_a[WGS_84];         /* selec. ellipsoid semimajor axes */
	ellip_d_b = array_b[WGS_84];         /* selec. ellipsoid semiminor axes */
	ellip_d_f = array_f[WGS_84];         /* selected ellipsoid flattening   */

									/* calculate eccentricity.         */
	ellip_e   = sqrt ((ellip_d_a * ellip_d_a - ellip_d_b *
					  ellip_d_b) / (ellip_d_a * ellip_d_a));
										/* second eccentricity squared     */
	ellip_e2s = (ellip_d_a * ellip_d_a - ellip_d_b *
				ellip_d_b) / (ellip_d_b * ellip_d_b);
										/* utm conversion constant n       */
	ellip_n   = (ellip_d_a - ellip_d_b) / (ellip_d_a + ellip_d_b);
 						/* utm conversion constant r.	   */
	ellip_r   = utm_k0 * ellip_d_a * (1-ellip_n);	

	u_cc_a    = ellip_d_a * (1.0 - ellip_n) * 
			   (1.0 + 5.0 / 4.0 * ellip_n * ellip_n); 		

	u_cc_b    = 3.0 / 2.0 * ellip_d_a * (1.0 - ellip_n) * 
			   (ellip_n + 7.0 / 8.0 * ellip_n * ellip_n * ellip_n);			

	u_cc_c = 15.0 / 16.0 * ellip_d_a * (1.0 - ellip_n) * ellip_n * ellip_n; 		
}

/*****************************************************************************\
----------------------->  COMPUTE POLAR EARTH RADIUS  <------------------------
\*****************************************************************************/
double CCoord::compute_polar_earth_radius (double equatorial_radius, double flatness)
{
   return (equatorial_radius * (1.0 - flatness));
} 

/*****************************************************************************\
---------------------------->  COMPUTE UTM BAND  <-----------------------------
\*****************************************************************************/
char CCoord::compute_utm_band(double gb_lat)
{								
			/*  u_band:	utm/mgr latitude band determination  */	
			/*  argument   gb_lat	latitude range -80 to 84     */
	int ir;
	char alpha_b;
	char alpha_table1[10] = {'M','L','K','J','H','G','F','E','D','C'};
	char alpha_table2[10] = {'N','P','Q','R','S','T','U','V','W','X'};

	if(gb_lat>=80)	                 /* special band               */
		ir = 9;
	else			
		ir = (int)fabs(gb_lat/8.0);	 /* normal bands               */
	if(gb_lat<0)	                         /* find grid zone character   */
		alpha_b = alpha_table1[ir];	 /* find letter, negative lats */
	else
		alpha_b = alpha_table2[ir];        /* find letter, positive lats */

	return (alpha_b);                     /* return band character      */
}

/*****************************************************************************\
--------------------->  COMPUTE UTM ZONE  <------------------------------------
\*****************************************************************************/				
	
int CCoord::compute_utm_zone(                /* utm/mgr longitude zone determination */
                     double gz_long, /* longitude range -180 to 180          */
                     char band)      /* latitude band range c - x            */
{						
	int iz;
												/* band v and 3 - 6 degrees  */
	if ((band =='V') && (gz_long >= 3) && (gz_long < 6))   
		return(32);	             
                               					/* band x and 6 - 9 degrees  */
	if ((band =='X') && (gz_long >= 6) && (gz_long < 9))   
		return(31);		
												/* band x and 9 - 12 degrees */
	if ((band =='X') && (gz_long >= 9) && (gz_long < 12))  
		return(33);	
												/* band x and 18 - 21 degrees*/
	if ((band =='X') && (gz_long >= 18) && (gz_long < 21)) 
		return(33);	
												/* band x and 21 - 24 degrees*/
	if ((band =='X') && (gz_long >= 21) && (gz_long < 24)) 
		return(35);
      											/* band x and 30 - 33 degrees*/
	if ((band =='X') && (gz_long >= 30) && (gz_long < 33)) 
		return(35);
      											/* band x and 33 - 36 degrees*/
	if ((band =='X') && (gz_long >= 33) && (gz_long < 36))
		return (37);
      											/* zone calculation          */
	iz = int(1 + (gz_long + 180) / 6.0);	             

	return (iz);	
}
/*****************************************************************************\
--------------------------->  DECLAT DMS  <------------------------------------
\*****************************************************************************/		
void CCoord::declat_dms(                /* convert decimal latitude degrees to d-m-s */
						double declat,  /* range -90 to +90                          */
						char *string)   /* longitude converted to an ascii string    */
{
	int lat_deg;
	int lat_min;
	int lat_sec;
	char n_s;
	double lat_deg_abs;
	double lat_minplussec;
	char string_temp[50]="";

	lat_deg_abs = fabs (declat); 
   													/* latitude degrees */
	lat_deg	= (int)lat_deg_abs;	
	lat_minplussec	= (lat_deg_abs - lat_deg) * 60;	
														/* latitude minutes */
	lat_min	= (int)lat_minplussec;
														/* latitude seconds */
	lat_sec	= int(.5 + (lat_minplussec - lat_min) * 60); 

	if (declat < 0) /* find the the sign for North/South */
		n_s = 'S';
	else
		n_s = 'N';
	if (lat_sec > 59)
	{
		lat_min += 1;
		lat_sec -= 60;
	}
	if (lat_min > 59)
	{
		lat_deg += 1;
		lat_min -= 60;
	}
	sprintf(string_temp, "%02d%02d%02d%c",lat_deg,lat_min,lat_sec,n_s);
	memcpy(string,string_temp,strlen(string_temp));
}						
						
/*****************************************************************************\
--------------------------->  DECLON DMS  <------------------------------------
\*****************************************************************************/																
void CCoord::declon_dms(               /* convert decimal longitude degrees to d-m-s */
                double declon, /* range -180 to +180 degrees                 */
                char *string)  /* latitude converted to an ascii string      */
{
	int lon_deg;
	int lon_min;
	int lon_sec;
	char e_w;
	double lon_deg_abs;
	double lon_minplussec;
	char string_temp[50]="";

	lon_deg_abs = fabs (declon); 	
										/* longitude degrees */
	lon_deg	= (int)lon_deg_abs;
	lon_minplussec = (lon_deg_abs - lon_deg) * 60;
   									/* longitude minutes */
	lon_min	= (int)lon_minplussec;
										/* longitude seconds */
	lon_sec	= int(.5 + (lon_minplussec - lon_min) * 60);

	if (declon < 0) /* find the the sign for East/West */
		e_w = 'W';
	else
		e_w = 'E';
	if (lon_sec > 59)
	{
		lon_min += 1;
		lon_sec -= 60;
	}
	if (lon_min>59)
	{
		lon_deg += 1;
		lon_min -= 60;
	}
	sprintf(string_temp, "%03d%02d%02d%c", lon_deg, lon_min, lon_sec, e_w);		
	memcpy(string,string_temp,strlen(string_temp));

}
			
/*****************************************************************************\
------------->  DEGREES LATITUDE TO RADIANS CONFORMAL LATITUDE  <--------------
\*****************************************************************************/
void CCoord::degrees_latitude_to_radians_conformal_latitude (
									double geographic_latitude,
									double *conformal_latitude) 
{
	double geographic_lat_in_radians;

	/* convert input argument into radians */
	geographic_lat_in_radians = geographic_latitude * DEGREES_TO_RADIANS; 

	/* deg_geo_lat_to_rad_conform_lat in radians */
	radians_latitude_to_radians_conformal_latitude (geographic_lat_in_radians, 
												   conformal_latitude);
} 
 
/*****************************************************************************\
------------------------->  DEGREES LAT LONG TO X Y  <-------------------------
\*****************************************************************************/
void CCoord::degree_lat_long_to_x_y (LAT_LONG_TYPE lati, LAT_LONG_TYPE longi, double *x_coord, double *y_coord) 
{
	double geo_lati, geo_longi;
	int dec, min, sec;
	char dir_chr;
	char strlati[7]="";
	char strlongi[8]="";

	dec=(int)lati.degree;
	min=(int)lati.minute;
	sec=(int)lati.second;
	dir_chr=lati.direct;
	sprintf(strlati, "%02d%02d%02d%c", dec,min,sec,dir_chr);
	dms_to_dec_deg (strlati, &geo_lati);
	dec=(int)longi.degree;
	min=(int)longi.minute;
	sec=(int)longi.second;
	dir_chr=longi.direct;
	sprintf(strlongi, "%03d%02d%02d%c", dec,min,sec,dir_chr);
	dms_to_dec_deg(strlongi, &geo_longi);
	degrees_lat_long_to_x_y(geo_lati, geo_longi, x_coord, y_coord);
}


void CCoord::degrees_lat_long_to_x_y (double point_geographic_latitude,
									double point_geographic_longitude,
									double *x_coordinate,
									double *y_coordinate) 
{	 		                  
	double point_geo_lat_in_radians;
	double point_geo_long_in_radians;

	/* convert lat/long to radians */

	point_geo_lat_in_radians = point_geographic_latitude * DEGREES_TO_RADIANS;

	point_geo_long_in_radians = point_geographic_longitude * DEGREES_TO_RADIANS;

	/* degrees_lat_long_to_x_y {radians} */
	//	TRACE("a coe : %f ",a_coefficient);
	radians_lat_long_to_x_y (point_geo_lat_in_radians,
					point_geo_long_in_radians,
					x_coordinate, 
					y_coordinate);
}

/*****************************************************************************\
-------------------------->  DMS TO DEC DEG      <-----------------------------
\*****************************************************************************/
void CCoord::dms_to_dec_deg (char *dms, double *deg)
{
	char   de[4];
	char   mn[3];
	char   se[3];   

	if (strlen(dms) == 7)
	{    
		de[0] = dms[0];
		de[1] = dms[1];
		de[2] = '\0';
		mn[0] = dms[2];
		mn[1] = dms[3];
		mn[2] = '\0';
		se[0] = dms[4];
		se[1] = dms[5];
		se[2] = '\0';

		*deg = atoi(de) + atoi(mn)/60.0 + atoi(se)/3600.0;

		if (dms[6] == 'S' || dms[6] == 's')
		{ 
			*deg = -(*deg);
		}
	}
	else				
	{	           
		de[0] = dms[0];
		de[1] = dms[1];
		de[2] = dms[2];
		de[3] = '\0';
		mn[0] = dms[3];
		mn[1] = dms[4];
		mn[2] = '\0';
		se[0] = dms[5];
		se[1] = dms[6];	            
		se[2] = '\0';
		*deg = atoi(de) + atoi(mn)/60.0 + atoi(se)/3600.0;

		if (dms[7] == 'W' || dms[7] == 'w')
		{ 
			*deg = -(*deg);	           
		}
	}
}
     
/*****************************************************************************\
------------------------------>  D LONG  <-------------------------------------
\*****************************************************************************/
double CCoord::d_long (             /* utm/mgr delta longitude calculation          */
               double lng,  /* longitude range -180 to 180                  */
               int    zone) /* zone range 1 to 60                           */ 
{				
	double cm;
										/* find central meridian value.      */
	cm	= 6 * zone - 183;
										/* return delta longitude (radians). */
	return ((lng - cm) * PI / 180);
}

/*****************************************************************************\
--------------------->  GEOREF TO DEGREES LAT LONG  <--------------------------
\*****************************************************************************/
void CCoord::georef_to_degrees_lat_long (char *georef_point,
		                 double *lat_p,
		                 double *long_p)
{
	double  geo_lat_in_radians;
	double  geo_long_in_radians;

	georef_to_radians_lat_long (georef_point, 
								&geo_lat_in_radians, 
								&geo_long_in_radians);

	*lat_p = geo_lat_in_radians * RADIANS_TO_DEGREES;
                               
	*long_p = geo_long_in_radians * RADIANS_TO_DEGREES;                                    
}

/*****************************************************************************\
------------------------>  GEOREF TO RADIANS LAT LONG  <-----------------------
\*****************************************************************************/
void CCoord::georef_to_radians_lat_long (char *georef_point,
			         double *point_geographic_latitude,
			         double *point_geographic_longitude) 
{
	/*   implementation notes:                                      */
	/*   this conversion returns only latitude and logitude data    */
	/*   with respect to a major grid which is not know. a major    */
	/*   grid is defined as an area with 15 degrees in lantitude    */
	/*   and longitude. each major grid starts at integral 15       */
	/*   degrees in lantitude and longitude.                        */

	long  major_lat_grid_minutes;
	long  major_long_grid_minutes;
	long  minor_lat_grid_minutes;
	long  minor_long_grid_minutes;
	long  minor_lat_minutes;
	long  minor_long_minutes;
	double temp_geo_latitude;
	double temp_geo_longitude;

	major_long_grid_minutes = MAJOR_GRID_TO_MIN * m_num[georef_point [0] - 
									georef_label[0]];
	major_lat_grid_minutes  = MAJOR_GRID_TO_MIN * m_num[georef_point [1] - 
									georef_label[0]];
	minor_long_grid_minutes = MINOR_GRID_TO_MIN * m_num[georef_point [2] - 
									georef_label[0]];
	minor_lat_grid_minutes  = MINOR_GRID_TO_MIN * m_num[georef_point [3] - 
									georef_label[0]];

	minor_long_minutes = 10 * (georef_point [4]-alpha_number[0]) +
								georef_point [5]-alpha_number[0];
				 
	minor_lat_minutes =  10 * (georef_point [6]-alpha_number[0]) +
								georef_point [7]-alpha_number[0];
		
	temp_geo_longitude = MINUTES_TO_RADIANS * (major_long_grid_minutes +
							minor_long_grid_minutes +
							minor_long_minutes);

	*point_geographic_longitude = temp_geo_longitude - PI;

	temp_geo_latitude = MINUTES_TO_RADIANS * (major_lat_grid_minutes +
									minor_lat_grid_minutes + minor_lat_minutes);

	*point_geographic_latitude = temp_geo_latitude - PI_OVER_2;
}

/*****************************************************************************\
------------------------------>  GEOREF TO X Y  <------------------------------
\*****************************************************************************/
void CCoord::georef_to_x_y (char   *georef_point,
							double *x_coordinate,
							double *y_coordinate) 
{
	double geographic_latitude;
	double geographic_longitude;

	georef_to_degrees_lat_long (georef_point, 
								&geographic_latitude, 
								&geographic_longitude);

	degrees_lat_long_to_x_y (geographic_latitude, geographic_longitude,
							x_coordinate, y_coordinate);

}
/*****************************************************************************\
------------------->  INITITIALIZE COORD CONVERSIONS  <------------------------
\*****************************************************************************/
void CCoord::initialize_coord_conversions()
{
	initialize_adaptation_data();
	compute_ellipsoid_constants();
}   
/*****************************************************************************\
--------------------->  INITITIALIZE ADAPTATION DATA  <------------------------
\*****************************************************************************/
void CCoord::initialize_adaptation_data()
{     
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int    lat_deg;
	int    lat_min;
	int    lat_sec;
	int    long_deg;
	int    long_min;
	int    long_sec;
	char   lat_chr;
	char   long_chr;
	double scc_geo_lat_in_radians;
	double scc_geo_long_in_radians;
	double flatness;
	double scc_position_latitude;
	double scc_position_longitude;

	//(start)CCoord원점좌표변경-050127
	lat_deg=37;
	lat_min=00;
	lat_sec=00;
	lat_chr='N';
	long_deg=127;
	long_min=00;
	long_sec=00;
	long_chr='E';
	flatness=0.0;
	/*
	lat_deg=37;
	lat_min=05;
	lat_sec=10;
	lat_chr='N';
	long_deg=127;
	long_min=01;
	long_sec=52;
	long_chr='E';*/
	//(original)CCoord원점좌표변경-050127
	/*
	lat_deg=37;
	lat_min=07;
	lat_sec=10;
	lat_chr='N';
	long_deg=127;
	long_min=03;
	long_sec=52;
	long_chr='E';
	*/
	//(end)CCoord원점좌표변경-050127

/*	sscanf (system_reference_center (),
			"%2d%2d%2d%1c%*c%3d%2d%2d%1c",
			&lat_deg,
			&lat_min,
			&lat_sec,
			&lat_chr,
			&long_deg,
			&long_min,
			&long_sec,
			&long_chr); */

	scc_position_latitude  =	(float) lat_deg +
								(float) lat_min /60.0 +
								(float) lat_sec /3600.0;
                        
	if ((lat_chr == 'S') || (lat_chr == 's'))
		scc_position_latitude = -(scc_position_latitude);
  
	scc_position_longitude =	(float) long_deg +
								(float) long_min /60.0 +
								(float) long_sec /3600.0;
                        
	if ((long_chr == 'W') || (long_chr == 'w'))
		scc_position_longitude = -(scc_position_longitude); 

	earth_model = wgs_84;              /* from APL_SYSTEM_ADAPT_FILE */
	conformal_earth_radius = 3430.619; /* from APL_SYSTEM_ADAPT_FILE */
	effective_earth_radius = 4574.16;  /* from APL_SYSTEM_ADAPT_FILE */
	epsilon = 0.000000953674;          /* 9.53674E-07                */
	switch (earth_model) {
		case hayford_1910:
			equatorial_radius_nm = 3444.053996; /* nautical miles = 6378388 meters */ 
			flatness = 1.0 / 297.0;
			break; 
     	case wgs_72:
			equatorial_radius_nm = 3443.917387;
			flatness = 1.0 / 298.26;
			break;
        case wgs_84:
			equatorial_radius_nm = 3443.9185;
			flatness = 1.0 / 298.2572;
			break;
		case bessel_1841:
			equatorial_radius_nm = 3443.518898;
			flatness = 1.0 / 299.2;
			break;
	} /* end switch */
	compute_earth_model_data (equatorial_radius_nm,
								flatness,
								&earth_eccentricity,
								&polar_earth_radius,
								&a_coefficient,
								&b_coefficient,
								&c_coefficient,
								&d_coefficient,
								&e_coefficient,
								&f_coefficient,
								&g_coefficient,
								&h_coefficient);


	/* system coordinate center (scc) position               */
	/* coordinates are in geographic latitude and longitude  */
	/* scc_position = sin_sad.own_site_scc_position          */


	scc_geo_lat_in_radians = scc_position_latitude * DEGREES_TO_RADIANS;

	radians_latitude_to_radians_conformal_latitude (scc_geo_lat_in_radians, &scc_conformal_lat_in_radians);

	scc_geo_long_in_radians = scc_position_longitude * DEGREES_TO_RADIANS;

	initialize_geo_data (conformal_earth_radius, scc_geo_long_in_radians, scc_conformal_lat_in_radians);

}     

/*****************************************************************************\
-------------------------->  INITIALIZE GEO DATA  <----------------------------
\*****************************************************************************/

void CCoord::initialize_geo_data  (double conformal_earth_radius_in,  
								double scc_longitude,  
								double scc_conformal_latitude) 
{
	pts_conformal_earth_radius =  conformal_earth_radius_in;
	pts_sector_conformal_lat = scc_conformal_latitude;
	radians_conformal_lat_to_radians_lat (scc_conformal_latitude, 
										 &pts_sector_lat);
	pts_sector_long = scc_longitude;
	pts_earth_rad_denominator = 4.0 * pow(pts_conformal_earth_radius,2.0);
	pts_sin_ref_lat = sin (pts_sector_conformal_lat);
	pts_cos_ref_lat = cos (pts_sector_conformal_lat);
	two_e_sub_0 = 2.0 * pts_conformal_earth_radius;
	four_e_sub_0 = 4.0 * pts_conformal_earth_radius;
	four_e_sub_0_sqr = 4.0 * pts_conformal_earth_radius * pts_conformal_earth_radius;
	four_e_sub_0_x_cos_lat = four_e_sub_0 * pts_cos_ref_lat;
	four_e_sub_0_x_sin_lat = four_e_sub_0 * pts_sin_ref_lat;
}

/*****************************************************************************\
------------------------------>  M ALPHA  <------------------------------------
\*****************************************************************************/						
char CCoord::m_alpha(                    /* mgr 100,000 meter square determination   */
				int m_nsd)          /* square desig, range 1-24                 */
{
	char m_ltr;

	m_ltr = georef_label [m_nsd-1];	       /* find nsd letter            */

	return (m_ltr);
}
						
/*****************************************************************************\
----------------------->  MGR TO RADIANS LAT LONG  <---------------------------
\*****************************************************************************/				
void CCoord::utm_to_radians_lat_long (char   *mgr_string_unpadded,
								double *lat_rad,
								double *lng_rad)
{
	int u_zone;                      /* utm zone, range 1 - 60                */
	char u_band;                     /* utm band, range c - x except I, O     */
	char utm_string[50]="";
	char mgr_string[50]="";
	char mgr_ltr_e;
	char mgr_ltr_n;
	int mgr_nsd_e;
	int mgr_nsd_n;
	int mgr_east;
	int mgr_north;
	int u_app_lat;
	int utm_east;
	int utm_north;

	//KJCCS에서 육상군정보를 10자리,8자리
	strcpy(mgr_string,mgr_string_unpadded);
	int mgrsLen;
	mgrsLen = strlen(mgr_string);
	if(mgrsLen ==15) //10자리 
	{
		mgr_string[17]='\0';
		mgr_string[16]=mgr_string[14];
		mgr_string[15]=mgr_string[13];
		mgr_string[14]=mgr_string[12];
		mgr_string[13]=mgr_string[11];
		mgr_string[12]=mgr_string[10];
		mgr_string[11] = ' ';
		mgr_string[10] =mgr_string[9];
		mgr_string[9] = mgr_string[8];
		mgr_string[8] = mgr_string[7];
		mgr_string[7] = mgr_string[6];
		mgr_string[6] = mgr_string[5];
		mgr_string[5] = ' ';

	}
	else if(mgrsLen ==13) //8자리 
	{
		mgr_string[17]='\0';
		mgr_string[16]='0';
		mgr_string[15]=mgr_string[12];
		mgr_string[14]=mgr_string[11];
		mgr_string[13]=mgr_string[10];
		mgr_string[12]=mgr_string[9];
		mgr_string[11] = ' ';
		mgr_string[10] ='0';
		mgr_string[9] = mgr_string[8];
		mgr_string[8] = mgr_string[7];
		mgr_string[7] = mgr_string[6];
		mgr_string[6] = mgr_string[5];
		mgr_string[5] = ' ';

	}
	else if(mgrsLen == 11) //6자리
	{
		mgr_string[17]='\0';
		mgr_string[16]='0';
		mgr_string[15]='0';
		mgr_string[14]=mgr_string[10];
		mgr_string[13]=mgr_string[9];
		mgr_string[12]=mgr_string[8];
		mgr_string[11] = ' ';
		mgr_string[10] ='0';
		mgr_string[9] = '0';
		mgr_string[8] = mgr_string[7];
		mgr_string[7] = mgr_string[6];
		mgr_string[6] = mgr_string[5];
		mgr_string[5] = ' ';
	}
	else if(mgrsLen == 9) //4자리 
	{
		mgr_string[17]='\0';
		mgr_string[16]='0';
		mgr_string[15]='0';
		mgr_string[14]='0';;
		mgr_string[13]=mgr_string[8];
		mgr_string[12]=mgr_string[7];
		mgr_string[11] = ' ';
		mgr_string[10] ='0';
		mgr_string[9] = '0';
		mgr_string[8] = '0';;
		mgr_string[7] = mgr_string[6];
		mgr_string[6] = mgr_string[5];
		mgr_string[5] = ' ';

	}
	
	u_zone    = atol (mgr_string); 
	u_band    = mgr_string [2];
	mgr_ltr_e = mgr_string [3];
	mgr_ltr_n = mgr_string [4];
	mgr_nsd_e = m_number (mgr_ltr_e);
	mgr_nsd_n = m_number (mgr_ltr_n);
	mgr_east  = atol (&mgr_string[6]);
	mgr_north = atol (&mgr_string[12]);
											/* utm approximate latitude       */
	u_app_lat = (m_number (u_band) - 13) * 8 + 4; 
											/* utm easting from mgr easting   */
	utm_east  = (int) u_m_east (u_zone, mgr_east, mgr_nsd_e); 		
											/* utm northing from mgr northing */	
	utm_north = (int) u_m_north ((int)ellip_d_a, u_zone, mgr_north, mgr_nsd_n, u_app_lat);
 			
	sprintf(utm_string, "%02d%c %06d %07d", u_zone, u_band, utm_east, utm_north);
											/* convert utm data to lat/long   */
	utm_internal_to_radians_lat_long (utm_string,
								lat_rad,
								lng_rad); 

}
						
/*****************************************************************************\
--------------------------------->  MNSD1 A <----------------------------------
\*****************************************************************************/
int CCoord::mnsd1_a (                 /* mgr adjusted easting 100,000 m2 designation */
				int nsd1,         /* initial nsd range 1 - 8                     */
				int m_zone_est)   /* zone range 1 to 60                          */
{             						
	int ma;

	ma = (m_zone_est - 1) % 3;	
	return (nsd1 + (ma * 8));              /* adjusted  easting nsd           */
}

/*****************************************************************************\
--------------------------------->  MNSD2 A  <---------------------------------
\*****************************************************************************/
int CCoord::mnsd2_a (                /* mgr adjusted northing 100,000 m2 designation */	
				int nsd2,       /* initial nsd range 0 - 99                     */
				int m_zone_nor) /* zone range 1 to 60                           */
{
	int mb;
	int nsd_a_nor;

	mb = nsd2 % 20;	
	if ((m_zone_nor % 2) == 1)	
		nsd_a_nor = mb + 1;	         /* odd utm zone values.             */
	else	
		nsd_a_nor = 1 + (mb + 5) % 20;     /* even utm zone values.            */

	return (nsd_a_nor);	                 /* adjusted  northing nsd           */
}

/*****************************************************************************\
----------------------------->  M NUMBER  <------------------------------------
\*****************************************************************************/
int CCoord::m_number (char m_ltr)
{
	int number;

	number = m_num [m_ltr - 'A'] + 1;
	return(number);
}

/*****************************************************************************\
------------------>  RADIANS CONFORMAL LAT TO RADIANS LAT  <-------------------
\*****************************************************************************/
void CCoord::radians_conformal_lat_to_radians_lat (double conformal_latitude,
											double *geographic_latitude)
{

	/* implementation notes:                                               */
	/*                                                                     */
	/*                          2          4          6                    */
	/*      lg = arsin[ (e+f*sin (lc)+g*sin (lc)+h*sin (lc)) * sin(lc) ]   */
	/*                                                                     */
	/*    where   e =  1.00673681                                          */
	/*            f = -0.00684471 ( note: adaptable )                      */
	/*            g =  0.00007755                                          */
	/*            h = -0.00000094                                          */
	/*           lg = geographic latitude                                  */
	/*           lc = conformal latitude                                   */
	/*                                                                     */

	double trig_e_const; /* =  1.00673681;*/
	double trig_f_const; /* = -0.00684471;*/
	double trig_g_const; /* =  0.00007755;*/
	double trig_h_const; /* = -0.00000094;*/
	double sin_conformal_lat;

	sin_conformal_lat = sin (conformal_latitude);

	trig_e_const = e_coefficient;
	trig_f_const = f_coefficient;
	trig_g_const = g_coefficient;
	trig_h_const = h_coefficient;

	*geographic_latitude = asin ((trig_e_const + trig_f_const *
									sin_conformal_lat * sin_conformal_lat +
									trig_g_const * pow(sin_conformal_lat,4.0) +
									trig_h_const * pow(sin_conformal_lat,6.0)) *
									sin_conformal_lat);
}

/*****************************************************************************\
--------->  RADIANS LATITUDE TO RADIANS CONFORMAL LATITUDE  <------------------
\*****************************************************************************/
void CCoord::radians_latitude_to_radians_conformal_latitude (
				double geographic_latitude,
				double *conformal_latitude) 
{
	/*  implementation notes:                                               */
	/*                                                                      */
	/*                          2          4          6                     */
	/*      lc = arsin[ (a+b*sin (lg)+c*sin (lg)+d*sin (lg)) * sin(lg) ]    */
	/*                                                                      */
	/*    where   a =  0.99327733                                           */
	/*            b =  0.00666251 ( note: adaptable )                       */
	/*            c =  0.00005959                                           */   
	/*            d =  0.00000056                                           */
	/*           lg = geographic latitude                                   */
	/*           lc = conformal latitude                                    */
	/*                                                                      */
   
	double sin_geographic_lat;
	sin_geographic_lat = sin (geographic_latitude);

	*conformal_latitude = asin ((a_coefficient + 
							b_coefficient * pow(sin_geographic_lat, 2.0) +
							c_coefficient * pow(sin_geographic_lat, 4.0) +
							d_coefficient * pow(sin_geographic_lat, 6.0)) *
							sin_geographic_lat);
} 

/*****************************************************************************\
----------------------->  RADIANS LAT LONG TO GEOREF  <------------------------
\*****************************************************************************/
void CCoord::radians_lat_long_to_georef (double point_geographic_latitude,
									double point_geographic_longitude,
									char *georef_point) 
{
	int latitude_minutes;
	int latitude_degrees;
	int longitude_minutes;
	int longitude_degrees;
	int minutes_digits;
	double lx;

	/* multiply by (60.0 min/deg * 100.0) and divide by 100 (integer)   */
	/* in order to truncate and add (180 deg * 60 deg/min = 10800 min)  */
	/* to make it be in the interval [0,360].                           */

	lx = point_geographic_longitude * RADIANS_TO_DEGREES * 6000.0;
	if (lx > 0)
		lx = lx + .5;
	else
		lx = lx - .5; 
	longitude_minutes = (int)(lx / 100) + 10800; 
	longitude_degrees = (longitude_minutes / 60) % 360; 

	/* multiply by 60.0 min/deg * 100.0 and divide by 100 (integer)           */
	/* in order to truncate and add (90 deg * 60 min/deg = 5400 min) to make  */
	/* latitude relative to south pole (make lattitude be in interval [0,180])*/
                                                         
	lx = point_geographic_latitude * RADIANS_TO_DEGREES * 6000.0;
	if (lx > 0)
		lx = lx + .5;
	else
		lx = lx - .5; 
	latitude_minutes = (int)(lx / 100) + 5400; 
	latitude_degrees = latitude_minutes / 60;

	georef_point [0] = georef_label [longitude_degrees / DEGREES_PER_MAJOR_GRID];
	georef_point [1] = georef_label [latitude_degrees / DEGREES_PER_MAJOR_GRID];
	georef_point [2] = georef_label [longitude_degrees % DEGREES_PER_MAJOR_GRID];
	georef_point [3] = georef_label [latitude_degrees % DEGREES_PER_MAJOR_GRID];
	minutes_digits = longitude_minutes % 60;
	georef_point [4] = alpha_number [minutes_digits / 10];
	georef_point [5] = alpha_number [minutes_digits % 10];
	minutes_digits = latitude_minutes % 60;
	georef_point [6] = alpha_number [minutes_digits / 10];
	georef_point [7] = alpha_number [minutes_digits % 10];
	georef_point [8] = '\0';
}

/*****************************************************************************\
--------------------->  RADIANS LAT LONG TO OTHER UTM  <-----------------------
\*****************************************************************************/							
void CCoord::radians_lat_long_to_other_utm(double rad_lat,
                                   double rad_lng,
                                   char utm_string[])
{
	double lat;
	double lng;
	double utm_dlong;
	int    utm_east;
	int    utm_north;
	int    u_band;
	int    u_zone;
	char   utm_string_temp[20]="";

	lat = rad_lat * 180.0 / PI; 
	lng = rad_lng * 180.0 / PI; 
									/* utm band designator c - x.   */
	u_band = compute_utm_band (lat);
										/* utm zone designator 1 - 60   */
	u_zone = compute_utm_zone (lng, u_band);
   									/* longitude from cent. meridan */
	utm_dlong = d_long (lng, u_zone); 
										/* calculated utm value r(c)    */
	utm_rc = rc (utm_k0, ellip_d_a, ellip_e, lat);	
										/* calculated utm easting       */  
	utm_east =  (int) u_east (utm_rc, ellip_e2s, utm_dlong, lat);	
										/* calculated utm northing      */ 
	utm_north = (int) u_north (utm_rc, ellip_r, ellip_n, utm_dlong, lat, u_band);

	sprintf(utm_string_temp, "%02d%c %06d %07d", u_zone, u_band, utm_east, utm_north);
	
	//utm_string = utm_string_temp;
	strcpy(utm_string, utm_string_temp);
}

/*****************************************************************************\
---------------------->  RADIANS LAT LONG TO UTM STR  <------------------------
\*****************************************************************************/							
void CCoord::radians_lat_long_to_utm_str(double rad_lat,
										double rad_lng,
										char utm_string[])
{
	double lat;
	double lng;
	double utm_dlong;
	int u_zone;
	int u_band;
	int utm_east;
	int utm_north;
	int nsd_east;
	int mgr_east;
	int mgr_north;
	int nsd_north;
	int mgr_nsda_n;
	int mgr_nsda_e;
	char mgr_ltr_est;
	char mgr_ltr_nor;
	char utm_string_temp	[20]="";
	char utm_string_large	[20]="";
	lat = rad_lat * 180.0 / PI; 
	lng = rad_lng * 180.0 / PI; 
									/* utm band designator c - x. */
	u_band = compute_utm_band (lat);
										/* utm zone designator 1 - 60 */
	u_zone = compute_utm_zone (lng, u_band); 
   									/* longitude from cent. meridan*/  	                                
	utm_dlong = d_long (lng, u_zone); 
										/* calculated utm value r(c)   */
	utm_rc = rc (utm_k0, ellip_d_a, ellip_e, lat);
										/* calculated utm easting      */  
	utm_east = (int) u_east (utm_rc, ellip_e2s, utm_dlong, lat); 
										/* calculated utm northing     */
	utm_north = (int) u_north (utm_rc, ellip_r, ellip_n, utm_dlong, lat, u_band);
										/* initial easting nsd.        */
	nsd_east = utm_east / 100000;
   									/* mgr easting value.          */
	mgr_east = utm_east - nsd_east * 100000;
   									/* initial northing nsd        */
	nsd_north = utm_north / 100000;
   									/* mgr northing value          */
	mgr_north = utm_north - nsd_north * 100000;	 
										/* mgr adjusted easting value  */
	mgr_nsda_e = mnsd1_a (nsd_east, u_zone);
										/* mgr adjusted northing value */
	mgr_nsda_n = mnsd2_a (nsd_north, u_zone);	
										/* mgr easting letter	       */
	mgr_ltr_est = m_alpha (mgr_nsda_e);
										/* mgr northing letter	       */
	mgr_ltr_nor = m_alpha (mgr_nsda_n);	

	sprintf(utm_string_large, "%02d%c%c%c %05d %05d",
				u_zone, u_band,mgr_ltr_est, mgr_ltr_nor,
				(int)(mgr_east),
				(int)(mgr_north)); 
    
	sprintf(utm_string_temp, "%02d%c%c%c%03d%03d",
				u_zone, u_band, mgr_ltr_est, mgr_ltr_nor,
				(int)(mgr_east / 100.0 + .5),
				(int)(mgr_north / 100.0 +.5)); 
	strcpy(utm_string, utm_string_temp);
}

/*****************************************************************************\
------------------------->  RADIANS LAT LONG TO X Y  <-------------------------
\*****************************************************************************/
void CCoord::radians_lat_long_to_x_y (double point_geographic_latitude,
									double point_geographic_longitude,
									double *x_coordinate,
									double *y_coordinate) 
{
	double   point_conformal_lat;
	double   delta_longitude;
	double   sin_delta_long;
	double   cos_delta_long;
	double   sin_point_lat;
	double   cos_point_lat;
	double   common_x_y_factor;
	double   temp_x;
	double   temp_y;

	radians_latitude_to_radians_conformal_latitude (point_geographic_latitude, 
													&point_conformal_lat);

	delta_longitude = point_geographic_longitude - pts_sector_long;

	sin_delta_long = sin (delta_longitude);
	cos_delta_long = cos (delta_longitude);
	sin_point_lat  = sin (point_conformal_lat);	
	cos_point_lat  = cos (point_conformal_lat);

	common_x_y_factor = 2.0 * pts_conformal_earth_radius /
					(1.0 + sin_point_lat * pts_sin_ref_lat +
					cos_point_lat * pts_cos_ref_lat * cos_delta_long);

	temp_x = common_x_y_factor * (sin_delta_long * cos_point_lat);
	temp_y = common_x_y_factor * (sin_point_lat * pts_cos_ref_lat -
					cos_point_lat * pts_sin_ref_lat * cos_delta_long);
/* 래스터맵을 위해서 임시로 막음
	*x_coordinate = temp_x;
	*y_coordinate = temp_y;
*/
	*x_coordinate = temp_x + 92.63483995;
	*y_coordinate = temp_y - 0.94031217;

} 

/*****************************************************************************\
---------------------------------->  RC  <-------------------------------------
\*****************************************************************************/
double CCoord::rc(                      /*  utm/mgr r(c) calculation                */	
			double rc_k0,         /*  scale factor constant                   */
			double rc_a,          /*  equatorial radius of earth (a)          */
			double rc_e,          /*  eccentricity of earth (e)               */
			double rc_lat)	/*  point latitude                          */					
{
	double rc_lat_s;
	double rc_lat_s2;
	double rc_ur0;
	double rc_e2;

	rc_e2 = rc_e * rc_e;	
   							  /* sine of latitude               */
	rc_lat_s  = sin (rc_lat * PI / 180.0); 
										  /* sine of latitude squared       */
	rc_lat_s2 = rc_lat_s * rc_lat_s;

	rc_ur0	= (3.0 / 8.0) * rc_e2 * rc_e2 * rc_lat_s2 *
							rc_lat_s2 + rc_e2 * rc_lat_s2 / 2.0 + 1;
										  /* calculated r(c)                */
	return (rc_k0 * rc_a * rc_ur0);	  
}   
 
/*****************************************************************************\
-------------------------------->  U EAST  <-----------------------------------
\*****************************************************************************/
double CCoord::u_east(                     /*  UTM Easting (meters)                  */
				double ue_rc,        /*  utm r(c) input                        */
				double ue_e1,        /*  utm e(1) input                        */
				double ue_dlong,     /*  utm delta longitude input (radians)   */
				double ue_lat)       /*  utm latitude input                    */
{	
	double ue_dlong2;
	double ue_lat_r;
	double ue_lat_s2;
	double ue_lat_c2;
	double ue0;
	double ue1;
	double ue2;
	double ue3;
	double ue4;
	double ue5;
	double ue6;
											  /* delta longitude squared     */
	ue_dlong2 = ue_dlong * ue_dlong;	    
											  /* latitude (radians).         */
	ue_lat_r  = ue_lat * PI/180;             
											  /* sine of latitude squared.   */
	ue_lat_s2 = sin(ue_lat_r) * sin(ue_lat_r); 
											  /* cosine of latitude squared. */
	ue_lat_c2 = cos(ue_lat_r) * cos(ue_lat_r); 

	ue0       = 5 * ue_dlong2 * ue_lat_c2 - (ue_dlong2 * ue_lat_s2 * 18);

	ue1       = 20 + 20 * ue_e1 * ue_lat_c2 + ue0;

	ue2       = ue_dlong2 * ue_lat_c2 / 120;

	ue3       = ue1 * ue2;	

	ue4       = 20-ue_dlong2 * ue_lat_s2;

	ue5       = ue_dlong2 * ue_lat_s2 / 120;

	ue6       =1 - ue4 * ue5 + ue3;	

	return (500000.0 + ue_rc * ue_dlong * cos(ue_lat_r) * ue6); 
}

/*****************************************************************************\
----------------------------->  U M EAST  <------------------------------------
\*****************************************************************************/		
double CCoord::u_m_east(                        /* UTM Easting Calculation           */
					int u_zone_e,           /* u_zone_e   UTM zone, range 1 - 60 */
					int m_east,             /* MGR easting, range 0 - 99999      */
					int m_nsd_e)            /* MGR NSD easting, range 1 - 24     */
{

	double u_east_ret;
	int    u_arem;

	u_arem = (u_zone_e - 1) % 3; 	

                                        /* utm calculated easting value      */
	u_east_ret = m_east + 100000 * (m_nsd_e - 8 * u_arem);			

	return (u_east_ret);				
}

/*****************************************************************************\
------------------------------->  U NORTH  <-----------------------------------
\*****************************************************************************/
double CCoord::u_north(                     /* UTM Northing calculation              */
				double un_rc,        /* utm r(c) input                        */
				double un_r,         /* utm r input                           */
				double un_n,         /* utm n input                           */
				double un_dlong,     /* utm delta longitude input (radians)   */
				double un_lat,       /* utm latitude input                    */
				char un_band)	    /* utm latitude band input               */			
{
	double un_dlong2;
	double un_lat_r;
	double un0;
	double un1;
	double un2;
	double un3;
	double un_north;
												/* delta longitude squared. */
	un_dlong2 = un_dlong * un_dlong;           
												/* latitude (radians).      */
	un_lat_r  = un_lat * PI/180.0;   
           
	un0 = 12.0 + 5.0 * un_dlong2 * cos(un_lat_r) * cos(un_lat_r) -
						un_dlong2 * sin(un_lat_r) * sin(un_lat_r);
                    
	un1 = (un_rc * un_dlong2 / 48) * sin(2.0 * un_lat_r) * un0; 	
	un2 = (1.0-(5.0/4.0) * un_n * cos(2 * un_lat_r)) * (3.0/2.0) * 
						un_r * un_n * sin(2 * un_lat_r);
                    
	un3 = 1 + 5.0/4.0 * un_n * un_n;	
										/* utm northing (meters)            */
	un_north = un_r * un_lat_r * un3 - un2 + un1+.5; 

										/* test for southern hemisphere.    */
	if (un_band < 'N') 
		un_north=un_north+10000000;	/* adjust utm northing.             */

	return (un_north);                   /* adjusted utm northing (meters).  */
}

/*****************************************************************************\
---------------------------->  U M NORTH  <------------------------------------
\*****************************************************************************/		
double CCoord::u_m_north(              /* utm northing calculation from mgr northing */
					int eq_rad,   /* equatorial radius of earth                 */ 
					int u_zone_n, /* utm zone, range 1 - 60                     */
					int m_north,  /* mgr northing, range 0 - 99999              */
					int m_nsd_n,  /* mgr nsd northing, range 1 - 20             */
					int u_lat_a)  /* utm approx latitude, -76 to +78.           */
{
	int u_brem;
	double  u_app_n;
	double  u_north_ret;
	int     u_mu;

	if ((u_zone_n%2)==1)	                 /* utm zone is odd                  */
		u_brem=m_nsd_n-1;	                 /* nsd less one for odd zones       */
	else	                                 /* utm zone is even                 */
		u_brem=m_nsd_n-6;	                 /* nsd less six for even zones      */
                                   
	if(u_brem<0)	                         /* shift negative values -5 thru -1 */
		u_brem=u_brem+20;	                 /* to +15 thru +19                  */	

										 /* approximate utm northing         */
	u_app_n	= u_lat_a * eq_rad / (180.0/PI); 

	u_mu = int(((u_app_n-m_north) / 2000000.0 - u_brem / 20.0) + .5);	

										 /* utm northing                     */
	u_north_ret	= m_north + u_brem * 100000 + u_mu * 2000000; 

							 /* shift northing value for southern hemisphere */
	if (u_lat_a<0)	                     
		u_north_ret = u_north_ret + 10000000;	

	return (u_north_ret);							
}

/*****************************************************************************\
--------------------->  UTM TO RADIANS LAT LONG  <-----------------------------
\*****************************************************************************/				
/* lat/long calc from utm easting & northing */
void CCoord::utm_internal_to_radians_lat_long (char *utm_string, double *lat_rad, double *lng_rad)
{
	double ll_north_a;
	double ll_s;
	double ll_plat_a;
	double ll_east_a;
	double cc_ca;
	double ll_plat0;
	double ll_v1;
	double cc_ba;
	double ll_plat_b;
	double ll_plat0_2;
	double ll_plat0_24;
	double ll_plat1;
	double ll_v2;
	double ll_plat2a;
	double ll_plat2bb;
	double ll_plat2b;
	double ll_plat2c;
	double ll_plat2;
	double ll_v3;
	double ll_plat3a;
	double ll_plat3bb;
	double ll_plat3c;
	double ll_long_cm;
	double ll_dlonga;
	double ll_plat3_rad;
	double ll_plat3b;
	double ll_dlongb;
	double ll_dlong_rad;
	int u_zone;								/* utm zone, range 1 - 60            */
	int u_band;								/* utm band, range c - x except I, O */
	int ll_east;							/* utm easting, range 0 - 999999     */
	int ll_north;							/* utm northing, range 0 - 9999999   */


	u_zone = atol(utm_string); 
	u_band = utm_string[2];                                
	ll_east = atol(&(utm_string[4])); 
	ll_north = atol(&(utm_string[11])); 
											/* remove false easting               */
	ll_east_a = ll_east - 500000; 
	ll_north_a = ll_north;

	if(u_band< 'N')	
		ll_north_a = ll_north - 10000000;	

	ll_s         = ll_north_a / utm_k0;	
	ll_plat_a    = ll_s / u_cc_a;	
	cc_ba        = u_cc_b / u_cc_a;	
	cc_ca        = u_cc_c / u_cc_a;	
	ll_plat_b    = ll_plat_a + cc_ba * sin (2 * ll_plat_a) - 
						cc_ca * sin (4 * ll_plat_a);	
	ll_plat0     = ll_plat_a + cc_ba * sin (2 * ll_plat_b) - 
						cc_ca * sin (4 * ll_plat_b);	
	ll_v1        = ellip_d_a / sqrt (1 - ellip_e * ellip_e * 
						sin (ll_plat0) * sin (ll_plat0));
	ll_plat0_2   = tan (ll_plat0) / 2;	
	ll_plat0_24  = tan (ll_plat0) / 24;	
	ll_plat1     = ll_plat0 - (ll_east_a / (utm_k0 * ll_v1)) * 
						(ll_east_a / (utm_k0 * ll_v1)) *
						ll_plat0_2 * (1.0 + ellip_e2s *
						cos (ll_plat0) * cos (ll_plat0));	

	ll_v2        = ellip_d_a / sqrt (1 - ellip_e * ellip_e * 
						sin (ll_plat1) * sin (ll_plat1));

	ll_plat2a    = 5 + 3 * tan (ll_plat1) * tan (ll_plat1) +
						6 * ellip_e2s * cos (2 * ll_plat1);	

	ll_plat2bb   = (ll_east_a / (utm_k0 * ll_v2)) * 
						(ll_east_a / (utm_k0 * ll_v2));

	ll_plat2b    = ll_plat2bb * ll_plat2bb * ll_plat0_24 *
						ll_plat2a;	

	ll_plat2c    = (1 + ellip_e2s * cos (ll_plat1) * cos (ll_plat1)) *
						ll_plat2bb * ll_plat0_2;	

	ll_plat2     = ll_plat0 - ll_plat2c + ll_plat2b;	

	ll_v3        = ellip_d_a / sqrt (1 - ellip_e * ellip_e * 
						sin (ll_plat2) * sin (ll_plat2));

	ll_plat3a    = 5 + 3 * tan (ll_plat2) * tan (ll_plat2) + 
						6 * ellip_e2s * cos (2 * ll_plat2);	

	ll_plat3bb   = (ll_east_a / (utm_k0 * ll_v3)) * 
						(ll_east_a / (utm_k0 * ll_v3));	

	ll_plat3b    = ll_plat3bb * ll_plat3bb * ll_plat0_24 * ll_plat3a;	

	ll_plat3c    = (1 + ellip_e2s * cos(ll_plat2) * cos(ll_plat2)) *
						ll_plat3bb * ll_plat0_2;
            
									/* point latitude (radians)          */
	ll_plat3_rad = ll_plat0 - ll_plat3c + ll_plat3b;		

	*lat_rad     = ll_plat3_rad;
									/* longitude centeral meridian value */
	ll_long_cm   = 6*u_zone - 183;	

	ll_dlonga    = 1 + 2 * tan (ll_plat3_rad) * tan (ll_plat3_rad) +
						ellip_e2s * cos (ll_plat3_rad) * cos (ll_plat3_rad);	

	ll_dlongb    = (ll_east_a / (utm_k0 * ll_v3)) * 
						(ll_east_a / (utm_k0 * ll_v3)) *
						(ll_east_a / (utm_k0 * ll_v3)) * 
						1.0 / (6.0 * cos(ll_plat0)) * ll_dlonga;	

									/* delta longitude (radians)         */
	ll_dlong_rad = ll_east_a / (utm_k0 * ll_v3 * cos (ll_plat0)) - ll_dlongb; 
	*lng_rad = ll_long_cm * PI/180.0 + ll_dlong_rad; 
}
				
/*****************************************************************************\
------------------------->   X Y TO DEGREESS LAT LONG <------------------------
\*****************************************************************************/
void CCoord::x_y_to_degrees_lat_long (double x_coordinate, double y_coordinate, double *lat_p, double *long_p)
{    
	double radians_latitude;
	double radians_longitude;

	x_y_to_radians_lat_long (x_coordinate,
							y_coordinate,
							&radians_latitude,
							&radians_longitude);
         
	*lat_p = radians_latitude * RADIANS_TO_DEGREES;
	*long_p = radians_longitude * RADIANS_TO_DEGREES;                                              
}

/*****************************************************************************\
---------------------->   X Y TO DEGREES LAT LONG STR  <-----------------------
\*****************************************************************************/
void CCoord::x_y_to_degrees_lat_long_str (double x_coordinate, double y_coordinate, char   *lat_str, char   *lng_str)
{   
	double radians_latitude;
	double radians_longitude;
	double lat_p;
	double lng_p;
	double minutes;
	char   lat_chr;
	char   lng_chr;
	int    lat_p_deg;
	int    lat_p_min;
	int    lat_p_sec;
	int    lng_p_deg;
	int    lng_p_min;
	int    lng_p_sec;

	x_y_to_radians_lat_long(x_coordinate,
							y_coordinate,
							&radians_latitude,
							&radians_longitude);
         
	lat_p = radians_latitude * RADIANS_TO_DEGREES;
	lng_p = radians_longitude * RADIANS_TO_DEGREES;

	lat_chr = 'N';
	if (lat_p < 0)
		lat_chr = 'S';
   
	lng_chr = 'E';
	if (lng_p < 0)
		lng_chr = 'W';

	lat_p = fabs(lat_p);
	lat_p_deg = (int) lat_p;
	minutes = (lat_p - (double) lat_p_deg) * 60;
	lat_p_min = (int) minutes;
	lat_p_sec = int(((minutes - (double) lat_p_min) * 60.0) + 0.5);
	if (lat_p_sec == 60) {
		lat_p_sec = 0;
		lat_p_min = lat_p_min + 1;
	}
	if (lat_p_min == 60) {
		lat_p_min = 0;
		lat_p_deg = lat_p_deg + 1;
	}

	lng_p = fabs(lng_p);
	lng_p_deg = (int) lng_p;
	minutes = (lng_p - (double) lng_p_deg) * 60;
	lng_p_min = (int) minutes;
	lng_p_sec = int(((minutes - (double) lng_p_min) * 60.0) + 0.5);
	if (lng_p_sec == 60) {
		lng_p_sec = 0;
		lng_p_min = lng_p_min + 1;
	}
	if (lng_p_min == 60) {
		lng_p_min = 0;
		lng_p_deg = lng_p_deg + 1;
	}

	sprintf(strlat,"%02d%02d%02d%c",lat_p_deg,lat_p_min,lat_p_sec,lat_chr);
	sprintf(strlng,"%03d%02d%02d%c",lng_p_deg,lng_p_min,lng_p_sec,lng_chr); 
	strcpy(lat_str,strlat);
	strcpy(lng_str,strlng);
}

/*****************************************************************************\
---------------------------->   X Y TO GEOREF  <-------------------------------
\*****************************************************************************/
void CCoord::x_y_to_georef_str (double x_coordinate, double y_coordinate, char   *georef_string)
{
	double geographic_latitude;
	double geographic_longitude;

	x_y_to_radians_lat_long (x_coordinate, 
							y_coordinate,
		     				&geographic_latitude, 
							&geographic_longitude);

	radians_lat_long_to_georef (geographic_latitude,
								geographic_longitude, 
								georef_string); 

}  

/*****************************************************************************\
-------------------------->   X Y TO RADIANS LAT_LONG <------------------------
\*****************************************************************************/
void CCoord::x_y_to_radians_lat_long (double x_coordinate, double y_coordinate, double *geographic_latitude, double *geographic_longitude)
{
	/* implementation notes:                                                 */
	/*                                                                       */
	/*   the formulas implemented by this procedure are as follows:          */
	/*                                                                       */
	/*                                                                       */
	/*                                       4 * eo * x                      */
	/*   ws = wo + arctan (------------------------------------------------) */
	/*                           2   2   2                                   */
	/*                     ( 4*eo - x - y ) * cos(co) - 4 * eo * y *sin(co)  */
	/*                                                                       */
	/*                                                                       */
	/*                                                                       */
	/*                                                  2   2   2            */
	/*                     4 * eo * y * cos(co) + ( 4*eo - x - y ) * sin(co) */
	/*   cs =      asin (------------------------------------------------)   */
	/*                                          2   2   2                    */
	/*                                    ( 4*eo + x + y )                   */
	/*                                                                       */
	/*                                                                       */
	/*  where                                                                */
	/*                                                                       */
	/*    ws  =  longitude for (x,y)                                         */
	/*    cs  =  conformal latitude for (x,y)                                */
	/*    wo  =  reference longitude                                         */
	/*    eo  =  conformal radius                                            */
	/*    co  =  reference site conformal latitude                           */


	double numerator;
	double latitude_factor;
	double general_factor;
	double real_32_x_2;
	double real_32_y_2;
	double conformal_latitude;

  //래스터지도를  전시하기위한 임시
	x_coordinate -= 92.63483995; 
	y_coordinate += 0.94031217;
	//임시 끝

	real_32_x_2 =  x_coordinate * x_coordinate;

	real_32_y_2 =  y_coordinate * y_coordinate;

	latitude_factor = four_e_sub_0_sqr + real_32_x_2 + real_32_y_2;

	general_factor = four_e_sub_0_sqr - real_32_x_2 - real_32_y_2;

	numerator = four_e_sub_0_x_cos_lat * y_coordinate +
				general_factor * pts_sin_ref_lat;

	if (latitude_factor < epsilon) 
		conformal_latitude = 0.0;
	else
		conformal_latitude = asin (numerator / latitude_factor);
		
	*geographic_longitude = pts_sector_long + 
				atan2 (four_e_sub_0 * x_coordinate,
				general_factor * pts_cos_ref_lat - 
				four_e_sub_0_x_sin_lat * y_coordinate);

	radians_conformal_lat_to_radians_lat (conformal_latitude,
										geographic_latitude);           

}

/*****************************************************************************\
---------------------------->  X Y TO UTM STR <--------------------------------
\*****************************************************************************/							
void CCoord::x_y_to_utm_str (double x_pos, double y_pos, char utm_string[])
{
	double lat;
	double lng;

	x_y_to_radians_lat_long (x_pos, y_pos, &lat, &lng);
	radians_lat_long_to_utm_str (lat, lng, utm_string);
}

/*****************************************************************************\
---------------------------->  X Y TO UTM STR <--------------------------------
\*****************************************************************************/
void CCoord::x_y_to__other_utm_str(double x_pos, double y_pos, char utm_string[])
{
	double lat;
	double lng;

	x_y_to_radians_lat_long(x_pos, y_pos, &lat, &lng);
	radians_lat_long_to_other_utm(lat, lng, utm_string);
}

/*****************************************************************************\
------------------------------>  GUI STEREO FOR <------------------------------
\*****************************************************************************/	
int CCoord::gui_stereo_for (void   *init_data, double lon, double lat, double *x, double *y) 
{
	radians_lat_long_to_x_y(lat,lon,x,y);
	*x = *x * 1852.0;
	*y = *y * 1852.0; 
	return(1);
}

/*****************************************************************************\
------------------------------>  GUI STEREO INV <------------------------------
\*****************************************************************************/	
int CCoord::gui_stereo_inv (void   *init_data, double x, double y, double *lon, double *lat) 
{
	x = x / 1852.0;
	y = y / 1852.0;
	x_y_to_radians_lat_long(x,y,lat,lon);
	return(1);
}

/*****************************************************************************\
------------------------------>  GUI STEREO INIT <-----------------------------
\*****************************************************************************/	
int CCoord::gui_stereo_init(double center_long, double center_lat, double max_angle, double height, double radius, double r_major, double r_minor, void   **init_data)
{                     
	return(1);
}

/*****************************************************************************\
----------------------------->  GUI STEREO CLEAR <-----------------------------
\*****************************************************************************/	
int CCoord::gui_stereo_clear(double center_long, double center_lat, double max_angle, double height, double radius, double r_major, double r_minor, void   **init_data)
{                     
	return(1);
}

/*****************************************************************************\
----------------------------->  GUI STEREO DUMMY <-----------------------------
\*****************************************************************************/	
int CCoord::gui_stereo_dummy()
{                     
	return(1);
}


void CCoord::DegreeToDmsLat(double geoPoint, int *Do, int *Min, double *Sec)
{
	*Do = (int)geoPoint;							//도
	*Min = (int)((geoPoint - *Do) * 60);			//분
	*Sec = (((geoPoint - *Do) * 60) - *Min) * 60;	//초
}


void CCoord::DegreeToDmsLon(double geoPoint, int *Do, int *Min, double *Sec)
{
	*Do = (int)geoPoint;							//도
	*Min = (int)((geoPoint - *Do) * 60);			//분
	*Sec = (((geoPoint - *Do) * 60) - *Min) * 60;	//초
}


void CCoord::degree_lat_lon_to_gars(double latitude, double longitude, int precision, char gars_coord[])
{
	char string_temp[9] = "";
	char strLatBand[3] = "";
	char strLongBand[4] = "";
	char temp[3] = "";

	//char tmp[2] = { 0, };

	/* North pole is an exception, read over and down */
	if (latitude == 90.0) {
		latitude = 89.99999999999;
	}
	// Check for valid lat/lon range
	if (latitude < -90 || latitude > 90) 
	{
		return;
	}
	if (longitude < -180 || longitude > 180) 
	{
		return;
	}
	// Get the longitude band ==============================================
	double longBand = longitude + 180;

	// Normalize to 0.0 <= longBand < 360
	while (longBand < 0) {
		longBand = longBand + 360;
	}
	while (longBand > 360) {
		longBand = longBand - 360;
	}
	longBand = floor(longBand * 2.0);
	int intLongBand = (int)longBand + 1; // Start at 001, not 000

	_itoa(intLongBand, strLongBand, 10);

	// Left pad the string with 0's so X becomes 00X
	while (sizeof(strLongBand) < 3) 
	{
		strLongBand[0] = '0';
		//strLongBand = "0" + strLongBand;
	}

	// Get the latitude band ===============================================
	char *letterArray = "ABCDEFGHJKLMNPQRSTUVWXYZ";
	double offset = latitude + 90;
	// Normalize offset to 0< offset <90
	while (offset < 0) {
		offset = offset + 180;
	}
	while (offset > 180) {
		offset = offset - 180;
	}
	offset = (double)floor(offset * 2.0);
	//int s = sizeof(letters);
	double firstOffest = (int)offset / 24;
	double secondOffest = (int)offset % 24;

	sprintf(temp, "%c%c", letterArray[(int)firstOffest], letterArray[(int)secondOffest]);
	strcpy(strLatBand, temp);

	// Id the precision is 30x30min then return the longitudinal and latitudinal bands
	if (precision == 30)
	{
		sprintf(string_temp, "%s%s", strLongBand, strLatBand);
		strcpy(gars_coord, string_temp);
	}

	// Get the quadrant ====================================================
	double latBand = (int)floor((latitude + 90.0) * 4.0) % (int)2.0;
	longBand = (int)floor((longitude + 180.0) * 4.0) % (int)2.0;
	char quadrant = '0';

	// return "0" if error occurs
	if (latBand < 0 || latBand > 1) {
		return ;
	}
	if (longBand < 0 || longBand > 1) {
		return ;
	}
	// Otherwise get the quadrant
	if (latBand == 0.0 && longBand == 0.0) {
		quadrant = '3';
	}
	else if (latBand == 1.0 && longBand == 0.0) {
		quadrant = '1';
	}
	else if (latBand == 1.0 && longBand == 1.0) {
		quadrant = '2';
	}
	else if (latBand == 0.0 && longBand == 1.0) {
		quadrant = '4';
	}

	// Id the precision is 15x15min then return the longitudinal and latitudinal bands
	// plus the quadrant
	if (precision == 15)
	{
		sprintf(string_temp, "%s%s%c", strLongBand, strLatBand, quadrant);
		strcpy(gars_coord, string_temp);
		//return strLongBand + strLatBand + quadrant;
	}

	// Get the keypad ======================================================
	/* Convert longitude and latitude from degrees to minutes */
	/* longitude assumed in -180 <= long < +180 range */
	double long_minutes = (longitude + 180) * 60.0;
	double lat_minutes = (latitude + 90) * 60.0;
	/* now we have a positive number of minutes */

	/* Find 30-min cell indices 0-719 and 0-359 */
	long horiz_index_30 = (long)(long_minutes / 30.0);
	long vert_index_30 = (long)(lat_minutes / 30.0);

	/* Compute remainders 0 <= x < 30.0 */
	double long_remainder = long_minutes - (horiz_index_30) * 30.0;
	double lat_remainder = lat_minutes - (vert_index_30) * 30.0;

	/* Find 15-min cell indices 0 or 1 */
	long horiz_index_15 = (long)(long_remainder / 15.0);
	long vert_index_15 = (long)(lat_remainder / 15.0);

	/* Compute remainders 0 <= x < 15.0 */
	long_remainder = long_remainder - (horiz_index_15) * 15.0;
	lat_remainder = lat_remainder - (vert_index_15) * 15.0;

	/* Find 5-min cell indices 0, 1, or 2 */
	long horiz_index_5 = (long)(long_remainder / 5.0);
	long vert_index_5 = (long)(lat_remainder / 5.0);

	char _5_minute_array[3][3] = { {'7', '4', '1'}, {'8', '5', '2'}, {'9', '6', '3'} };

	char keypad = _5_minute_array[(int)horiz_index_5][(int)vert_index_5];

	sprintf(string_temp, "%s%s%c%c", strLongBand, strLatBand, quadrant, keypad);
	strcpy(gars_coord, string_temp);
}
/*****************************************************************************\
------------------------->  E N D  O F  F I L E  <-----------------------------
\*****************************************************************************/



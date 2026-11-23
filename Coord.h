// Coord.h: interface for the CCoord class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_COORD_H__8168831B_FDA2_453C_9647_144A3878A39A__INCLUDED_)
//#define AFX_COORD_H__8168831B_FDA2_453C_9647_144A3878A39A__INCLUDED_

#pragma once

#include "GeneralDefn4Math.h"

#undef	AFX_DATA
#ifdef	GEODC_IMPL
#define	AFX_DATA	_declspec(dllexport)
#else
#define	AFX_DATA	_declspec(dllimport)
#endif


 
#define WGS_84						0
#define DEGREES_PER_MAJOR_GRID		15
#define MAJOR_GRID_TO_MIN			(15 * 60)
#define MINOR_GRID_TO_MIN			60

#define LAT_DEGREE					37
#define LAT_MIN						5
#define LAT_SEC						10
#define LAT_CHAR					'N'
#define LNG_DEGREE					129
#define LNG_MIN						00
#define LNG_SEC						00
#define LNG_CHAR					'E'


class AFX_DATA CCoord
{
public: // Member Functions
	int    earth_model;
	double a_coefficient;
	double b_coefficient;
	double c_coefficient;
	double d_coefficient;
	double e_coefficient;
	double f_coefficient;
	double g_coefficient;
	double h_coefficient;
	double earth_eccentricity;
	double polar_earth_radius;
	double conformal_earth_radius;
	double scc_conformal_lat_in_radians;

	double epsilon;
	double effective_earth_radius;
	double equatorial_radius_nm;
	double four_e_sub_0;
	double four_e_sub_0_sqr;
	double four_e_sub_0_x_cos_lat;
	double four_e_sub_0_x_sin_lat;
	double pts_conformal_earth_radius;
	double pts_sector_conformal_lat;
	double pts_sector_lat;
	double pts_sector_long;  
	double pts_earth_rad_denominator;
	double pts_sin_ref_lat;
	double pts_cos_ref_lat;
	double two_e_sub_0;
	double ellip_d_b;
	double ellip_d_f;
	double ellip_d_a;
	double ellip_e;
	double ellip_n;
	double ellip_r;
	double ellip_e2s;
	double u_cc_a;
	double u_cc_b;
	double u_cc_c;
	double utm_k0;
	double utm_rc;

	enum direction_type
	{
	   NORTH,
	   SOUTH,
	   EAST,
	   WEST
	};

	enum earth_model_type
	{
	   hayford_1910,
	   wgs_72,
	   wgs_84,
	   bessel_1841 
	}; 

	typedef struct {
		short degree;
		short minute;
		short second;
		char  direct;
	} LAT_LONG_TYPE;

	double array_a[9];
	double array_b[9];
	double array_f[9];
	char alpha_number[10];
	char georef_label[26];
	int m_num[26];
	char	strlat[20];
	char	strlng[20];
	

	CCoord();
	virtual ~CCoord();
	/**************** LOCAL Function Prototype *****************************/

		/* Purpose:
	 *   This procedure computes the coefficients based on the specified
	 *   eccentricity.
	 *
	 * Parameters:
	 *   None
	 *
	 * Values Returned:
	 *   None
	 */
	void compute_ellipsoid_constants ();

	/* Purpose:
	 *   This procedure computes the UTM/MGR latitude band.
	 *
	 * Parameters:
	 *   double gb_lat
	 *
	 * Values Returned:
	 *   char   compute_utm_band
	 */
	char compute_utm_band(double gb_lat);
	
	/* Purpose:
		*   This procedure computes the UTM/MGR longitude zone.
		*
		* Parameters:
		*   double gz_long
		*   char   band
		*
		* Values Returned:
		*   int    compute_utm_zone
	*/
	int compute_utm_zone (double gz_long, char band);
	
	/* Purpose:
		 *   This procedure converts decimal latitude degrees to degrees,
		 *   minutes, and seconds.
		 *
		 * Parameters:
		 *   double declat
		 *
		 * Values Returned:
		 *   char   *string
	*/
	void declat_dms(double declat, char *string);

	/* Purpose:
		 *   This procedure converts decimal lolgitude degrees to degrees,
		 *   minutes, and seconds.
		 *
		 * Parameters:
		 *   double declon
		 *
		 * Values Returned:
		 *   char   *string
	*/
	void declon_dms(double declon, char *string);

	/* Purpose:
		 *   This procedure computes the delta longitude fronm the central meridian.
		 *
		 * Parameters:
		 *   double lng
		 *   int    zone
		 *
		 * Values Returned:
		 *   double d_long
	*/
	double d_long (double lng, int zone);

	/* Purpose:
	 *   This procedure computes the MGR 100,000 meter square letter designation.
	 *
	 * Parameters:
	 *   int    m_nsd     square desig, range 1-24
	 *
	 * Values Returned:
	 *   char   m_alpha  range A-Z except I and O
	 */
	char m_alpha(int m_nsd);
 
	/* Purpose:
	 *   This procedure computes the UTM/MGR longitude zone.
	 *
	 * Parameters:
	 *   char   m_ltr
	 *
	 * Values Returned:
	 *   int    m_number
	 */
	int m_number (char m_ltr);
    
	/* Purpose:
	 *   This procedure computes the MGR adjusted Easting.
	 *
	 * Parameters:
	 *   int    nsd1
	 *   int    m_zone_est
	 *
	 * Values Returned:
	 *   int    mnsd1_a
	 */
	int mnsd1_a(int nsd1, int m_zone_est);
 
	/* Purpose:
	 *   This procedure computes the MGR adjusted Northing.
	 *
	 * Parameters:
	 *   int    nsd1
	 *   int    m_zone_nor
	 *
	 * Values Returned:
	 *   int    mnsd2_a
	 */
	int mnsd2_a(int nsd2, int m_zone_nor);

	/* Purpose:
	 *   This procedure computes the UTM/MGR r(c) value.
	 *
	 * Parameters:
	 *   double rc_k0rc_k0	scale factor constant
	 *   double rc_a	equatorial radius of earth (a)
	 *   double rc_e        eccentricity of earth (e)
	 *   double rc_lat      point latitude
	 *
	 * Values Returned:
	 *   double rc
	 */
	double rc(double rc_k0, double rc_a, double rc_e, double rc_lat);	
 
	/* Purpose:
	 *   This procedure computes the UTM Easting using the four
	 *   supplied parameters.
	 *
	 * Parameters:
	 *   double ue_rc
	 *   double ue_e1
	 *   double ue_dlong
	 *   double ue_lat
	 *
	 * Values Returned:
	 *   double u_east
	 */
	double u_east(double ue_rc, double ue_e1, double ue_dlong, double ue_lat);
 
	/* Purpose:
	 *   This procedure computes the UTM Easting using the three
	 *   supplied parameters.
	 *
	 * Parameters:
	 *   int    u_zone_e
	 *   int    m_east
	 *   int    m_nsd_e
	 *
	 * Values Returned:
	 *   double u_m_east
	 */
	double u_m_east(int u_zone_e, int m_east, int m_nsd_e);
   
	/* Purpose:
	 *   This procedure computes the UTM Northing using the five
	 *   supplied parameters.
	 *
	 * Parameters:
	 *   double un_rc
	 *   double un_r
	 *   double un_n
	 *   double un_dialog
	 *   double un_lat
	 *   double un_band
	 *
	 * Values Returned:
	 *   double u_north
	 */
	double u_north(double un_rc, double un_r, double un_n, double un_dlong, double un_lat, char un_band);

	/* Purpose:
	 *   This procedure computes the UTM Northing using the four
	 *   supplied parameters.
	 *
	 * Parameters:
	 *   int    eq_rad
	 *   int    u_zone_n
	 *   int    m_north
	 *   int    m_nsd_n
	 *   int    u_lat_a
	 *
	 * Values Returned:
	 *   double u_m_north
	 */
	double u_m_north(int eq_rad, int u_zone_n, int m_north, int m_nsd_n, int u_lat_a);


	/*****************************************************************************
	 ***                                                                       ***
	 ***                          EXPORTED PROTOTYPES                          ***
	 ***                                                                       ***
	 *****************************************************************************/

	/* Purpose:
		*   This procedure computes the coefficients based on the specified
		*   eccentricity.
		*
		* Parameters:
		*   double earth_eccentricity
		*
		* Values Returned:
		*   double *a_con
		*   double *b_con
		*   double *c_con
		*   double *d_con
		*   double *e_con
		*   double *f_con
		*   double *g_con
		*   double *h_con
	*/
	void compute_coefficients (double earth_eccentricity,
							   double *a_con,	     
							   double *b_con,
							   double *c_con,
							   double *d_con,
							   double *e_con,
							   double *f_con,
							   double *g_con,
							   double *h_con);

	/* Purpose:
		*   This procedure computes the factors for a particular earth model
		*   based upon the specified equitorial radius and flatness.
		*
		* Parameters:
		*   double equatorial_radius
		*   double flatness
		*
		* Values Returned:
		*   double *earth_eccentricity
		*   double *polar_earth_radius
		*   double *a_con
		*   double *b_con
		*   double *c_con
		*   double *d_con
		*   double *e_con
		*   double *f_con
		*   double *g_con
		*   double *h_con
	*/
	void compute_earth_model_data (double equatorial_radius,
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
									 double *h_con);

	/* Purpose:
		*   This function computes the earth eccentricity.
		*
		* Parameters:
		*   double flatness
		*
		* Values Returned:
		*   double eccentricity
	*/
	double compute_eccentricity (double flatness);

	/* Purpose:
		* This function computes the polar earth radius.
		*
		* Parameters:
		*   double equatorial_radius
		*   double flatness
		*
		* Values Returned:
		*   double polar_earth_radius
	*/
	double compute_polar_earth_radius  (double equatorial_radius, double flatness);
 
	/* Purpose:
		*   This procedure converts lat/long to conformal stereographic (x,y).
		*
		* Parameters:
		*   double geographic_latitude
		*   double geographic_longitude
		*
		* Values Returned:
		*   double *x_coordinate
		*   double *y_coordinate
	*/
	void degrees_lat_long_to_x_y (double geographic_latitude, double geographic_longitude, double *x_coordinate, double *y_coordinate);

	/* Purpose:
		*   This function will convert a degrees minutes and seconds (dms)
		*   character string to a double precision value.
		*
		* Parameters:
		*   char *dms      :     dms character string
		*
		* Values Returned:
		*   deg            :     decimal latitute
		*
	*/
	void dms_to_dec_deg (char *dms, double *deg);

	/* Purpose:
		* This procedure translates georef to geographic latitude and longitude in
		* degrees, minutes and seconds.
		*
		* Parameters:
		*   char *georef_point
		*
		* Values Returned:
		*   double *lat_p
		*   double *long_p
	*/
	void georef_to_degrees_lat_long (char *georef_point, double *lat_p, double *long_p);                              			    

	/* Purpose:
		* This procedure translates georef to geographic latitude and longitude in
		* radians.
		*
		* Parameters:
		*   char *georef_point
		*
		* Values Returned:
		*   double *geographic_latitude
		*   double *geographic_longitude
	*/
	void georef_to_radians_lat_long (char   *georef_point, double *geographic_latitude, double *geographic_longitude); 

	/* Purpose:
		* This procedure translates georef to (x,y) conformal stereographic.
		*
		* Parameters:
		*   char *georef_point
		*
		* Values Returned:
		*   double *x_coordinate
		*   double *y_coordinate
	*/
	void georef_to_x_y (char   *georef_point, double *x_coordinate, double *y_coordinate); 	

	/* Purpose:
		*   This procedure initializes the adaptation data.
		*
		* Parameters:
		*   None
		*
		* Values Returned:
		*   None
	*/
	void initialize_adaptation_data();

	/* Purpose:
		*   This procedure initializes the coordinate conversion adaptable parameters.
		*
		* Parameters:
		*   None
		*
		* Values Returned:
		*   None
	*/
	void initialize_coord_conversions();
   
	/* Purpose:
		* This procedure is used to initialize geographical adaptable data and
		* local constants used frequently in the conversion procedures.
		*
		* Parameters:
		*   double conformal_earth_radius
		*   double scc_longitude
		*   double scc_conformal_latitude
		*
		* Values Returned:
		*   None
	*/
	void initialize_geo_data (double conformal_earth_radius_in, double scc_longitude,  double scc_conformal_latitude); 

	/* Purpose:
		*   This procedure converts MGR to latitude and longitude in radians.
		*
		* Parameters:
		*   char   *unpadded_utm_string      example: "10TDL362830"
		*          (The above UTM represents approximately 41 deg N, 124 deg W)
		*
		* Values Returned:
		*   double *lat_rad
		*   double *lng_rad
	*/
	void utm_to_radians_lat_long (char   *unpadded_utm_string, double *lat_rad, double *lng_rad);

	/* Purpose:
		* This procedure converts conformal latitude in radians to geographic
		* latitude in radians.
		*
		* Parameters:
		*   double conformal_latitude
		*
		* Values Returned:
		*   double *geographic_latitude
	*/
	void radians_conformal_lat_to_radians_lat (double conformal_latitude, double *geographic_latitude);
  
	/* Purpose:
		* This procedure converts geographic latitude in radians to conformal
		* latitude in degrees.
		*
		* Parameters:
		*   double geographic_latitude
		*
		* Values Returned:
		*   double *concormal_latitude
	*/
	void radians_latitude_to_degrees_conformal_latitude (double geographic_latitude, double *conformal_latitude);

	 /* Purpose:
	  * This procedure converts geographic latitude in radians to conformal
	  * latitude in radians.
	  *
	  * Parameters:
	  *   double geographic_latitude
	  *
	  * Values Returned:
	  *   double *conformal_latitude
	  */
	void radians_latitude_to_radians_conformal_latitude (double geographic_latitude, double *conformal_latitude);

	 /* Purpose:
	  * This procedure converts lat/long in radians to georef.
	  *
	  * Parameters:
	  *   double geographic_latitude
	  *   double geographic_longitude
	  *
	  * Values Returned:
	  *   char *georef_point
	  */
	void radians_lat_long_to_georef (double geographic_latitude, double geographic_longitude, char *georef_point);  
 
	 /* Purpose:
	  *   This procedure converts latitude and longitude in radians to MGR
	  *   coordinates.
	  *
	  * Parameters:
	  *   double lat
	  *   double lng
	  *
	  * Values Returned:
	  *   double *mgr_string
	  */
	void radians_lat_long_to_mgr(double lat, double lng, char *mgr_string_small, char *mgr_string_large);
 
	 /* Purpose:
	  *   This procedure converts latitude and longitude in radians to UTM
	  *   coordinates.
	  *
	  * Parameters:
	  *   double lat
	  *   double lng
	  *
	  * Values Returned:
	  *   double *utm_string
	  */
	void radians_lat_long_to_utm_str (double lat, double lng, char utm_string[]);
 
	 /* Purpose:
	  * This procedure converts geographic lat/long to stereographic (x,y).
	  *
	  * Parameters:
	  *   double geographic_latitude
	  *   double geographic_longitude
	  *
	  * Values Returned:
	  *   double *x_coordinate
	  *   double *y_coordinate
	  */
	void radians_lat_long_to_x_y (double geographic_latitude, double geographic_longitude, double *x_coordinate, double *y_coordinate); 
 
	 /* Purpose:
	  *   This procedure converts UTM to latitude and longitude in radians.
	  *
	  * Parameters:
	  *   char   *utm_string
	  *
	  * Values Returned:
	  *   double *lat_rad
	  *   double *lng_rad
	  */
	void utm_internal_to_radians_lat_long (char   *utm_string, double *lat_rad, double *lng_rad);
 	 		
	 /* Purpose:
	  * This procedure translates stereo (x,y) to geographic latitude and longitude.
	  *
	  * Parameters:
	  *   double x_coordinate
	  *   double y_coordinate
	  *
	  * Values Returned:
	  *   double *lat_p
	  *   double *long_p
	  */
	void x_y_to_degrees_lat_long (double x_coordinate, double y_coordinate, double *lat_p, double *long_p);
 
	 /* Purpose:
	  * This procedure translates stereo (x,y) to geographic latitude and longitude.
	  *
	  * Parameters:
	  *   double x_coordinate
	  *   double y_coordinate
	  *
	  * Values Returned:
	  *   char   *lat_p
	  *   char   *lng_p
	  */
	void x_y_to_degrees_lat_long_str (double x_coordinate, double y_coordinate, char *lat_str, char *lng_str);
 
	 /* Purpose:
	  * This procedure translates stereo (x,y) to georef.
	  *
	  * Parameters:
	  *   double x_coordinate
	  *   double y_coordinate
	  *
	  * Values Returned:
	  *   char *georef_string
	  */
	void x_y_to_georef_str (double x_coordinate, double y_coordinate, char   *georef_string);
   	
	 /* Purpose:
	  * This procedure translates stereo (x,y) to geographic latitude and
	  * longitude.
	  *
	  * Parameters:
	  *   double x_coordinate
	  *   double y_coordinate
	  *
	  * Values Returned:
	  *   double *geographic_latitude
	  *   double *geographic_longitude
	  */
	void x_y_to_radians_lat_long (double x_coordinate, double y_coordinate, double *geographic_latitude, double *geographic_longitude);
 
	 /* Purpose:
	  *   This procedure converts (x,y) conformal stereographic to UTM.
	  *   coordinates.
	  *
	  * Parameters:
	  *   double x_pos
	  *   double y_pos
	  *
	  * Values Returned:
	  *   double *utm_string
	  */
	void x_y_to_utm_str(double x_pos, double y_pos, char utm_string[]); 
 
	/* Purpose:
	 *   This procedure converts (x,y) conformal stereographic to UTM.
	 *   coordinates.
	 *
	 * Parameters:
	 *   double x_pos
	 *   double y_pos
	 *
	 * Values Returned:
	 *   double *utm_string
	*/
	void x_y_to__other_utm_str(double x_pos, double y_pos, char utm_string[]);

	 /* Purpose:
	  * This procedure converts geographic lat/long to stereographic (x,y).
	  *
	  * Parameters:
	  *   double lat : geographic latitude in radians
	  *   double lon : geographic_longitude in radians
	  *
	  * Values Returned:
	  *   double *x  : x coordinate
	  *   double *y  : y coordinate
	  */
	int gui_stereo_for (void * init_data, double lon, double lat, double *x, double *y); 
  
	 /* Purpose:
	  * This procedure converts stereographic (x,y) to geographic lat/long.
	  *
	  * Parameters:
	  *   double x    : x coordinate
	  *   double y    : y coordinate
	  *
	  * Values Returned:
	  *   double *lon : geographic longitude in radians
	  *   double *lat : geographic latitude in radians
	  */
	int gui_stereo_inv (void * init_data, double x, double y, double *lon, double *lat); 
 
	 /* Purpose:
	  * This procedure is the intermaphics initialization callback
	  * for coordinate conversion.
	  *
	  * Parameters:
	  *   double center_long
	  *   double center_lat
	  *   double max_angle
	  *   double height
	  *   double radius
	  *   double r_major
	  *   double r_minor
	  *   void   **init_data
	  *
	  * Values Returned:
	  *   none
	  *
	  */
	int gui_stereo_init(double center_long, double center_lat, double max_angle, double height, double radius, double r_major, double r_minor, void   **init_data);		                     	                				   
 
	 /* Purpose:
	  * This procedure is the intermaphics clear callback
	  * for coordinate conversion.
	  *
	  * Parameters:
	  *   double center_long
	  *   double center_lat
	  *   double max_angle
	  *   double height
	  *   double radius
	  *   double r_major
	  *   double r_minor
	  *   void   **init_data
	  *
	  * Values Returned:
	  *   none
	  *
	  */
	int gui_stereo_clear(double center_long, double center_lat, double max_angle, double height, double radius, double r_major, double r_minor, void   **init_data);		                     	                				   
 
	 /* Purpose:
	  * This procedure is the intermaphics dummy procedure for
	  * coordinate conversion.
	  *
	  * Parameters:
	  *   none
	  *
	  * Values Returned:
	  *   none
	  *
	  */
	int gui_stereo_dummy();


	void radians_lat_long_to_other_utm(double rad_lat, double rad_lng, char utm_string[]);
	void degrees_latitude_to_radians_conformal_latitude (double geographic_latitude, double *conformal_latitude);
	void degree_lat_long_to_x_y (LAT_LONG_TYPE lati, LAT_LONG_TYPE longi, double *x_coord, double *y_coord); 
	
	double GetEarthEccentricity() {return earth_eccentricity;}; 
	double GetPolarEarthRadius() {return polar_earth_radius;}; 
	double GetPtsConformalEarthRadius() {return pts_conformal_earth_radius;}; 
	double GetEquatorialRadiusNM() {return equatorial_radius_nm;}; 

public:
	void DegreeToDmsLat(double geoPoint, int *Do, int *Min, double *Sec);
	void DegreeToDmsLon(double geoPoint, int *Do, int *Min, double *Sec);
	void degree_lat_lon_to_gars(double latitude, double longitude, int precision, char gars_coord[]);


};

#undef	AFX_DATA
#define	AFX_DATA


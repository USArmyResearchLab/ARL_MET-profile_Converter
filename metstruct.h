/** FILE NAME: metstruct.h
________________________________________________________________________
   2017 U.S. Government

   Licensed under the U.S. Army Research Laboratory CC0 1.0 Universal (CC0_1.0) Public Domain Dedication 
   and the Contributor License Agreement (CLA) (together known as the "License"); you may not use this 
   file except in compliance with the License.  You may obtain a copy of the License at 

       https://github.com/USArmyResearchLab.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. 
_________________________________________________________________________

    This include file is used in the primary include file convert.h. 
    metstruct.h contains many of the structure definitions and one or more 
    define statements.  Some structures are complex, but mostly arranged in a 
    logical order. For example, a structure may include an array of structures.
*/

#define MAXSIZE 4000 /* max size of sound and other arrays */


/* info on the site */
struct site_info
{
 char filename[81]; /* name of input file */
 char date[51]; /* year month day */ 
 char time[21]; /* hour min sec */
 char time_id[4]; /* time zone (e.g., GMT or UT) */
 char hgt_id[4];  /* MSL or AGL */
 float lat; /* latitude */
 float lon; /* longitude */
 float elev; /* elevation */
 float prs; /* pressure */
 float ceil; /* ceiling or cloud base */
 float vis; /* visibility */
 float snowrate;  /* not used, but retained for possible future use */
 float rainrate;      /* not used, but retained for possible future use */
 float refract_index;   /* not used, but retained for possible future use */
 float grid_sp;  /* grid spacing of WRF input data */
 float interp_type; /* horizontal interpretation method (0=none, 1=inv dist weight, 2 = bilinear) */
};

/* surface structure */
struct surface_data
{
 struct site_info site; /* information on the site */
 float tmp; /* temperature */
 float hum; /* humidity */
 float spd; /* wind speed */
 float dir; /* wind direction */
 float prs; /* pressure */
};

/* data values for each height for upper level soundings */
struct level_data
{
 float hgt; /* height */
 float prs; /* pressure */
 float tmp; /* temperature */
 float vtmp; /* virtual temperature */
 float hum; /* humidity */
 float dew; /* dewpoint */
 float spd; /* wind speed */
 float dir; /* wind direction */
 float u;   /*u-component */
 float v;   /*v-component */
 float dens; /*density */
 int lvlnum; /* level from 01 to n */
 char htype[4]; /* AGL or MSL (currently use AGL) */
};

/* sounding (normally WRF or raob output) structure */

struct sound
{
 int nht; /* number of pressure levels or heights */
 struct site_info site; /* information on the site */
 struct level_data level[MAXSIZE]; /*data for height or pressure levels (up to MAXSIZE heights or pressure levels)*/
};

/* Other useful structures */

struct value {
   float prs_min;
   float prs_max;
   float hgt_min;
   float hgt_max;
   float tmp_min;
   float tmp_max;
   float hum_min;
   float hum_max;
   float dew_min;
   float dew_max;
   float spd_min;
   float spd_max;
   float dir_min;
   float dir_max;
   float u_min;
   float u_max;
   float v_min;
   float v_max;
};


/* Temporary structure for use of level and layer functions. */

struct temporary          
     {
       float h[MAXSIZE];
       float t[MAXSIZE];
       float tv[MAXSIZE];
       float p[MAXSIZE];
       float hum[MAXSIZE];
       float dew[MAXSIZE];
       float spd[MAXSIZE];
       float dir[MAXSIZE];
       float u[MAXSIZE];
       float v[MAXSIZE];
     };





/** FILENAME:  convert.h
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

   Contains #define, function prototypes, and other include files.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "metstruct.h"


#define ERROR -999.0  /* error or missing data flag for float or double */
#define IERROR -999   /* error or missing data flag for integer */
#define R 287.05     /* gas constant for dry air */
#define G 9.81        /* acceleration due to gravity */
#define DC 348.4      /* constant for density calculations (= 1/R) as defined in various publications */
#define MILDEG 17.777778 /* mils per degree */ 
#define NM 1.94384       /* number of knots (nautical miles /hr) = 1 m/s */
#define MINHEIGHT 0   /* used in check_data and clean_data functions, min height value*/ 
#define MAXHEIGHT 999999.0 /* max height value, used in clean_data function, etc. */
#define MAXMINUTE 99999 /* Max concatinated min and sec for use with 2 or 10 s RAOB data */
#define MAXWINDSPD 221 /* Max wind speed for RAOB; used as a check for missing wind data.*/


   /* Function definitions follow.*/

   int checkdata(struct sound *);
     /* any of the sound data structures     */

   int readraob(struct sound *, char *);
     /* Use for one program version of message generation program. */

   int readwrf(struct sound *, char *);
     /* struct sound for input from WRF output file, input path  */

   int readgfs(struct sound *, char *);
     /* struct sound for input from GFS output file, input path  */

   int readinputdata(struct sound *);
     /* read data from intermediate ("standard" format) input file */

   int writeusrmsg(struct sound *, char *);
     /* struct sound for layer output, path for output     */

   int writeusrlvl(struct sound *, char *);
     /* struct sound for level output, path for output     */

   int msgvalues(struct sound *, struct sound *, struct sound *);
     /* struct sound for ppdf or raob, msg (layer values), mlevel (level values)  */

   int msgvaluesprs(struct sound *, struct sound *, struct sound *);
     /* struct sound for ppdf or raob, msg (layer values), mlevel (level values)  */

   double tvfromtemp(double, double, double);
       /* approx. virtual temp from temp, press, gross humitity    */

   double presscomp(double, double, double, double, double); /*Use with height based program.*/
     /* virt temp, previous virt temp, prev p, z, prev z */

   double tvfromtemptd(double, double, double);  /*Need for NOAA archived radiosonde input.*/
     /* temperature, pressure, dewpoint */           

   int level(float, /*int,*/ int, float *, float *, float *, float *);
      /*  zmax, size, htsize, zh, value, level value, z   */

   int levelprs(float, int, float *, float *, float *, float *);
      /*  pmin, size, plsize, pl, value, level value, p   */

   int layer(float, int, int, float *, float *, float *, float *, float *);
     /*  zmax, size, htsize, zh, value, level value, layer value, z  */

   int layerprs(float, int, int, float *, float *, float *, float *, float *);
     /*  pmin, size, plsize, pl, value, level value, layer value, p  */

   int clean_data(struct sound *, float);

   int initdata(struct sound *);



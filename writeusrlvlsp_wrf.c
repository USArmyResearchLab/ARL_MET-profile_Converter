/** FILE NAME: writeusrlvlsp_wrf.c
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
 
This function prints pressure level based output to appropriate files. The data lines in the output file 
have pressure as the first variable.  Input data are taken from soundings generated from WRF output. 
**/

#include "convert.h"

int writeusrlvl(struct sound *user, char *outpath)
{

   FILE *fuseroutl;
   int i, size;
   float zone;
   char outfile[121], infilename[71];
   
  /* Get output file name. ********************/

    strcpy(outfile, outpath);
    sscanf(user->site.filename, "%29s", infilename);
    /*printf("infilename: %s\n\n", infilename);*/ 
    strcat(outfile, infilename);
    strcat(outfile, "_USRLVL_P");

  /* Open output file.  ************************************/
     
     if(!(fuseroutl = fopen(outfile,"w")))
      {
        fprintf(stderr,"Unable to open user data file.\n");
        exit(1);
      }

   /* Print output to files. ************************************/
  
     size = user->nht; /*** loop indices ***/
     /*printf("\n size = %5d\n\n", size);*/

   /*   USER DEFINED level output ***********************/

    fprintf(fuseroutl,"USER DEFINED level output\n\n");

    fprintf(fuseroutl,"Date: %s   Time: %s  Latitude: %9.4f     Longitude: %9.4f\n", 
                        user->site.date, user->site.time, user->site.lat, user->site.lon);
    fprintf(fuseroutl,"Elevation: %7.2f     Ceiling: %7.1f     Visibility: %7.1f\n\n", 
                        user->site.elev, user->site.ceil, user->site.vis);  

    fprintf(fuseroutl," Line   Pressure    Height   Wind Direction  Wind Speed   Virt Temp Temperature\n");
    fprintf(fuseroutl,"          (hPa)       (m)      (degrees)       (kn)         (K)       (K)\n\n");

    for(i=0;i<size;i++)
      {
         if(user->level[i].spd != ERROR)
           {
             user->level[i].spd *= NM;             /*WRF wind speed in m/s.*/
           }

         fprintf(fuseroutl,"%3d   %8.1f    %7.0f     %7.0f     %8.1f      %8.1f   %8.1f\n",
                 i, user->level[i].prs, user->level[i].hgt, 
                 user->level[i].dir, user->level[i].spd,
                 user->level[i].vtmp, user->level[i].tmp);
      }

    printf("\n User defined message printed to USRLVL_P output.\n");

/**********End of output statements**********************/

   fclose(fuseroutl);

   return;
}

    

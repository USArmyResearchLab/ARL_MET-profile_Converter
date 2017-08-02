/* FILE NAME: readwrf.c. 
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

Reads sounding data derived from WRF output. This program produces input for the convertdatawrf and convertdataprswrf programs that produce output soundings in various tabular formats. The WRF based soundings have heights relative to MSL.   
*/

#include "convert.h"

int readwrf(struct sound *wrf, char *inputfilename)
 {

   FILE *fin;
   char label[81], filler[11];      
   char sitename[121];
   char lat_id[2], lon_id[2];                         
   float hgt_dif;
   float lat_req, lon_req;  
   int i, j, ind, k; 

/* Open input data file.  ******/

    if(!(fin = fopen(inputfilename,"r")))
     {
        fprintf(stderr,"Unable to open wrf input data file.\n");
        exit(1);
     }

/* Read input data.  *************/

 /* Set header values for ceiling and visibility equal to ERROR (-999) to avoid confusion with 
    possible real values (vs. 0.0 for each).  ****/

     wrf->site.ceil = ERROR;
     wrf->site.vis = ERROR;  

 /* Read in header information. *****************/

   fscanf(fin,"%f%f%f", &wrf->site.lat, &wrf->site.lon, &wrf->site.elev);
 
   fscanf(fin, "%s", label);
   sscanf(label, "%10s%1s%8s", wrf->site.date, filler, wrf->site.time);

   fscanf(fin,"%f%f%f%f", &wrf->site.grid_sp, &lat_req, &lon_req, &wrf->site.interp_type);
   
   printf("\ndate, lat, lon, elev, grid_sp, interp_ type= %s %7.3f %7.3f %7.2f  %7.2f %3.0f\n\n", 
        wrf->site.date, wrf->site.lat, wrf->site.lon, wrf->site.elev, wrf->site.grid_sp, wrf->site.interp_type);
   printf("wrf->site.filename = %s\n\n", wrf->site.filename);

  /* End of optional header reading. Common format of data follows. *********/
  /* Read in data lines. ********************************************/

   i=0;

   while(fscanf(fin,"%f%f%f%f%f%f", &wrf->level[i].hgt, &wrf->level[i].prs, &wrf->level[i].tmp, 
          &wrf->level[i].hum, &wrf->level[i].spd, &wrf->level[i].dir) == 6)      
    {      
      i++;
    }
   
  /* Set wrf->nht. ***********/

   wrf->nht = i;

 /* Convert MSL heights to AGL. ****************************/

      if(wrf->level[0].hgt > 0)                   /* Always true so far for WRF profile listings. */
         {                                         /* But just in case ... ***/
            wrf->site.elev = wrf->level[0].hgt;
            strcpy(wrf->site.hgt_id, "MSL");
          }                                    /* Though very unlikely, the surface height could be 0. */ 
      else                                     /* AGL and MSL would be the same, but no effect on calculations. */
         strcpy(wrf->site.hgt_id, "AGL");
          
     if(strcmp(wrf->site.hgt_id, "MSL") == 0)     /* Change MSL to AGL.  Not used for AGL.  *****/
       { 
         if(wrf->level[0].hgt < wrf->site.elev) 
            wrf->site.elev = wrf->level[0].hgt; /* Check for wrf surface vs. elevation values. */
                                                  /* Should not be necessary, but could happen.   */
         if(wrf->level[0].hgt > wrf->site.elev)
           hgt_dif = wrf->level[0].hgt - wrf->site.elev;
         else
           hgt_dif = 0; 

         for(i=0;i<wrf->nht;i++)                  /* This loop for MSL heights only.  */
           {
             wrf->level[i].hgt-= (wrf->site.elev + hgt_dif);
           }
        }

 /* Convert temperature from C to K.  *********************/

   for (i=0;i<wrf->nht;i++)
     {
        wrf->level[i].tmp += 273.16;
     }

/***********************************************/

  fclose(fin);
  
  return;
}


     

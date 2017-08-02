/** FILE NAME: readnoaa.c.
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
 
Reads sounding data in NOAA archive tabular format. This program produces input for the convertdatawyo and convertprsdatawyo programs that produce level and layer profiles in various formats. These WMO soundings have heights relative to MSL. 

Since the NOAA archive format lists mandatory and special levels, the PTH and wind data have to be processed separately though the data are read in together.  The program can read in any number of met data lines.  However, for processing into an output sounding at least two lines are needed, one of which normally is the surface.  The upper limit is the size of the arrays as defined in  metstruct.h. 
*/

#include "convert.h"

int readraob(struct sound *raob, char *inputfilename)
 {

   FILE *fin;
   char label[81], lat[11], lon[11], tp[11][5], wsunit[7];      
   char site_id[6], site[6][15], sitename[121];
   float hgt_dif, actual_ltime;
   int i, j, ind; 
   float temp[4];

/* Open input data file.  ******/

    if(!(fin = fopen(inputfilename,"r")))
     {  
        fprintf(stderr,"\nUnable to open raob input data file.\n\n");
        exit(1);
     }

/* Read input data.  *************/

 /* Set header values for ceiling and visibility equal to ERROR (-999) to avoid confusion with 
    possible real values (vs. 0.0 for each), since raob data do not include these variables. ****/

     raob->site.ceil = ERROR;
     raob->site.vis = ERROR;

 /* Read in header information. ******************************/

       fscanf(fin,"%s", label);
       fscanf(fin, "%s", raob->site.time);
       fscanf(fin, "%s", raob->site.date);
       fscanf(fin,"%s", label);
       strcat(raob->site.date, label);
       fscanf(fin,"%s", label);
       strcat(raob->site.date, label); 

       fscanf(fin,"%s%s%s", label, label, sitename);

    /* Latitude *******/

       i=0;
       fscanf(fin,"%1s", tp[i]); 
       strcpy(lat, tp[i]);

       while((strcmp(tp[i], "N") != 0 && strcmp(tp[i], "S") != 0) && i < 7)
          {
            i++;
            fscanf(fin,"%1s", tp[i]); 
            strcat(lat, tp[i]);
          }
       
       sscanf(lat, "%f", &raob->site.lat);

       if(strcmp(tp[i], "S") == 0)
         {
           raob->site.lat = -raob->site.lat;
         }

    /* Longitude ******/

       i=0;
       fscanf(fin,"%1s", tp[i]); 
       strcpy(lon, tp[i]);

       while((strcmp(tp[i], "W") != 0 && strcmp(tp[i], "E") != 0) && i < 8)
          {
            i++;
            fscanf(fin,"%1s", tp[i]); 
          }
       
       sscanf(lon, "%f", &raob->site.lon);

       if(strcmp(tp[i], "W") == 0)
         {
           raob->site.lon = -raob->site.lon;
         }

    /* End of latitude and longitude ingest section. ****/

       fscanf(fin,"%f%f%", &raob->site.elev, &actual_ltime); /*Retain actual launch time here.*/

       for(i=0;i<8;i++)
         {
            fscanf(fin,"%s", label);
         }
      
       fscanf(fin,"%s", site_id);  
       strcat(sitename, " ");
       strcat(sitename, site_id);
       printf("Sitename is: %s\n\n", sitename);   /*Name of the site.*/

       fscanf(fin,"%s%s", label, wsunit);  /*Wind speed unit: kn or m/s.*/
  

   /*** End of optional header reading. Common format of data follows. *********/
   /*** Read data lines.  Need various checks for various situations of missing data. ******/ 

   /* Read surface data line which is first line even if standard levels have higher pressure.*/

   fscanf(fin, "%d%f%f%f%f%f%f", &raob->level[0].lvlnum, &raob->level[0].prs, &raob->level[0].hgt, &raob->level[0].tmp, &raob->level[0].dew, &raob->level[0].dir, &raob->level[0].spd);

   printf(" raob tmp, dew, dir, spd = %7.0f %7.0f %7.0f %7.0f\n", raob->level[0].tmp, raob->level[0].dew, raob->level[0].dir, raob->level[0].spd);
   printf("\n\n"); 

   i=1;

   while(fscanf(fin,"%d%f%f%f%f%f%f", &raob->level[i].lvlnum, &raob->level[i].prs, &raob->level[i].hgt, &raob->level[i].tmp, &raob->level[i].dew, &raob->level[i].dir, &raob->level[i].spd) == 7)
    {     
 
       if(raob->level[i].prs > raob->level[0].prs)
         {
           printf("\nFound standard level below surface.\n");
           printf("i, raob->level[i].prs = %5d %8.1f\n\n", i, raob->level[i].prs);
           i--;
         }

       i++;
     }

   raob->nht = i; 

 /* Change missing data indicator. Input has missing indicator of 99999. program uses ERROR = -999. ***/

 for(i=0;i<raob->nht;i++)
   {
      if(raob->level[i].spd == 99999)
        {
          raob->level[i].spd = ERROR;
          raob->level[i].dir = ERROR;      
        } 

      if(raob->level[i].tmp == 99999)
        {
          raob->level[i].tmp = ERROR;
          raob->level[i].dew = ERROR;
        }
      else
        {
          raob->level[i].tmp *= 0.1;  /* Need because NOAA T and Td units are *10. */
          raob->level[i].dew *= 0.1;   
        } 

      if(strcmp(wsunit, "kt") != 0)   /* Normally wind speed is in knots.  Applied if in m/s. */ 
        {
          raob->level[i].spd *= NM;
        }

      raob->level[i].prs *= 0.1;  /* Need because NOAA P units are *10. */
      
   }

 /* Convert MSL heights to AGL. ****************************/

      if(raob->level[0].hgt > 0)                   /* Always true so far for NOAA raob listings. */
         {                                         /* But just in case ... ***/
            raob->site.elev = raob->level[0].hgt;
            strcpy(raob->site.hgt_id, "MSL");
          }                                    /* Though very unlikely, the surface height could be 0. */ 
      else                                     /* AGL and MSL would be the same, but no effect on calculations. */       
         strcpy(raob->site.hgt_id, "AGL");         
          
     if(strcmp(raob->site.hgt_id, "MSL") == 0)     /* Change MSL to AGL.  Not used for AGL.  *****/
       { 
         if(raob->level[0].hgt < raob->site.elev) 
            raob->site.elev = raob->level[0].hgt; /* Check for raob surface vs. elevation values. */
                                                  /* Should not be necessary, but could happen.   */
         if(raob->level[0].hgt > raob->site.elev)
           hgt_dif = raob->level[0].hgt - raob->site.elev;
         else
           hgt_dif = 0; 
   
         for(i=0;i<raob->nht;i++)                  /* This loop for MSL heights only.  */
           {
             raob->level[i].hgt-= (raob->site.elev + hgt_dif);
           }
        }

     /*checkdata(raob);*/

/***********************************************/

  fclose(fin);   
  
  return;
}
     

/** FILE NAME: convertdataprswrf.c
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

   This program converts a WRF generated "sounding" into user defined pressure based 
   level and layer vertical profiles.  Note: this program assumes WRF "sounding" 
   has wind values in m/s and degrees, and temperature in C. 
*/

#include "convert.h"

int main(int argnum, char* charname[])
                 /* charname[] is the name of the input file. Copied to "snd->site.filename."*/
{                   
   FILE *fparams;
   FILE *fusrin;
   int i, j, nht; 
   float pmin, max_press;
   char inpath[121], outpath[121], inputfilename[161]; 
   int usrswitch=0, usrswitchlvl=0;

   struct sound *snd;
   struct sound *usrmsg;
   struct sound *usrmsglevel;

   snd = (struct sound *)malloc(sizeof(struct sound));
   usrmsg = (struct sound *)malloc(sizeof(struct sound));
   usrmsglevel = (struct sound *)malloc(sizeof(struct sound));


 /* Initialize structure and other arrays.  ***********************************/

     initdata(snd);
     initdata(usrmsg);
     initdata(usrmsglevel); 
   
  /*  Open parameters file, input file from raob, and open message info files. ***/
 
   printf("\n");

   if(!(fparams = fopen("input_parameters", "r")))
      {
        fprintf(stderr,"Unable to open input_parameters file.\n");
        fprintf(stderr,"Cannot continue. Create file with input and output paths.\n");
        exit(1);
      }

   if(!(fusrin = fopen("usrprs_lvls","r")))  /*Later may have separate lvls files. TBD.*/
      {
        fprintf(stderr,"Unable to open usrprs data file.\n");
        usrswitch++;
        usrswitchlvl++;
      }

    if(usrswitch != 0)
      {
        fprintf(stderr,"No user defined message computed.\n");
      }

    if(usrswitchlvl != 0)
      {
        fprintf(stderr,"No user defined pressure level output computed.\n");
      }

    if(usrswitch != 0 && usrswitchlvl != 0)
      {
        fprintf(stderr,"No user defined profiles computed.  Need pressure structure input file.\n");
        exit(1);
      }

  /* Get pathname, otherwise assume same directory as program. Need pressure structure input file.**/

   fscanf(fparams,"%s%s", inpath, outpath); 
   /*printf("Input path: %s   Output path: %s \n", inpath, outpath);*/
   strcpy(snd->site.filename, charname[1]);
   strcpy(inputfilename, inpath);
   strcat(inputfilename, snd->site.filename);

  /* Read data from input data file. ***************/
 
    readwrf(snd, inputfilename);   
    printf("snd->site.filename = %s\n\n",snd->site.filename);

  /* Check for data that are out of bounds - gross error check. ****/

   clean_data(snd, snd->level[0].hgt);
   printf("snd->nht = %8d\n\n", snd->nht);

   for (i=0;i<snd->nht;i++)
     {
       if(snd->level[i].hgt == ERROR || snd->level[i].tmp == ERROR || snd->level[i].prs
            == ERROR || snd->level[i].hum == ERROR || snd->level[i].spd == ERROR ||
            snd->level[i].dir == ERROR)
         {
           

           printf("Input sounding data error!!!\n");
           printf("i, Pressure, Height = %4d %6.1f %8.0f\n", i, snd->level[i].prs, snd->level[i].hgt);
           printf("Check sounding before re-running if i < snd-nht.\n");
           printf("Exiting program.\n\n");
           exit(0);
           
           if(i == 0)/*Don't use sounding if no or incomplete surface line,*/
             {                        
               printf("Check sounding data before resuming.\n\n");
               exit(0);
             }
          }
      }

  /* Set up message pressure levels. **************************************/

  /*  for USER defined message  ****************************/

    if(usrswitch == 0 || usrswitchlvl == 0) 
      {
        i=0;
        j=0;

        while(fscanf(fusrin,"%f", &usrmsglevel->level[i].prs) == 1 && usrmsglevel->level[i].prs >= snd->level[snd->nht-1].prs)
          {
            /*printf("i, prs = %6d %9.1f\n", i, usrmsglevel->level[i].prs);*/
            i++;
          }
        nht = i; printf("\n");
        usrmsglevel->nht = nht; /*printf("usrmsglevel->nht = %3d\n", usrmsglevel->nht);*/

        if(snd->level[0].prs > usrmsglevel->level[0].prs) /* Where surface P > input maximum P level*/
          {
             for(i=nht;i>0;i--)
               {
                 usrmsglevel->level[i].prs = usrmsglevel->level[i-1].prs;
                 /*printf("i, prs = %6d %9.1f\n", i, usrmsglevel->level[i].prs);*/
               }
             usrmsglevel->level[0].prs = snd->level[0].prs; 
             usrmsglevel->nht = nht+1; 
          }

        if(snd->level[0].prs < usrmsglevel->level[0].prs)  /* Where surface P < or = input maximum P level.*/
          {
            while(usrmsglevel->level[j].prs >= snd->level[0].prs)  
              {
                j++; 
              }
            j--;
            for(i=1;i<nht-j;i++)
              {
                usrmsglevel->level[i].prs = usrmsglevel->level[i+j].prs;
              }
             usrmsglevel->level[0].prs = snd->level[0].prs; 
             usrmsglevel->nht = nht - j; 
           }

        printf("\nNumber of USER DEFINED levels (usrmsglevel->nht) = %4d\n\n", usrmsglevel->nht);
      }

  /* COMPUTATIONS. ##################################################*/
  
  /* Compute values for the selected type of output "sounding".  
     The function "msgvaluesprs" performs the main work of the program, and 
     eliminates fields that are missing or out of bounds (identified 
     by ERROR/IERROR). ****/

   if(usrswitch == 0 || usrswitchlvl == 0)
     msgvaluesprs(snd, usrmsg, usrmsglevel);

   /*usrmsg->nht +=1; checkdata(usrmsg);*/

   /* OUTPUT SECTION ##############################################*/

   if(usrswitch == 0)
     {
       writeusrmsg(usrmsg, outpath);
     }

    if(usrswitchlvl == 0)
     {
       writeusrlvl(usrmsglevel, outpath);
     }

   printf("\n");

   free(snd);
   free(usrmsg);
   free(usrmsglevel); 

   fclose(fusrin);
   fclose(fparams);

}  /* End of convert data program for WRF pressure based profiles. &&&&&&&&&&&&&&&&&&&&&&*/

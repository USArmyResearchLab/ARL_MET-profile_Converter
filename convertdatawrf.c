/** FILE NAME: convertdatawrf.c
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

   This program converts a WRF generated "sounding" into user defined level and 
   layer vertical profiles.  It includes a second user defined height structure for layer and level 
   output as a placeholder (commented out).  Note: this program assumes WRF "sounding" 
   has wind values in m/s and degrees, and temperature in C. 
*/

#include "convert.h"

int main(int argnum, char* charname[])
                 /* charname[] is the name of the input file. Copies to "snd->site.filename."*/
{                   
   FILE *fparams;
   FILE *fdata;
   FILE *fusrin;
   /*FILE *fusr2in;*/
   int i, j=0, k=0; 
   float zmax;
   char inpath[121], outpath[121], inputfilename[161]; 
   int usrswitch=0;
   /*int usr2switch=0;*/

   struct sound *snd;
   struct sound *usrmsg;
   struct sound *usrmsglevel;
   /*struct sound *usr2msg;
   struct sound *usr2msglevel;*/

   snd = (struct sound *)malloc(sizeof(struct sound));
   usrmsg = (struct sound *)malloc(sizeof(struct sound));
   usrmsglevel = (struct sound *)malloc(sizeof(struct sound));
/* usr2msg = (struct sound *)malloc(sizeof(struct sound));
   usr2msglevel = (struct sound *)malloc(sizeof(struct sound));*/

 /* Initialize structure and other arrays.  ***********************************/

     initdata(usrmsg);
     initdata(usrmsglevel); 
     /*initdata(usr2msg);
     initdata(usr2msglevel);*/ 
   
  /*  Open parameters file, input file from raob, and open message info files. ***/
 
   printf("\n");

   if(!(fparams = fopen("input_parameters", "r")))
      {
        fprintf(stderr,"Unable to open input_parameters file.\n");
        fprintf(stderr,"Cannot continue. Create file with input and output paths.\n");
        exit(1);
      }

   if(!(fusrin = fopen("usrhgt_lvls","r")))
      {
        fprintf(stderr,"Unable to open usrmsg data file.\n");
        fprintf(stderr,"No user message computed.\n");
        usrswitch++;
      }

/*   if(!(fusr2in = fopen("usrhgt2_lvls","r")))
      {
        fprintf(stderr,"Unable to open usr2 data file.\n");
        fprintf(stderr,"No user2 message computed.\n");
        usr2switch++;
      }*/

    if(usrswitch !=0 /*&& usr2switch !=0*/)
      {
        fprintf(stderr,"No soundings computed.  Need height structure input files.\n");
        exit(1);
      }

  /* Get pathname, otherwise assume same directory as program. **/

   fscanf(fparams,"%s%s", inpath, outpath); 
   /*printf("%s %s \n", inpath, outpath);*/
   strcpy(snd->site.filename, charname[1]);
   strcpy(inputfilename, inpath);
   strcat(inputfilename, snd->site.filename);
   /*printf("\nInput file name is %s\n", inputfilename);*/

  /* Read data from input data file. ***************/

    readwrf(snd, inputfilename);
    printf("snd->site.filename = %s\n\n",snd->site.filename);

  /* Check for data that are out of bounds - gross error check.  ****/

   clean_data(snd, snd->level[0].hgt);
   printf("snd->nht = %8d\n", snd->nht);

   for (i=0;i<snd->nht;i++)
     {
       if(snd->level[i].hgt == ERROR || snd->level[i].tmp == ERROR || snd->level[i].prs
            == ERROR || snd->level[i].hum == ERROR || snd->level[i].spd == ERROR ||
            snd->level[i].dir == ERROR)
         {
           
           printf("snd->level[i].hgt = %9.2f\n",snd->level[i].hgt);
           printf("snd sounding data error!!!\n");
           printf("i, Line, Height = %4d %5d %9.1f\n", i, snd->level[i].lvlnum, 
                snd->level[i].hgt);
           
           if(i == 0)/*Don't use sounding if no or incomplete surface line,*/
             {                          
               printf("Check snd data before resuming.\n\n");
               exit(0);
             }
          }
      }

  /* Set up sounding heights. **************************************/

  /*    First USER defined message  ****************************/

    if(usrswitch ==0)
      {
       i=-1; 
       do
          {
            i++;
            fscanf(fusrin,"%f", &usrmsglevel->level[i].hgt);
         }
        while (usrmsglevel->level[i].hgt <MAXHEIGHT &&
               usrmsglevel->level[i].hgt <= snd->level[snd->nht-1].hgt && usrmsglevel->level[i].hgt != ERROR);
        usrmsglevel->nht = i;
        printf("\nNumber of USER DEFINED levels (usrmsglevel->nht) = %7d\n", usrmsglevel->nht);
       }

  /*    Second USER defined message  ****************************/

  /* if(usr2switch ==0)
      {
       i=-1; 
       do
          {
            i++;
            fscanf(fusr2in,"%f", &usrmsglevel->level[i].hgt);
         }
        while (usrmsglevel->level[i].hgt <MAXHEIGHT &&
               usrmsglevel->level[i].hgt <= snd->level[snd->nht-1].hgt && usrmsglevel->level[i].hgt != ERROR);
        usrmsglevel->nht = i;
        printf("\nNumber of USER2 DEFINED levels (usrmsglevel->nht) = %7d\n", usrmsglevel->nht);
       } */

  /* COMPUTATIONS. ##################################################*/
  
  /* Compute values for the selected types of output "soundings".  
     The function "msgvalues" performs the main work of the program, and 
     eliminates fields that are missing or out of bounds (identified 
     by ERROR/IERROR). ****/

   if(usrswitch ==0)
     msgvalues(snd, usrmsg, usrmsglevel);

   /*usrmsg->nht +=1; checkdata(usrmsg);*/

   /*if(usr2switch ==0)
     msgvalues(snd, usr2msg, usr2msglevel);*/

   /*usr2msg->nht +=1; checkdata(usr2msg);*/


   /* OUTPUT SECTION ##############################################*/

   if(usrswitch ==0)
     {
       writeusrmsg(usrmsg, outpath);
       writeusrlvl(usrmsglevel, outpath);
       fclose(fusrin);
     }

   /*if(usr2switch ==0)
     {
       writeusrmsg(usr2msg, outpath);
       writeusrlvl(usr2msglevel, outpath);
     }*/

   printf("\n");

   free(usrmsg);
   free(usrmsglevel);
   /*free(usr2msg);
   free(usr2msglevel);*/ 
   fclose(fparams);

}  /* End of convert data program. &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

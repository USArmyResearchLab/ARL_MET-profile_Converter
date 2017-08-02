/** FILE NAME: convertdataprswyo.c
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

  This program converts input from a pressure based raob output file           
  into a user defined message based on pressure levels and produces 
  output at the user defined pressure levels as well as layers. */

#include "convert.h"

int main(int argnum, char* charname[])
                 /* charname[] is the name of the input file. Copies to "snd->site.filename."*/
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

   fscanf(fparams, "%s%s", inpath, outpath);
   /*printf("\ninpath = %s  outpath = %s \n\n", inpath, outpath);*/
   strcpy(snd->site.filename, charname[1]);
   strcpy(inputfilename, inpath);
   strcat(inputfilename, snd->site.filename);
   printf("\nInput file name is %s\n", inputfilename);

  /* Read data from input data file. ***************/
 
    readraob(snd, inputfilename);   
    printf("snd->site.filename = %s\n\n",snd->site.filename);

  /* Check for data that are out of bounds - gross error check. ****/

   clean_data(snd, snd->level[0].hgt);

  /* Set up message pressure levels. **************************************/

  /*  For USER defined message  ****************************/

    if(usrswitch == 0 || usrswitchlvl == 0) 
      {
        i=0;
        j=0;

        while(fscanf(fusrin,"%f", &usrmsglevel->level[i].prs) == 1 && usrmsglevel->level[i].prs >= snd->level[snd->nht-1].prs)
          {
            i++;
          }
        nht = i; printf("\n");
        usrmsglevel->nht = nht; /*printf("usrmsglevel->nht = %3d\n", usrmsglevel->nht);*/

        if(snd->level[0].prs > usrmsglevel->level[0].prs) /* Where surface P > input maximum P level*/
          {
             for(i=nht;i>0;i--)
               {
                 usrmsglevel->level[i].prs = usrmsglevel->level[i-1].prs;
               }
             usrmsglevel->level[0].prs = snd->level[0].prs; 
          }

        if(snd->level[0].prs < usrmsglevel->level[0].prs)  /* Where surface P < or = input maximum P level.*/
          {
            while(usrmsglevel->level[j].prs >= snd->level[0].prs) /*  ##### Added = ##### */
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

}  /* End of convertprs data program. &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

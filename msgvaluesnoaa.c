/** FILE NAME: msgvaluesnoaa.c
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

  This program computes level and layer values for an output sounding.  Input data 
  from the NOAA archive site. No need to eliminate lines with missing data.  
  Pressure computed with hypsometric method from nearest observation immediately below, 
  if within the layer, or from bottom level of each layer.    
*/

#include "convert.h"

int msgvalues(struct sound *snd, struct sound *msg, struct sound *mlevel)
{
                               /* msg = layer values, mlevel = level values */
   int i, j, index;
   int size, msize, tsize, wsize;  /*tsize is for temperature, wsize for wind */
   float zmax, dir;
   double speed, direction;  /* these wind variables for component & spd/dir calculations*/
   float tv, tv0, z, z0, z_ratio;

   struct temporary *soundt;  /* use in level and layer functions */
   struct temporary *soundw;
   struct temporary *leveltempt;
   struct temporary *leveltempw;
   struct temporary *layertempt;
   struct temporary *layertempw;

   soundt = (struct temporary *)malloc(sizeof(struct temporary));
   leveltempt = (struct temporary *)malloc(sizeof(struct temporary));
   layertempt = (struct temporary *)malloc(sizeof(struct temporary));
   soundw = (struct temporary *)malloc(sizeof(struct temporary));
   leveltempw = (struct temporary *)malloc(sizeof(struct temporary));
   layertempw = (struct temporary *)malloc(sizeof(struct temporary));

/* Initialize structure and temporary variables **********************************/

     for(i=0;i<MAXSIZE;i++)  /*For height, T, Tv, and Td.*/
       {
         soundt->h[i] = ERROR;
         soundt->t[i] = ERROR;
         soundt->tv[i] = ERROR;
         soundt->p[i] = ERROR;
         soundt->dew[i] = ERROR;
         leveltempt->h[i] = ERROR;
         leveltempt->t[i] = ERROR;
         leveltempt->tv[i] = ERROR;
         leveltempt->p[i] = ERROR;
         leveltempt->dew[i] = ERROR;
         layertempt->h[i] = ERROR;
         layertempt->t[i] = ERROR;
         layertempt->tv[i] = ERROR;
         layertempt->p[i] = ERROR;
         layertempt->dew[i] = ERROR;
        }

     for(i=0;i<MAXSIZE;i++)  /* For wind speed and direction.*/
       {
         soundw->h[i] = ERROR;
         soundw->t[i] = ERROR;
         soundw->tv[i] = ERROR;
         soundw->p[i] = ERROR;
         soundw->u[i] = ERROR;
         soundw->v[i] = ERROR;
         leveltempw->h[i] = ERROR;
         leveltempw->t[i] = ERROR;
         leveltempw->tv[i] = ERROR;
         leveltempw->p[i] = ERROR;
         leveltempw->u[i] = ERROR;
         leveltempw->v[i] = ERROR;
         layertempw->h[i] = ERROR;
         layertempw->t[i] = ERROR;
         layertempw->tv[i] = ERROR;
         layertempw->p[i] = ERROR;
         layertempw->u[i] = ERROR;
         layertempw->v[i] = ERROR;
        }

/* Parameters for level and layer values. ****************/

   msg->nht = mlevel->nht - 1;  /* number of layer values one less than level values */
   size = snd->nht;
   msize = mlevel->nht;
   zmax = snd->level[size-1].hgt + 0.001;

/* Compute temperature and virtual temperature ###################################*/
    
    /*  Compute level values. ***************************************/

    for (i=0;i<size;i++)
     {
       if(snd->level[i].tmp != ERROR && snd->level[i].dew != ERROR)
         {
           snd->level[i].tmp += 273.16;  /*Change to K from C.  Need to do here vs. in main or read routines.*/
           snd->level[i].dew += 273.16;  /*Change to K from C.*/
           snd->level[i].vtmp = tvfromtemptd(snd->level[i].tmp, snd->level[i].prs, snd->level[i].dew);

         }
     }

   j=-1;                 /*Set up temporary variables for use in level and for layer as needed */
   for (i=0;i<size;i++)
     {  
 
       if(snd->level[i].tmp != ERROR && snd->level[i].dew != ERROR)
         {
           j++;
           soundt->h[j] = snd->level[i].hgt;
           soundt->t[j] = snd->level[i].tmp;
           soundt->tv[j] = snd->level[i].vtmp;
           soundt->p[j] = snd->level[i].prs;
           soundt->dew[j] = snd->level[i].dew;
         }
      }

   tsize = j; /*printf("tsize = %4d\n\n", tsize);*/ 

   for (i=0;i<msize;i++)
     {
        leveltempt->h[i] = mlevel->level[i].hgt;
        layertempt->h[i] = leveltempt->h[i];
     }

   /*  Compute level values */

   level(zmax, msize, leveltempt->h, soundt->t, leveltempt->t, soundt->h);
   level(zmax, msize, leveltempt->h, soundt->tv, leveltempt->tv, soundt->h);
   level(zmax, msize, leveltempt->h, soundt->dew, leveltempt->dew, soundt->h);

 /* Compute layer values. ********************************************/

   layer(zmax, tsize, msize, layertempt->h, soundt->t, leveltempt->t, layertempt->t, soundt->h);
   layer(zmax, tsize, msize, layertempt->h, soundt->tv, leveltempt->tv, layertempt->tv, soundt->h);
   layer(zmax, tsize, msize, layertempt->h, soundt->dew, leveltempt->dew, layertempt->dew, soundt->h);

   /* Wind Speed and Direction ###########################################*/

/* Compute components from snd (input) wind speed and direction. *************************/

   j=-1;
   for (i=0;i<size;i++)
     {  
       if(snd->level[i].spd != ERROR && snd->level[i].dir != ERROR)
         {
            j++;

            dir = -snd->level[i].dir * M_PI/180 + 3*M_PI/2;  
            soundw->u[j] = cos(dir) * snd->level[i].spd;
            soundw->v[j] = sin(dir) * snd->level[i].spd;
        
            soundw->h[j] = snd->level[i].hgt;

         }
     }
   wsize = j; /*printf("wsize = %4d\n\n", wsize);*/ 

   for (i=0;i<msize;i++)
     {
        leveltempw->h[i] = mlevel->level[i].hgt;
        layertempw->h[i] = leveltempw->h[i];
     }

   /* Compute level values.  *******************************/

   level(zmax, msize, leveltempw->h, soundw->u, leveltempw->u, soundw->h);
   level(zmax, msize, leveltempw->h, soundw->v, leveltempw->v, soundw->h);
      
   /* Compute level wind speed & direction from components. *****************/

   for(i=0;i<msize;i++)
     {

       mlevel->level[i].dir = (2*M_PI - atan2(leveltempw->u[i], -leveltempw->v[i]))*180/M_PI;
       if(mlevel->level[i].dir > 360)
         mlevel->level[i].dir -= 360;

       mlevel->level[i].spd = sqrt(leveltempw->u[i]*leveltempw->u[i] + leveltempw->v[i]*leveltempw->v[i]);          
     }

   /* Compute layer values of components. ********************************/

    layer(zmax, wsize, msize, layertempw->h, soundw->u, leveltempw->u, layertempw->u, soundw->h);
    layer(zmax, wsize, msize, layertempw->h, soundw->v, leveltempw->v, layertempw->v, soundw->h);

   /* Compute message layer wind speed & direction from components.  Note that msg->level[i].spd, etc.
       refer to layer values.  Also, load component values into msg structure.      *******************/

     msg->level[0].spd = snd->level[0].spd;
     msg->level[0].dir = snd->level[0].dir;
     msg->level[0].u = soundw->u[0]; 
     msg->level[0].v = soundw->v[0];
     
     for(i=1;i<msize;i++)
       {
         msg->level[i].u = layertempw->u[i-1];          
         msg->level[i].v = layertempw->v[i-1];

         msg->level[i].dir = (2*M_PI - atan2(msg->level[i].u, -msg->level[i].v))*180/M_PI;
         if(msg->level[i].dir > 360)
           msg->level[i].dir -= 360;

         msg->level[i].spd = sqrt(msg->level[i].u*msg->level[i].u + msg->level[i].v*msg->level[i].v);
       }

 /* Load height values into msg structures (level values already via mlevel). ###########*/

   for (i=0;i<msize;i++)
     {  
        msg->level[i].hgt = mlevel->level[i].hgt;
     }

 /* Load t, tv, hum values into mlevel and msg structures.  ##################################*/
 
   for (i=0;i<msize;i++)                          /* level values */
     {
        mlevel->level[i].tmp = leveltempt->t[i];
        mlevel->level[i].vtmp = leveltempt->tv[i];
        mlevel->level[i].dew = leveltempt->dew[i];
     }
 
   msg->level[0].tmp = snd->level[0].tmp;        /* msg values (= surface + layers) */
   msg->level[0].vtmp = snd->level[0].vtmp;
   msg->level[0].dew = snd->level[0].dew;
   for (i=1;i<msize;i++)
     {  
        msg->level[i].tmp = layertempt->t[i-1];
        msg->level[i].vtmp = layertempt->tv[i-1];
        msg->level[i].dew = layertempt->dew[i-1];
     } 

   /* Pressure values. ###############################################*/
      

  /* Compute level pressure values using better hypsometric method. This part was modified to account for
       for cases of few observation levels and large vertical gap(s) between observation levels. **********/

   mlevel->level[0].prs = snd->level[0].prs;

   j=0;
   
   for(i=1; i<msize; i++)              /*Values for temporary sound structure set before level calculations above.*/
     {
       while(soundt->h[j] < mlevel->level[i].hgt)
         {  
           j++;
         }

       if(soundt->h[j] == mlevel->level[i].hgt)
         mlevel->level[i].prs = soundt->p[j];
       else
         {

            if(soundt->h[j-1] < mlevel->level[i-1].hgt && j > 0)
               { 
                   mlevel->level[i].prs = presscomp(mlevel->level[i].vtmp, mlevel->level[i-1].vtmp,
                                          mlevel->level[i-1].prs, mlevel->level[i].hgt, mlevel->level[i-1].hgt); 
               }
            else
               { 
                   mlevel->level[i].prs = presscomp(mlevel->level[i].vtmp, soundt->tv[j-1],
                                          soundt->p[j-1], mlevel->level[i].hgt, soundt->h[j-1]);
               }

         }

     }
       /*printf("\n\n Ended at computation of level pressures. \n\n"); exit(0);*/

   /* Compute layer pressure values. ***************************************************/

   msg->level[0].prs = mlevel->level[0].prs;     /* msg values (= surface + layers)*/
   j=1;

   for(i=1;i<msize;i++)
     {
    
       while(soundt->h[j] < (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5)
         {  
           j++;
           /*printf("j = %5d\n", j);*/
         }
       
       if(soundt->h[j] == (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5)
         {
           msg->level[i].prs = soundt->p[j]; 
           printf("Height = midpoint: i, j, soundt->h[j], midpoint = %3d %3d %6.1f %6.1f\n\n",
                   i, j, soundt->h[j], (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5);
         }
       else
         {

          if(soundt->h[j-1] < (mlevel->level[i-1].hgt + mlevel->level[i-2].hgt)*0.5) 
            {
              tv = msg->level[i].vtmp;
              tv0 = mlevel->level[i-1].vtmp;
              z = (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5;
              z0 = mlevel->level[i-1].hgt;

              msg->level[i].prs = presscomp(tv, tv0, mlevel->level[i-1].prs, z, z0);

            }
          else
            {
              tv = msg->level[i].vtmp;
              tv0 = soundt->tv[j];
              z = (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5;
              z0 = soundt->h[j-1];

              msg->level[i].prs = presscomp(tv, tv0, soundt->p[j-1], z, z0);

            }  
         }  
     }

  /* Load in site information (date, time, lat, lon, etc.). ######## */

   msg->site = snd->site;
   mlevel->site = snd->site;   /*For use with level output when needed.*/

  /*Free temporary arrays. ****************************/

 free(soundt);
 free(leveltempt);
 free(layertempt);
 free(soundw);
 free(leveltempw);
 free(layertempw);

 return;

}  /* End of level and layer computation section. *****************/

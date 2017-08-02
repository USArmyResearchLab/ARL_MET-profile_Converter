/** FILE NAME: msgvalues.c
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

  This program computes level and layer values for output soundings.  Pressure 
  computed with hypsometric method from observation level immediately below,
  if within the layer, or from bottom level of that layer.
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

   struct temporary *sound;  /* use in level and layer functions */
   struct temporary *leveltemp;
   struct temporary *layertemp;

   sound = (struct temporary *)malloc(sizeof(struct temporary));
   leveltemp = (struct temporary *)malloc(sizeof(struct temporary));
   layertemp = (struct temporary *)malloc(sizeof(struct temporary));

/* Initialize structure and temporary variables **********************************/

     for(i=0;i<MAXSIZE;i++)
       {
         sound->h[i] = ERROR;
         sound->t[i] = ERROR;
         sound->tv[i] = ERROR;
         sound->p[i] = ERROR;
         sound->hum[i] = ERROR;
         sound->u[i] = ERROR;
         sound->v[i] = ERROR;
         leveltemp->h[i] = ERROR;
         leveltemp->t[i] = ERROR;
         leveltemp->tv[i] = ERROR;
         leveltemp->p[i] = ERROR;
         leveltemp->hum[i] = ERROR;
         leveltemp->u[i] = ERROR;
         leveltemp->v[i] = ERROR;
         layertemp->h[i] = ERROR;
         layertemp->t[i] = ERROR;
         layertemp->tv[i] = ERROR;
         layertemp->p[i] = ERROR;
         layertemp->hum[i] = ERROR;
         layertemp->u[i] = ERROR;
         layertemp->v[i] = ERROR;
        }

/* Parameters for level and layer values. ****************/

   msg->nht = mlevel->nht - 1;  /* number of layer values one less than level values */
   size = snd->nht;
   msize = mlevel->nht;
   zmax = snd->level[size-1].hgt + 0.001;
  
/* Compute components from snd (input) wind speed and direction. *************************/

   j=-1;
   for (i=0;i<size;i++)
     {  
       if(snd->level[i].spd != ERROR && snd->level[i].dir != ERROR)
         {
            j++;

            dir = -snd->level[i].dir * M_PI/180 + 3*M_PI/2;  
            sound->u[j] = cos(dir) * snd->level[i].spd;
            sound->v[j] = sin(dir) * snd->level[i].spd;
        
            sound->h[j] = snd->level[i].hgt;  /* In this version can compute sound->h for both wind and PTH
                                              /* since all data levels should have all variables. */

         }
     }
    wsize = j; /*printf("wsize = %4d\n", wsize);*/

/* Compute temperature and virtual temperature ###################################*/
    
    /*  Compute level values. ***************************************/

    for (i=0;i<size;i++)
     {
        snd->level[i].vtmp = tvfromtemp(snd->level[i].tmp, snd->level[i].prs, snd->level[i].hum);
     }

   j=-1;             /*set up temporary variables for use in level and for layer as needed */
   for (i=0;i<size;i++)
     {  
       if(snd->level[i].hgt != ERROR && snd->level[i].tmp != ERROR && 
               snd->level[i].prs != ERROR)
         {
           j++;
           /*sound->h[j] = snd->level[i].hgt;*/ /*Computed above in wind components section: all data levels should have all variables.*/
           sound->t[j] = snd->level[i].tmp;
           sound->tv[j] = snd->level[i].vtmp;
           sound->p[j] = snd->level[i].prs;
           sound->hum[j] = snd->level[i].hum;
        }
      }
   tsize = j; /*printf("tsize = %4d\n", tsize);*/

/* Check that all data levels really do have all variables. Should be equal for this version of program. *******/

   if(tsize != wsize)
     {
        printf("tsize = %4d    wsize = %4d\n", tsize, wsize);
        printf("Check sounding data for unusual or missing data fields.  Exiting program.\n\n");
        exit(0);
     }
/* End of tsize and wsize check. ************************/


   for (i=0;i<msize;i++)
     {
        leveltemp->h[i] = mlevel->level[i].hgt;
        layertemp->h[i] = leveltemp->h[i];
     }

   /*  Compute level values */

   level(zmax, msize, leveltemp->h, sound->t, leveltemp->t, sound->h);
   level(zmax, msize, leveltemp->h, sound->tv, leveltemp->tv, sound->h);
   level(zmax, msize, leveltemp->h, sound->hum, leveltemp->hum, sound->h);

 /* Compute layer values. ********************************************/

   layer(zmax, tsize, msize, layertemp->h, sound->t, leveltemp->t, layertemp->t, sound->h);
   layer(zmax, tsize, msize, layertemp->h, sound->tv, leveltemp->tv, layertemp->tv, sound->h);
   layer(zmax, tsize, msize, layertemp->h, sound->hum, leveltemp->hum, layertemp->hum, sound->h);

   /* Wind Speed and Direction ###########################################*/

   /* Compute level values.  *******************************/
  
   level(zmax, msize, leveltemp->h, sound->u, leveltemp->u, sound->h);
   level(zmax, msize, leveltemp->h, sound->v, leveltemp->v, sound->h);
       
   /* Compute level wind speed & direction from components. *****************/

   for(i=0;i<msize;i++)
     {

       mlevel->level[i].dir = (2*M_PI - atan2(leveltemp->u[i], -leveltemp->v[i]))*180/M_PI;
       if(mlevel->level[i].dir > 360)
         mlevel->level[i].dir -= 360;

       mlevel->level[i].spd = sqrt(leveltemp->u[i]*leveltemp->u[i] + leveltemp->v[i]*leveltemp->v[i]);          
     }

   /* Compute layer values of components. ********************************/

    layer(zmax, wsize, msize, layertemp->h, sound->u, leveltemp->u, layertemp->u, sound->h);
    layer(zmax, wsize, msize, layertemp->h, sound->v, leveltemp->v, layertemp->v, sound->h);

   /* Compute message layer wind speed & direction from components.  Note that msg->level[i].spd, etc.
       refers to layer values, and load component values into msg structure.         *******************/

     msg->level[0].spd = snd->level[0].spd;
     msg->level[0].dir = snd->level[0].dir;
     msg->level[0].u = sound->u[0];        
     msg->level[0].v = sound->v[0];
     
     for(i=1;i<msize;i++)
       {
         msg->level[i].u = layertemp->u[i-1];          
         msg->level[i].v = layertemp->v[i-1];
 
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
        mlevel->level[i].tmp = leveltemp->t[i];
        mlevel->level[i].vtmp = leveltemp->tv[i];
        mlevel->level[i].hum = leveltemp->hum[i];
     }
 
   msg->level[0].tmp = snd->level[0].tmp;        /* msg values (= surface + layers) */
   msg->level[0].vtmp = snd->level[0].vtmp;
   msg->level[0].hum = snd->level[0].hum;
   for (i=1;i<msize;i++)
     {  
        msg->level[i].tmp = layertemp->t[i-1];
        msg->level[i].vtmp = layertemp->tv[i-1];
        msg->level[i].hum = layertemp->hum[i-1];
     } 

   /* Pressure values. ###############################################*/
      

  /* Compute level pressure values using better hypsometric method. This part was modified to account for
       for cases of few observation levels and large vertical gap(s) between observation levels. ************/

   mlevel->level[0].prs = snd->level[0].prs;

   j=0;
   
   for(i=1; i<msize; i++)              /*Values for temporary sound structure set before level calculations above.*/
     {
       while(sound->h[j] < mlevel->level[i].hgt)
         {  
           j++;
           /*printf("j = %5d\n", j);*/
         }

       if(sound->h[j] == mlevel->level[i].hgt)
         mlevel->level[i].prs = sound->p[j];
       else
         {

            if(sound->h[j-1] < mlevel->level[i-1].hgt && j > 0)
               { mlevel->level[i].prs = presscomp(mlevel->level[i].vtmp, mlevel->level[i-1].vtmp,
                                          mlevel->level[i-1].prs, mlevel->level[i].hgt, mlevel->level[i-1].hgt); 

               }
            else
               { mlevel->level[i].prs = presscomp(mlevel->level[i].vtmp, sound->tv[j-1],
                                          sound->p[j-1], mlevel->level[i].hgt, sound->h[j-1]);
               }
         }
     }

   /* Compute layer pressure values. ***************************************************/

   msg->level[0].prs = mlevel->level[0].prs;     /* msg values (= surface + layers)*/
   j=1;

   for(i=1;i<msize;i++)
     {
    
       while(sound->h[j] < (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5)
         {  
           j++;
         }
       
       if(sound->h[j] == (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5)
         {
           msg->level[i].prs = sound->p[j]; 
           printf("Height = midpoint: i, j, sound->h[j], midpoint = %3d %3d %6.1f %6.1f\n\n",
                   i, j, sound->h[j], (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5);
         }
       else
         {

          if(sound->h[j-1] < (mlevel->level[i-1].hgt + mlevel->level[i-2].hgt)*0.5) 
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
              tv0 = sound->tv[j];
              z = (mlevel->level[i].hgt + mlevel->level[i-1].hgt)*0.5;
              z0 = sound->h[j-1];

              msg->level[i].prs = presscomp(tv, tv0, sound->p[j-1], z, z0);
            }  
         }  
     }

   /*printf("Ending at computation of layer pressure values.\n\n");exit(0);*/
    
   /* Load in site information (date, time, lat, lon, etc.). ######## */

   msg->site = snd->site;
   mlevel->site = snd->site; /* For use with level output as needed.*/

/* Free temporary arrays. *********/

 free(sound);
 free(leveltemp);
 free(layertemp);

 return;

}  /* End of level and layer computation section. *****************/


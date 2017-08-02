/** FILE NAME: msgvaluesprs.c
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

  This program computes level and layer values for a user defined pressure (P) level profile.  
  Interpolation is performed with respect to ln(P) vs. P.                                                             
*/

#include "convert.h"

int msgvaluesprs(struct sound *snd, struct sound *msg, struct sound *mlevel)
{
                               /* msg = layer values, mlevel = level values */
   int i, j, index;
   int size, msize, tsize, wsize;  /*tsize is for temperature, wsize for wind */
   float pmin, dir;               
   float tv, tv0, z, z0, p, p0;

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

   msg->nht = mlevel->nht - 1;  /* Number of layer values are one less than level values. */
   size = snd->nht;
   msize = mlevel->nht;
   pmin = log(snd->level[size-1].prs) - 0.0001;
   /*printf("ln(pmin), pmin, size, msize = %9.4f %9.2f %7d %7d\n\n",pmin, exp(pmin), size, msize);*/ 
  
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
        
            sound->p[j] = log(snd->level[i].prs);
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
           /*sound->p[j] = log(snd->level[i].prs);*/ /*Computed above in wind components section.*/
           sound->t[j] = snd->level[i].tmp;
           sound->tv[j] = snd->level[i].vtmp;
           sound->h[j] = snd->level[i].hgt;
           sound->hum[j] = snd->level[i].hum;
        }
      }

   tsize = j; /*printf("tsize = %4d\n", tsize);*/

   for (i=0;i<msize;i++)
     {
        leveltemp->p[i] = log(mlevel->level[i].prs);
        layertemp->p[i] = leveltemp->p[i];
     }

   /*  Compute level values. Includes height since using ln(pressure).*/  
 
   levelprs(pmin, msize, leveltemp->p, sound->t, leveltemp->t, sound->p);
   levelprs(pmin, msize, leveltemp->p, sound->tv, leveltemp->tv, sound->p);
   levelprs(pmin, msize, leveltemp->p, sound->hum, leveltemp->hum, sound->p);
   levelprs(pmin, msize, leveltemp->p, sound->h, leveltemp->h, sound->p);

 /* Compute layer values. ********************************************/

   layerprs(pmin, tsize, msize, layertemp->p, sound->t, leveltemp->t, layertemp->t, sound->p); 
   layerprs(pmin, tsize, msize, layertemp->p, sound->tv, leveltemp->tv, layertemp->tv, sound->p);
   layerprs(pmin, tsize, msize, layertemp->p, sound->hum, leveltemp->hum, layertemp->hum, sound->p);
   layerprs(pmin, tsize, msize, layertemp->p, sound->h, leveltemp->h, layertemp->h, sound->p);

   /* Wind Speed and Direction ###########################################*/

   /* Compute level values.  *******************************/
  
   levelprs(pmin, msize, leveltemp->p, sound->u, leveltemp->u, sound->p); 
   levelprs(pmin, msize, leveltemp->p, sound->v, leveltemp->v, sound->p);
       
   /* Compute level wind speed & direction from components. *****************/

   for(i=0;i<msize;i++)
     {

       mlevel->level[i].dir = (2*M_PI - atan2(leveltemp->u[i], -leveltemp->v[i]))*180/M_PI;
       if(mlevel->level[i].dir > 360)
         mlevel->level[i].dir -= 360;

       mlevel->level[i].spd = sqrt(leveltemp->u[i]*leveltemp->u[i] + leveltemp->v[i]*leveltemp->v[i]);          
     }

   /* Compute layer values of components. ********************************/

    layerprs(pmin, wsize, msize, layertemp->p, sound->u, leveltemp->u, layertemp->u, sound->p);
    layerprs(pmin, wsize, msize, layertemp->p, sound->v, leveltemp->v, layertemp->v, sound->p);

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

 /* Load pressure values into msg structures (level values already via mlevel). ###########*/


   for (i=0;i<msize;i++)
     {  
        msg->level[i].prs = mlevel->level[i].prs;
     }

 /* Load t, tv, hum, h values into mlevel and msg structures.  ##################################*/
 
   for (i=0;i<msize;i++)                          /* level values */
     {
        mlevel->level[i].tmp = leveltemp->t[i];
        mlevel->level[i].vtmp = leveltemp->tv[i];
        mlevel->level[i].hum = leveltemp->hum[i];
        mlevel->level[i].hgt = leveltemp->h[i];
     }
 
   msg->level[0].tmp = snd->level[0].tmp;        /* msg values (= surface + layers) */
   msg->level[0].vtmp = snd->level[0].vtmp;
   msg->level[0].hum = snd->level[0].hum;
   msg->level[0].hgt = snd->level[0].hgt;

   for (i=1;i<msize;i++)
     {  
        msg->level[i].tmp = layertemp->t[i-1];
        msg->level[i].vtmp = layertemp->tv[i-1];
        msg->level[i].hum = layertemp->hum[i-1];
        msg->level[i].hgt = layertemp->h[i-1];
     }       
    
 /* Load in site information (date, time, lat, lon, etc.). ######## */

   msg->site = snd->site;
   mlevel->site = snd->site;   /*For use with level output as needed.*/

/* Free temporary arrays. ****************/

 free(sound);
 free(leveltemp);
 free(layertemp);

 return;

}
/* End of level and layer computation section. *****************/

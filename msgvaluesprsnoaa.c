/** FILE NAME: msgvaluesprsnoaa.c
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
   
  This program computes level and layer values for a user defined pressure level profile using
  data from the NOAA archive.                                                             
*/

#include "convert.h"

int msgvaluesprs(struct sound *snd, struct sound *msg, struct sound *mlevel)
{
                               /* msg = layer values, mlevel = level values */
   int i, j, index;
   int size, msize, tsize, wsize;  /*tsize is for temperature, wsize for wind */
   float pmin, dir;               
   float tv, tv0, z, z0, p, p0;

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

     for(i=0;i<MAXSIZE;i++)  /*For height, T, Td, Tv.*/
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

   msg->nht = mlevel->nht - 1;  /* Number of layer values are one less than level values. */
   size = snd->nht;
   msize = mlevel->nht;
   pmin = log(snd->level[size-1].prs) - 0.0001;
   /*printf("ln(pmin), pmin, size, msize = %9.4f %9.2f %7d %7d\n\n",pmin, exp(pmin), size, msize);*/

/* Compute temperature and virtual temperature ###################################*/
    
    /*  Compute level values. ***************************************/

    for (i=0;i<size;i++)
     {
       if(snd->level[i].tmp != ERROR && snd->level[i].dew != ERROR)
         {
           snd->level[i].tmp += 273.16;  /*Change to K from C.*/
           snd->level[i].dew += 273.16;  /*Change to K from C.*/
           snd->level[i].vtmp = tvfromtemptd(snd->level[i].tmp, snd->level[i].prs, snd->level[i].dew);
         }
     }

   j=-1;              /*Set up temporary variables for use in level and for layer as needed */
   for (i=0;i<size;i++)
     {  
 
       if(snd->level[i].tmp != ERROR && snd->level[i].dew != ERROR)
         {
           j++;
           soundt->p[j] = snd->level[i].prs;
           soundt->t[j] = snd->level[i].tmp;
           soundt->tv[j] = snd->level[i].vtmp;
           soundt->h[j] = snd->level[i].hgt;
           soundt->dew[j] = snd->level[i].dew;
         }
      }

   tsize = j; /*printf("tsize = %4d\n\n", tsize);*/ 


   for (i=0;i<msize;i++)
     {
        leveltempt->p[i] = mlevel->level[i].prs;
        layertempt->p[i] = leveltempt->p[i];
     }

   /*  Compute level values */  
 
   levelprs(pmin, msize, leveltempt->p, soundt->t, leveltempt->t, soundt->p);
   levelprs(pmin, msize, leveltempt->p, soundt->tv, leveltempt->tv, soundt->p);
   levelprs(pmin, msize, leveltempt->p, soundt->dew, leveltempt->dew, soundt->p);
   levelprs(pmin, msize, leveltempt->p, soundt->h, leveltempt->h, soundt->p);

 /* Compute layer values. ********************************************/

   layerprs(pmin, tsize, msize, layertempt->p, soundt->t, leveltempt->t, layertempt->t, soundt->p); 
   layerprs(pmin, tsize, msize, layertempt->p, soundt->tv, leveltempt->tv, layertempt->tv, soundt->p);
   layerprs(pmin, tsize, msize, layertempt->p, soundt->dew, leveltempt->dew, layertempt->dew, soundt->p);
   layerprs(pmin, tsize, msize, layertempt->p, soundt->h, leveltempt->h, layertempt->h, soundt->p);

 /* Wind Speed and Direction ###########################################*/

  /* Compute components from snd (input) wind speed and direction. ******************/

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
            soundw->p[j] = snd->level[i].prs;
         }
     }
   wsize = j; /*printf("wsize = %4d\n\n", wsize);*/ /*exit(0);*/


   for (i=0;i<msize;i++)
     {
        leveltempw->p[i] = mlevel->level[i].prs;
        layertempw->p[i] = leveltempw->p[i];
     }

   /* Compute level values.  *******************************/
  
   levelprs(pmin, msize, leveltempw->p, soundw->u, leveltempw->u, soundw->p); 
   levelprs(pmin, msize, leveltempw->p, soundw->v, leveltempw->v, soundw->p);
       
   /* Compute level wind speed & direction from components. *****************/

   for(i=0;i<msize;i++)
     {
       mlevel->level[i].dir = (2*M_PI - atan2(leveltempw->u[i], -leveltempw->v[i]))*180/M_PI;
       if(mlevel->level[i].dir > 360)
         mlevel->level[i].dir -= 360;

       mlevel->level[i].spd = sqrt(leveltempw->u[i]*leveltempw->u[i] + leveltempw->v[i]*leveltempw->v[i]);          
     }

   /* Compute layer values of components. ********************************/

    layerprs(pmin, wsize, msize, layertempw->p, soundw->u, leveltempw->u, layertempw->u, soundw->p);
    layerprs(pmin, wsize, msize, layertempw->p, soundw->v, leveltempw->v, layertempw->v, soundw->p);

   /* Compute message layer wind speed & direction from components.  Note that msg->level[i].spd, etc.
       refer to layer values, and load component values into msg structure.         *******************/

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


 /* Load pressure values into msg structures (level values already via mlevel). ###########*/

   for (i=0;i<msize;i++)
     {  
        msg->level[i].prs = mlevel->level[i].prs;
     }

 /* Load h, t, tv, hum values into mlevel and msg structures.  ##################################*/
 
   for (i=0;i<msize;i++)                          /* level values */
     {
        mlevel->level[i].hgt = leveltempt->h[i];
        mlevel->level[i].tmp = leveltempt->t[i];
        mlevel->level[i].vtmp = leveltempt->tv[i];
        mlevel->level[i].dew = leveltempt->dew[i];
     }
 
   msg->level[0].hgt = snd->level[0].hgt;
   msg->level[0].tmp = snd->level[0].tmp;        /* msg values (= surface + layers) */
   msg->level[0].vtmp = snd->level[0].vtmp;
   msg->level[0].dew = snd->level[0].dew;
   for (i=1;i<msize;i++)
     {
        msg->level[i].hgt = layertempt->h[i-1];  
        msg->level[i].tmp = layertempt->t[i-1];
        msg->level[i].vtmp = layertempt->tv[i-1];
        msg->level[i].hum = layertempt->hum[i-1];
     } 


   /* Load in site information (date, time, lat, lon, etc.). ######## */

   msg->site = snd->site;
   mlevel->site = snd->site;

 free(soundt);
 free(leveltempt);
 free(layertempt);
 free(soundw);
 free(leveltempw);
 free(layertempw);

 return;

}
/* End of level and layer computation section. *****************/

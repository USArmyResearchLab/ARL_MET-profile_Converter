/** FILE NAME: clean_data.c
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

   Checks if the data values are within pre-defined bounds (not restrictive).
   A data value is set to the value for missing data (-999.0 or -999) if 
   it lies outside of the "useful data" envelope.  
*/

#include "convert.h"       

int clean_data(struct sound *data, float min_height)
{
   int i;

   struct value *check;
   check = (struct value *)malloc(sizeof(struct value));

  /* Use only if need to convert data into AGL. */
  /* for(i=0;i<data->nht;i++)
     {
       data->level[i].hgt -= data->site.elev;
     }
  */

   /* Set gross error check values. *******/

   check->prs_min=2.0;    /*hPa*/
   check->prs_max=1090.0;
   check->hgt_min=min_height; 
   check->hgt_max=MAXHEIGHT;
   check->tmp_min=153; /*degrees K*/
   check->tmp_max=353.0;
   check->hum_min=0.0;  /*relative humidity*/ 
   check->hum_max=100.5;
   check->spd_min=0;
   check->spd_max=200.0;
   check->dir_min=0;
   check->dir_max=360.0; /*degrees*/
   check->u_min=-200.0;
   check->u_max=200.0;
   check->v_min=-200.0;
   check->v_max=200.0;   
 
  /* Perform gross error check-> *********/

 for(i=0;i<data->nht;i++) 
  {
   if(data->level[i].hgt != ERROR && (data->level[i].hgt < check->hgt_min ||
	  data->level[i].hgt > check->hgt_max))
     {
        data->level[i].hgt=ERROR;
        data->level[i].tmp=ERROR;
        data->level[i].hum=ERROR;
        data->level[i].spd=ERROR;
        data->level[i].dir=ERROR;
        data->level[i].prs=ERROR;
        data->level[i].u=ERROR;
        data->level[i].v=ERROR;
     }
  else 
    {
     if(data->level[i].prs != ERROR && (data->level[i].prs < check->prs_min ||
          data->level[i].prs > check->prs_max))
               data->level[i].prs = ERROR;

     if(data->level[i].tmp != ERROR && (data->level[i].tmp < check->tmp_min ||
	  data->level[i].tmp > check->tmp_max))
               data->level[i].tmp = ERROR;

     if(data->level[i].hum != ERROR && (data->level[i].hum < check->hum_min ||
	  data->level[i].hum > check->hum_max))
               data->level[i].hum = ERROR;

     if(data->level[i].spd != ERROR && (data->level[i].spd < check->spd_min ||
	  data->level[i].spd > check->spd_max))
              data->level[i].spd = ERROR;

     if(data->level[i].dir != ERROR && (data->level[i].dir < check->dir_min ||
	  data->level[i].dir > check->dir_max))
              data->level[i].dir = ERROR;

     if(data->level[i].u != ERROR && (data->level[i].u < check->u_min ||
	  data->level[i].u > check->u_max))
              data->level[i].u = ERROR;

     if(data->level[i].v != ERROR && (data->level[i].v < check->v_min ||
	  data->level[i].v > check->v_max))
              data->level[i].v = ERROR;
  }
 }
  
 free(check);

}  /* End of clean_data function. **************/

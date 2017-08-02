/** FILE NAME: levelprs.c
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

  This program is used in the generation of a defined atmospheric structure of pressures and layers.  
  It produces values at defined pressure levels.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int levelprs (float pmin, int prsize, float *pl, float *value, 
           float *lev_value, float *p)

 {

  int i=0,j=0; 

  /*printf("pmin, prsize = %6.4f %3d\n\n", pmin, prsize);*/ 

  while(p[j] > pmin && pl[i] > pmin && i < prsize)
    {           
      if(pl[i] <= p[0])
        {
          /*printf("j, i, p[j], pl[i]: %3d %3d %6.4f %6.4f\n", j, i, p[j], pl[i]);*/

          if(pl[i] == p[j])
            {
              lev_value[i] = value[j];
              j++; 
            }
          else
            {
              if(p[j] < pl[i])
                {
                   j--;
                }
              lev_value[i] = value[j] - 
                   (value[j]-value[j+1])*(p[j]-pl[i])/(p[j]-p[j+1]);
             
              /*printf("i, pl[i], lev_value[i] = %3d %6.4f %6.1f\n\n", i, pl[i], lev_value[i]);*/ 
                                 
            }
         }

       while(pl[i+1] <= p[j+1] && p[j+1] >= pmin && p[j] > -999)
        {
          j++; 
        }
       
       i++; 
    }  
         
  return;

} /* End of level function. ##################*/

/** FILE NAME: level.c
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

  This program is used in the generation of level output, that is, any  
  defined atmospheric structure of heights.  It produces values at
  defined height levels.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int level (float zmax, int htsize, float *zh, float *value, 
           float *lev_value, float *z)

 {

  int i=0,j=0;

  while(z[j] < zmax && zh[i] < zmax && i < htsize)
    {
      if(zh[i] >= z[0])
        {
          if(zh[i] == z[j])
            {
              lev_value[i] = value[j];
              j++;
            }
          else
            {
              if(z[j] > zh[i])
                j--;
              lev_value[i] = value[j] - 
                   (value[j]-value[j+1])*(zh[i]-z[j])/(z[j+1]-z[j]);                                 
            }
         }
       
       /*printf("i, lev_value = %4d, %8.1f\n", i, lev_value[i]);*/

       while(zh[i+1] >= z[j+1] && z[j+1] > .00001)
         j++;
       i++;
    }  
   /*printf("\n\n");*/
   
   /*for(i=0;i<htsize;i++)
     {
       printf("i, height, level value = %4d, %7.1f, %7.1f\n",
                 i, zh[i], lev_value[i]);
     }
   printf("\n\n");*/ 

   return;

} /* End of level function. ##################*/

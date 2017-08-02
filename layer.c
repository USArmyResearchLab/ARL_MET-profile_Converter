/** FILE NAME: layer.c
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

  The layer program is used in the generation of layer output, that is, 
  for any defined atmospheric structure of heights and layers.  It produces values
  for layers (weighted or integrated mean) defined by the levels in the zh array.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int layer(float zmax, int size, int htsize, float *zh, float *value, 
          float *lev_value, float *lay_value, float *z)
{ 
 
  int ind, i=0, j;
  float tempval[size];
  float tempz[size];
  float mean[size];
  float sum; 
 

  while(i < htsize-1 && zh[i+1] <= zmax)   /* Find layer means.*/
    {
      i++;
      ind = 0;
      sum = 0;
      tempval[ind] = lev_value[i-1];    /* Lower boundary value*/
      tempz[ind] = zh[i-1];
      for(j=0;j<size;j++)
        {
          if(z[j] < zh[i] && z[j] > zh[i-1])  /* Values within layer.*/
            {
              ind++;
              tempval[ind] = value[j];
              tempz[ind] = z[j];
            }
        }
      ind++;                               /* Upper boundary level.*/
      tempval[ind] = lev_value[i];
      tempz[ind] = zh[i];

      for(j=1;j<ind+1;j++)                /* Sub-layer average.*/
        {
          mean[j-1] = (tempval[j] + tempval[j-1]) * 0.5;
        }

      for(j=1;j<ind+1;j++)              /* Proportional weighting of each layer.*/
        {
          sum += mean[j-1] * (tempz[j] - tempz[j-1]);
        }

      
      lay_value[i-1] = sum /(zh[i] - zh[i-1]); /* Mean layer value.*/
      
    }    /* end of while loop.*/

    /*for(i=1;i<htsize;i++)
     {
       printf("i = %3d,  zh[i] = %7.1f,  lay_value[i-1] = %7.1f\n", i, zh[i], lay_value[i-1]);
     }
    printf("\n");*/
 
   return;

} /* End of layer function. #####################################*/

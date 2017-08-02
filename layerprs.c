/** FILE NAME: layerprs.c
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

  The layerprs program is used in the generation of a defined atmospheric structure of pressures and pressure
  layers.  It produces values for layers defined by the levels in the pl array.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int layerprs(float pmin, int size, int psize, float *pl, float *value, 
          float *lev_value, float *lay_value, float *p)
{ 
 
  int ind, i=0, j;
  float *tempval;
  float *tempp;
  float *mean;
  float sum; 

  tempval = (float *)malloc(sizeof(float)*size); 
  tempp = (float *)malloc(sizeof(float)*size);   
  mean = (float *)malloc(sizeof(float)*size); 
 

  while(i < psize-1 && pl[i+1] >= pmin)   /* Find layer means.*/
    {
      i++;
      ind = 0;
      sum = 0;

      tempval[ind] = lev_value[i-1];    /* Lower boundary value*/
      tempp[ind] = pl[i-1];                

      for(j=0;j<size;j++)
        {
          if(p[j] > pl[i] && p[j] < pl[i-1])  /* Values within layer.*/
            {
              ind++;
              tempval[ind] = value[j];
              tempp[ind] = p[j];
            }
        }
      ind++;                               /* Upper boundary level.*/
      tempval[ind] = lev_value[i];
      tempp[ind] = pl[i];                  

      for(j=1;j<ind+1;j++)                /* Sub-layer average.*/
        {
          mean[j-1] = (tempval[j] + tempval[j-1]) * 0.5;
        }

      for(j=1;j<ind+1;j++)              /* Proportional weighting of each layer.*/
        {
          sum += mean[j-1] * (tempp[j-1] - tempp[j]); 
        }

      
      lay_value[i-1] = sum /(pl[i-1] - pl[i]); /* Mean layer value.*/  
      
    }    /* end of while loop.*/

    /*for(i=1;i<psize;i++)
     {
       printf("i = %3d,  pl[i] = %7.1f,  lay_value[i-1] = %7.1f\n", i, pl[i], lay_value[i-1]);
     }
    printf("\n\n");*/

   /* Free temporary arrays. ******/

   free(tempval);
   free(tempp);
   free(mean);
 
   return;

} /* End of layer function. #####################################*/

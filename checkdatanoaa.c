/** FILE NAME: checkdatanoaa.c
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

    Use to check input and intermediate data via writing file in struct 
    sound format.  Use with RAOB data in the NOAA archive format.
*/

#include "convert.h"  

int checkdata(struct sound *data)
{
  FILE *fpout;  
  int i, count;
  
  
  if((fpout = fopen("datacheck", "w")) == NULL)
    {
      printf("Can't open datacheck.  Exiting program.\n");
      exit(1);
    }  

  /* Write data to be checked to the file datacheck. ***********/

  printf("In checkdata: data->nht = %4d\n", data->nht); 
  fprintf(fpout,"%s\n%s  %s\n", data->site.filename, data->site.date, data->site.time);
  fprintf(fpout,"%4d %7.2f %7.2f %8.1f\n\n", data->nht, data->site.lat,
           data->site.lon, data->site.elev);
  fprintf(fpout,"    hgt    prs     tmp     dew    u       v       spd     dir\n\n");

  for(i=0;i<data->nht;i++) 
    {
      fprintf(fpout,"%7.0f %7.1f %7.1f %7.1f %7.1f %7.1f %7.0f %7.0f\n",
          data->level[i].hgt, data->level[i].prs, data->level[i].tmp, data->level[i].dew,
          data->level[i].u, data->level[i].v, data->level[i].spd, data->level[i].dir);
    }   
  count = i;
  fprintf(fpout,"\n");
  fclose(fpout);
  return(count);
} 

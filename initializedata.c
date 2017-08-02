/** FILE NAME: initializedata.c
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

   This function initializes the sounding structure, and any other structure
   of the form sound.  Initialized to the define constant ERROR (= -999.0), 
   or for integer the define constant IERROR (= -999).
*/

#include "convert.h"
initdata(struct sound *data)

{
   
 int j;
 
 data->nht=0;
 strcpy(data->site.time_id,"UT");
 data->site.lat = ERROR;
 data->site.lon = ERROR;
 data->site.ceil = ERROR;
 data->site.vis = ERROR;
 for( j=0 ; j<MAXSIZE; j++) 
  {        
    data->level[j].hgt = ERROR;
    data->level[j].prs = ERROR;
    data->level[j].tmp = ERROR;
    data->level[j].vtmp = ERROR;
    data->level[j].hum = ERROR;
    data->level[j].spd = ERROR;
    data->level[j].dir = ERROR;
    data->level[j].dew = ERROR; 
    data->level[j].u = ERROR;
    data->level[j].v = ERROR;
    data->level[j].lvlnum = IERROR;
    strcpy(data->level[j].htype, "XXX");
   }

} /* end of initdata */

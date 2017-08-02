/* FILE NAME: tvfromtemp.c
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

   Uses standard equations.  Converts sensible temperature and relative humidity into virtual temperature. 
   Units: tv and t in K; h in %; p in hPa. 
*/

#include "convert.h"

double tvfromtemp(double t, double p, double h)
{
	 double es, r, tv;

	 es = 6.108 * exp(17.27 * (t - 273.16)/(t - 35.86));
	 r = 0.622 * es/(p - es) * h * 0.01;
	 tv = t * (1 + r/.622)/(1 + r);
	 return(tv);
}


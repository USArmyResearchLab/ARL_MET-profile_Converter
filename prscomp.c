/** FILE NAME: prscomp.c
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

   This function uses standard equations to compute pressure at the top of a layer
   given the pressure at the bottom of the layer (p0), the virtual 
   temperatures at the top and bottom of the layer, and the heights at
   the top and bottom of the layer.
*/

#include "convert.h"

double presscomp(double tv, double tv0, double p0, double z, double z0)

  {
     double p, meantv;

     meantv = (tv + tv0)/2;
     p = p0 * exp(-G/(R*meantv) * (z - z0));

     return(p);
  }

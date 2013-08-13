/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_SCALENORMALISE__
#define __PV_SCALENORMALISE__

#include "PVModel.h"
#include "PVMath.h"

/**
* Scale Normalise class.
* Moves points to unit vector space.
*/
class PVScaleNormalise
{
public:

	/**
	* Default Constructor.
	*/
  PVScaleNormalise ()
  {
  };

   /**
   * Invokation method to operate on model.
   */
  void perform (PVModel *) throw (PVException &);
};


#endif /*  */

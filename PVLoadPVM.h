/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_LOADPVM__
#define __PV_LOADPVM__

#include "PVModel.h"
#include "PVModelFile.h"

/**
* PVM Importer class.
* Plugin used to deal with proprietry PointViewers Model format.
*/

class PVLoadPVM//:public PVImporter
{
public:

  /*
  * Default constructor.
  */
  
  PVLoadPVM ()
  {
  };

  /**
  * Invokation method to operate on model.
  */

  void perform (PVModel *, const char*) throw (PVException &);


};


#endif /*  */

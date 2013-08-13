/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_LOADASCII__
#define __PV_LOADASCII__

//#include "PVCommand.h"

#include <fstream>
using namespace std;

#include "PVModel.h"

/**
* ASCII Importer class.
* Plugin used to deal with a variety of Ascii based formats.
* Primarily XYZ files with 3 or 6 (colour or normals) fields.
*/

class PVLoadAscii//:public PVCommand
{
public:
  /**
  * Default constructor.
  */
  PVLoadAscii ()
  {
  };

   /**
   * Invokation method to operate on model.
   */
  void perform (PVModel*, const char*) throw (PVException &);
};


#endif /*  */

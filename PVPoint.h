/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_POINT__
#define __PV_POINT__

//#include "PVColour.h"

/**
* Point Structure.
* Created as such for minimal memory overhead.
* Stores a collection of vectors of various types.
* NOTE: float is enough accuracy!
*/

class PVPoint // PVVector*
{
public:
  float x;
  float y;
  float z;
  float r;
  float g;
  float b;
  float nx;
  float ny;
  float nz;

  //PVVector*
  //PVColour* colour;

	//PVPoint(int pv_nvect) { } // Allocate array of float

	//inline setPosition(float,float,float) { }

	//inline setColour(float,float,float) {}
};

#endif

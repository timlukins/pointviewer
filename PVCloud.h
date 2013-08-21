/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_CLOUD__
#define __PV_CLOUD__

#include <vector>
#include <iostream>
#include <map>
#include <cstring>
#include <climits>
#include <cstdlib>
using namespace std;

#include "PVPoint.h"
#include "PVException.h"
/**
* Cloud Class.
* Container for points.
* Responsible for memory management and organisation.
*/
class PVCloud
{
public:
  /**
  * Standard Constructor.
  * Passed and initial allocation space of points.
  * May be shrunk afterwards.
  * @see resize
  */
  PVCloud (unsigned long) throw (PVException &);

  /**
  * Destructor.
  * Ultimately responsible for reclaiming memory.
  */
  virtual ~ PVCloud ();

  /**
  * Pointer to raw array of pointers to points.
  * Allows direct access through data[p].x, data[p].y, etc.
  */
  PVPoint **data;

  // PVPointTree** tree; //TODO: hierarchical organisation of cloud in global space (for better rendering techniques)

  /**
  * Record of total number of points in cloud.
  */
  unsigned long npoints;

  /**
  * Record of current point defined.
  */
  unsigned long atpoint;

	/**
	* Record of bytes used
	*/

	// unsigned long bytes

  /**
  * Create new point with just x,y and z defined.
  * Colour is created from current atcloud value.
  */
  void newPoint (double, double, double) throw (PVException &);

  /**
  * Create new point with x,y,z and colour r,g,b defined.
  */
  void newPoint (double, double, double, float, float,
		 float) throw (PVException &);

  /**
  * Create new point with all x,y,z,r,g,b,nx,ny,nz defined.
  */
  void newPoint (double, double, double, float, float, float, double,
		 double, double) throw (PVException &);

	/**
	* PROPER USAGE!
	*/

  //inline PVPoint* newPoint(int pv_nvectors) { return new PVPoint(pv_nvectors); }

  /**
  * More safe access to points.
  */
  PVPoint *operator[] (unsigned long) throw (PVException &);

  /**
  * Memory grow/shrink method on demand.
  * @param oldsize The old number of points (usually npoints).
  * @param newsize The new size (can be less or more).
  */
  void resize (unsigned long, unsigned long) throw (PVException &);

  /**
  * Get Colour.
  */

  //static PVColour* getColour(float,float,float);

private:

  /**
  * Static record of colours used be all clouds.
  */

	//  typedef map<string,PVColour*> ColourMap;
  //static ColourMap sv_colourmap;

  /**
  * Record if points are being auto-coloured.
  */

  //bool av_autocolour;

  /**
  * Total number of times autocolouring has occured!
  */

  //static long av_autocoloured;

};


#endif

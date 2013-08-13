/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_MODEL__
#define __PV_MODEL__

#include <vector>
#include <iostream>
#include <string>
using namespace std;

#include "PVCloud.h"
#include "PVException.h"

/**
* Model Class.
* Container of all data stored in clouds.
* Also has selection model for specifying clouds.
*/

class PVModel
{

public:

  typedef vector <PVCloud*>Clouds;
  typedef vector <PVCloud*>::iterator Clouds_iter;
  Clouds				av_clouds;
  unsigned long av_current;
  Clouds_iter		av_atcloud;

  // TODO: total records? e.g. total number of points/memory used?

public:

	/**
	* Default constructor.
	*/
  
	PVModel ();

	/**
	* Destructor.
	* Responsible for calling delete on all clouds!
	*/
  
	virtual ~ PVModel ();

  // Creation 

	/**
	* Create new cloud with no initial points allocated.
	*/
  
	PVCloud *newCloud () throw (PVException &);

	/**
	* Create new cloud with specified inital space for points.
	*/
  
	PVCloud *newCloud (unsigned long) throw (PVException &);

	/**
	* End cloud - so that memory can be reclaimed.
	*/
  
	void endCloud (PVCloud *) throw (PVException &);

  // Iteration

	/**
	* Reset current iteration of clouds.
	* Throws exception if no clouds defined.
	*/
  
	PVCloud *firstCloud () throw (PVException &);

	/**
	* Get the next cloud in iteration
	* Throws exception if at end.
	*/
  
	PVCloud *nextCloud () throw (PVException &);

  /**
  * Return current "selected" cloud.
  */

  PVCloud* current() throw (PVException&);

  /**
  * Step to next selection.
  */

  void stepForwards() throw (PVException&); 

  /**
  * Step to previous selection.
  */

  void stepBackwards() throw (PVException&);

};


#endif

/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_EXCEPTION__
#define __PV_EXCEPTION__

#include <string>
#include <iostream>
#include <stdexcept>		// TODO: use more of this!
using namespace std;

// TODO: so much more could be handled here!
// e.g. "levels" of severity, debug info, etc...

/** 
* Exception class.
* Standard wrapper for all exceptions in pointviewer.
*/

class PVException		// TODO: would be better if this inherited from std::runtime_error
{
public:
	/**
	* Record of the origin of this exception.
	*/
	string from;		     /**< Description of generator of exception. */

	/**
	* Record of nature of this exception.
	*/
	string msg;		     /**< Message passed from generator describing exception. */

	/**
	* Default Constructor.
	* Sets the generator and message to empty.
	*/
	PVException ()
	{
		from = "";
		msg = "";
	}
	/**
	* Default Destructor.
	*/ virtual ~ PVException ()
	{
	}
	/**
	* Standard Constructor.
	* Usually invoked with full information on the nature of the exception.
	* @param pv_f From description.
	* @param pv_m Message.
	* @see PVException()
	*/
	PVException (string pv_f, string pv_m)
	{
		from = pv_f;
		msg = pv_m;
	}
};


#endif


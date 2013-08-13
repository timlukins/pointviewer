/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_COMMAND__
#define __PV_COMMAND__

#include <string>
using namespace std;

#include "PVModel.h"

/**
*
* Ultimately, this could be implemented for dynamic plugins
* loaded with a call to SDL_LoadObject and SDL_LoadFunction.
*/

class PVCommand
{
public:

	/**
	* The text for the command (i.e. what needs to be typed to call it.)
	*/

	virtual string text() = 0;

	/**
	* The suggest hot-key to run the command (order loaded resolves clashes).
	*/

	virtual char key() = 0;

	/**
	* The help text for this command (if any).
	*/

	virtual string help() = 0;

	/**
	* The number/type of args accepted.
	*/

	virtual int args() = 0;

	/**
	* Invoke - actually do what it does.
	* Given, constructor has provided pointers to data.
	*/

	virtual void perform() = 0;

};

#endif


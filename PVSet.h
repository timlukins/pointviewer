/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_SET__
#define __PV_SET__

#include <string>
using namespace std;

#include "PVCommand.h"
#include "PVState.h"

/**
* Generic "set" command to alter state.
* e.g. 'set axis on'
*/

class PVSet : public PVCommand
{
private:

	PVState*									ap_state;

public:
	
	PVSet(PVState* pp_state)	{ ap_state = pp_state; }

	virtual ~PVSet()					{}

	inline string text()			{ return "set"; }

	inline char key()					{ return '\0'; } // Needs args - no default

	inline string help()			{ return "set <state variable> <value>"; }

	inline int args()					{ return 2; }

	inline void perform()
	{


	}
};

#endif


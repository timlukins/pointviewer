/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_TRANSLATECENTER__
#define __PV_TRANSLATECENTER__

#include <string>
using namespace std;

#include "PVCommand.h"
#include "PVState.h"
#include "PVModel.h"
#include "PVMath.h"

/**
* Generic "center" command
*/

class PVTranslateCenter : public PVCommand
{
private:

	PVState*									ap_state;
  PVModel*                  ap_model;

public:
	
	PVTranslateCenter(PVState* pp_state, PVModel* pp_model)	{ ap_state = pp_state; ap_model = pp_model; }

	virtual ~PVTranslateCenter()					{}

	inline string text()			{ return "center"; }

	inline char key()					{ return 'c'; } // Needs args - no default

	inline string help()			{ return "center [move origin to centroid of selected/first cloud]"; }

	inline int args()					{ return 0; }

        void perform();
    
};

#endif


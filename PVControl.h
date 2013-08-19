/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_CONTROL__
#define __PV_CONTROL__

// The one place we use a define for version?
#define PV_TITLE ". . . p o i n t v i e w e r . . ."
//#define PV_COPYR "(c) 2005 IPAB Research, University of Edinburgh."

#include <vector>
using namespace std;

// NOTE: we use SDL here to provide cross platform interface.
#include "SDL.h"
#include "PVModel.h"
#include "PVException.h"
#include "PVState.h"
#include "PVThread.h"
#include "PVCommand.h"
#include "PVModelFile.h"

#include "PVMath.h" // Temporary resize points

#include "PVSet.h"
#include "PVLoadAscii.h"
#include "PVLoadPVM.h"
#include "PVTranslateCenter.h"
#include "PVScaleNormalise.h"

//#include "PVImportAscii.h"
//#include "PVTranslateCentroid.h"
//#include "PVScaleNormalise.h"
//#include "PVImportC3D.h"
//#include "PVImportPVM.h"

/**
* Control Class.
* Central point of control, responsible for presenting an interface to the user
* and handling all callbacks as appropriate.
*/

class PVControl : public PVThread
{
private:

	SDL_Surface*				ap_screen;
	PVState*						ap_state;
	PVModel*						ap_model;
	vector<PVCommand*>	ap_commands;
	// PVTerminal
	//PVView* ap_view;
	//int av_mode; // VISUAL||TERMINAL

public:

	/**
	* Standard Constructor.
	* Passed the View and Model to control.
	*/

  PVControl(PVState*,PVModel*);

	/**
	* Default destructor.
	*/

  virtual ~ PVControl ();

	/**
	* Parse for command line options.
	*/

	void init(int, char **) throw (PVException&);

	/**
	* Ready create actual window to run in.
	*/

	void ready() throw (PVException&);

	/**
	* Start event management main loop.
	*/

  void run ();

};

#endif


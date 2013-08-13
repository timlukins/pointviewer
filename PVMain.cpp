/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVControl.h"
#include "PVModel.h"
#include "PVView.h"
#include "PVState.h"
#include "PVException.h"

/** @file
* Entry point for pointviewer program.
*/

int
main (int pv_argc, char* pp_argv[])
{
	// Straightforward MVC framework...

	PVState* lp_state;  // 
	PVControl* lp_control;
	PVView* lp_view;
	PVModel *lp_model;

	try
	{
		// Create a model...

		lp_model = new PVModel ();

		// Create settings...

		lp_state = new PVState();

		// Create a controller window to update settings and model... 

		lp_control = new PVControl(lp_state,lp_model);

		// Init it with the command line to set state and load model...

		lp_control->init(pv_argc, pp_argv);
		
		// Ready control by creating window...

		lp_control->ready();

		// Create a view, determined by the settings and model...

		lp_view = new PVView(lp_state,lp_model); 

		// Init OpenGL...

		lp_view->init();

		// Ready and run...
		// TODO: one of these days calling ->start() on the view thread will work with single OpenGL context... 

		while(lp_control->running()) {
			lp_control->run();
			lp_view->run();
		}	

	} 
	catch(PVException& er_prob)
	{
		// Something gone wrong with init...

		cerr << er_prob.from << ":" << er_prob.msg << endl;
		return 1;
	}

	// Clean up...

	delete (lp_view);
	delete (lp_control);
	delete (lp_state);
	delete (lp_model);

	// Return...

	return 0;
}

/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_VIEW__
#define __PV_VIEW__

#ifdef _WIN32
#include <windows.h>		// Need this first for OpenGL support on Window$
#endif

#ifdef HAVE_GL_GL_H
# include <GL/gl.h>
#elif defined(HAVE_OPENGL_GL_H)
# include <OpenGL/gl.h>
#else
# error no gl.h
#endif

#include <string>
using namespace std;

#include "PVState.h"
#include "PVModel.h"
#include "PVThread.h"
#include "PVMath.h"

/**
* View Class.
* Central class for actually rendering a model onto the screen.
* Uses only OpenGL (in the context of window managed by Control).
*/
class PVView : public PVThread
{
public:
	
	/**
	* Default Constructor.
	*/

	PVView (PVState*, PVModel*);
	
	/**
	* Desctructor.
	*/

	virtual ~ PVView ();

	/**
	* Init
	*/

	void init();

	// VARIOUS EVENT HANDLERS CALLED BY CONTROL

	/**
	* Draw model on screen.
	*/
	void draw (); // TODO: run on seperate thread!
	
	/**
	* Handle alterations due to window resizing.
	*/
	
	void reshape ();

	/**
	* Update thread.
	*/

	void run();

private: 

	PVModel * ap_model;
	PVState* ap_state;
};

#endif

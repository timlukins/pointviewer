/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_THREAD__
#define __PV_THREAD__

// Another big reason to use SDL - threads!
#include <SDL.h>
#include <SDL_thread.h>

class PVThread
{
public:

	static SDL_mutex* ap_mutex; // Generic shate mutex for all threads to see

	bool av_stop;

	PVThread();

	virtual ~PVThread();

	static int _run(void*);

	virtual void ready();
	virtual void run()=0;

	SDL_Thread* start();

	void stop();

	static void wait(SDL_Thread*);

	bool running();

	void lock();   // Take control over shared mutex -> control all other threads access
	void unlock();

};

#endif


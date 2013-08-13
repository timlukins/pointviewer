/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVThread.h"

SDL_mutex* PVThread::ap_mutex = SDL_CreateMutex();

PVThread::PVThread()
{
	av_stop = false; // By default
}

PVThread::~PVThread() {}

int
PVThread::_run(void* pp_thread)
{
	PVThread* pp_this = reinterpret_cast<PVThread*>(pp_thread);
	pp_this->ready();
	while (!pp_this->av_stop) {
		pp_this->run();
	}
	return 0;
}

SDL_Thread*
PVThread::start()
{
	av_stop=false;
	return SDL_CreateThread(&PVThread::_run,this);
}


void
PVThread::wait(SDL_Thread* pp_thread)
{
	SDL_WaitThread(pp_thread, NULL);
}

void
PVThread::stop()
{
	av_stop=true;
}

bool
PVThread::running()
{
	return !av_stop;
}

void
PVThread::ready()
{
	// Nothing is done by default...
}

void 
PVThread::lock() 
{ 
	SDL_LockMutex(ap_mutex);
}
	
void
PVThread::unlock()
{ 
	SDL_UnlockMutex(ap_mutex);
}

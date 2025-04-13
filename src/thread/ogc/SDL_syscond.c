/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include <errno.h>
#include <ogc/cond.h>
#include <ogc/timesupp.h>

#include "SDL_thread.h"
#include "SDL_sysmutex_c.h"

struct SDL_cond {
	cond_t cond;
};

/* Create a condition variable */
SDL_cond *SDL_CreateCond(void)
{
	SDL_cond *cond;

	cond = (SDL_cond *) SDL_malloc(sizeof(SDL_cond));
	if ( cond ) {
		if ( LWP_CondInit(&cond->cond) != 0 ) {
			SDL_SetError("LWP_CondInit() failed");
			SDL_free(cond);
			cond = NULL;
		}
	} else {
		SDL_OutOfMemory();
	}
	return(cond);
}

/* Destroy a condition variable */
void SDL_DestroyCond(SDL_cond *cond)
{
	if ( cond ) {
		LWP_CondDestroy(cond->cond);
		SDL_free(cond);
	}
}

/* Restart one of the threads that are waiting on the condition variable */
int SDL_CondSignal(SDL_cond *cond)
{
	int retval;

	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	retval = 0;
	if ( LWP_CondSignal(cond->cond) != 0 ) {
		SDL_SetError("LWP_CondSignal() failed");
		retval = -1;
	}
	return retval;
}

/* Restart all threads that are waiting on the condition variable */
int SDL_CondBroadcast(SDL_cond *cond)
{
	int retval;

	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	retval = 0;
	if ( LWP_CondBroadcast(cond->cond) != 0 ) {
		SDL_SetError("LWP_CondBroadcast() failed");
		retval = -1;
	}
	return retval;
}

/* Wait on the condition variable for at most 'ms' milliseconds.
   The mutex must be locked before entering this function!
   The mutex is unlocked during the wait, and locked again after the wait.

Typical use:

Thread A:
	SDL_LockMutex(lock);
	while ( ! condition ) {
		SDL_CondWait(cond);
	}
	SDL_UnlockMutex(lock);

Thread B:
	SDL_LockMutex(lock);
	...
	condition = true;
	...
	SDL_UnlockMutex(lock);
 */
int SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, Uint32 ms)
{
	int retval;
	struct timespec tv;

	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	tv.tv_sec = ms / TB_MSPERSEC;
	tv.tv_nsec = (ms % TB_MSPERSEC) * TB_NSPERMS;

	retval = LWP_CondTimedWait(cond->cond, mutex->id, &tv);
	switch ( retval ) {
	    case ETIMEDOUT:
		retval = SDL_MUTEX_TIMEDOUT;
		break;
	    case 0:
		break;
	    default:
		SDL_SetError("LWP_CondTimedWait() failed");
		retval = -1;
		break;
	}
	return retval;
}

/* Wait on the condition variable, unlocking the provided mutex.
   The mutex must be locked before entering this function!
 */
int SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex)
{
	int retval;

	if ( ! cond ) {
		SDL_SetError("Passed a NULL condition variable");
		return -1;
	}

	retval = 0;
	if ( LWP_CondWait(cond->cond, mutex->id) != 0 ) {
		SDL_SetError("LWP_CondWait() failed");
		retval = -1;
	}
	return retval;
}

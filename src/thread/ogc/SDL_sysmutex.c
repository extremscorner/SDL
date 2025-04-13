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

#include <ogc/mutex.h>

#include "SDL_thread.h"

struct SDL_mutex {
	mutex_t id;
};

/* Create a mutex */
SDL_mutex *SDL_CreateMutex(void)
{
	SDL_mutex *mutex;

	mutex = (SDL_mutex *) SDL_malloc(sizeof(SDL_mutex));
	if ( mutex ) {
		if ( LWP_MutexInit(&mutex->id, true) != 0 ) {
			SDL_SetError("LWP_MutexInit() failed");
			SDL_free(mutex);
			mutex = NULL;
		}
	} else {
		SDL_OutOfMemory();
	}
	return(mutex);
}

/* Destroy a mutex */
void SDL_DestroyMutex(SDL_mutex *mutex)
{
	if ( mutex ) {
		LWP_MutexDestroy(mutex->id);
		SDL_free(mutex);
	}
}

/* Lock the mutex */
int SDL_mutexP(SDL_mutex *mutex)
{
	int retval;

	if ( ! mutex ) {
		SDL_SetError("Passed a NULL mutex");
		return -1;
	}

	retval = 0;
	if ( LWP_MutexLock(mutex->id) != 0 ) {
		SDL_SetError("LWP_MutexLock() failed");
		retval = -1;
	}
	return retval;
}

/* Unlock the mutex */
int SDL_mutexV(SDL_mutex *mutex)
{
	int retval;

	if ( ! mutex ) {
		SDL_SetError("Passed a NULL mutex");
		return -1;
	}

	retval = 0;
	if ( LWP_MutexUnlock(mutex->id) != 0 ) {
		SDL_SetError("LWP_MutexUnlock() failed");
		retval = -1;
	}
	return retval;
}

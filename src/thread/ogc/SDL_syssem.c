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
#include <ogc/semaphore.h>
#include <ogc/timesupp.h>

#include "SDL_thread.h"

struct SDL_semaphore {
	sem_t sem;
};

/* Create a semaphore, initialized with value */
SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
	SDL_sem *sem;

	sem = (SDL_sem *) SDL_malloc(sizeof(SDL_sem));
	if ( sem ) {
		if ( LWP_SemInit(&sem->sem, initial_value, 0xFFFFFFFF) != 0 ) {
			SDL_SetError("LWP_SemInit() failed");
			SDL_free(sem);
			sem = NULL;
		}
	} else {
		SDL_OutOfMemory();
	}
	return(sem);
}

/* Destroy a semaphore */
void SDL_DestroySemaphore(SDL_sem *sem)
{
	if ( sem ) {
		LWP_SemDestroy(sem->sem);
		SDL_free(sem);
	}
}

int SDL_SemTryWait(SDL_sem *sem)
{
	int retval;

	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	retval = 0;
	if ( LWP_SemTryWait(sem->sem) != 0 ) {
		retval = SDL_MUTEX_TIMEDOUT;
	}
	return retval;
}

int SDL_SemWait(SDL_sem *sem)
{
	int retval;

	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	retval = 0;
	if ( LWP_SemWait(sem->sem) != 0 ) {
		SDL_SetError("LWP_SemWait() failed");
		retval = -1;
	}
	return retval;
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 ms)
{
	int retval;
	struct timespec tv;

	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	/* Try the easy cases first */
	if ( ms == 0 ) {
		return SDL_SemTryWait(sem);
	}
	if ( ms == SDL_MUTEX_MAXWAIT ) {
		return SDL_SemWait(sem);
	}

	tv.tv_sec = ms / TB_MSPERSEC;
	tv.tv_nsec = (ms % TB_MSPERSEC) * TB_NSPERMS;

	retval = LWP_SemTimedWait(sem->sem, &tv);
	switch ( retval ) {
	    case ETIMEDOUT:
		retval = SDL_MUTEX_TIMEDOUT;
		break;
	    case 0:
		break;
	    default:
		SDL_SetError("LWP_SemTimedWait() failed");
		retval = -1;
		break;
	}
	return retval;
}

/* Returns the current count of the semaphore */
Uint32 SDL_SemValue(SDL_sem *sem)
{
	Uint32 value;

	value = 0;
	if ( sem ) {
		LWP_SemGetValue(sem->sem, &value);
	}
	return value;
}

/* Atomically increases the semaphore's count (not blocking) */
int SDL_SemPost(SDL_sem *sem)
{
	int retval;

	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	retval = 0;
	if ( LWP_SemPost(sem->sem) != 0 ) {
		SDL_SetError("LWP_SemPost() failed");
		retval = -1;
	}
	return retval;
}

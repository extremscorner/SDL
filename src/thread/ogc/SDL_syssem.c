/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#include <errno.h>
#include <ogc/semaphore.h>
#include <ogc/timesupp.h>

#include "SDL_thread.h"

struct SDL_semaphore
{
    sem_t sem;
};

/* Create a semaphore, initialized with value */
SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
    SDL_sem *sem;

    sem = (SDL_sem *)SDL_malloc(sizeof(SDL_sem));
    if (sem != NULL) {
        if (LWP_SemInit(&sem->sem, initial_value, SDL_MAX_UINT32) != 0) {
            SDL_SetError("LWP_SemInit() failed");
            SDL_free(sem);
            sem = NULL;
        }
    } else {
        SDL_OutOfMemory();
    }
    return sem;
}

/* Destroy a semaphore */
void SDL_DestroySemaphore(SDL_sem *sem)
{
    if (sem != NULL) {
        LWP_SemDestroy(sem->sem);
        SDL_free(sem);
    }
}

int SDL_SemTryWait(SDL_sem *sem)
{
    if (sem == NULL) {
        return SDL_InvalidParamError("sem");
    }

    if (LWP_SemTryWait(sem->sem) != 0) {
        return SDL_MUTEX_TIMEDOUT;
    }
    return 0;
}

int SDL_SemWait(SDL_sem *sem)
{
    if (sem == NULL) {
        return SDL_InvalidParamError("sem");
    }

    if (LWP_SemWait(sem->sem) != 0) {
        return SDL_SetError("LWP_SemWait() failed");
    }
    return 0;
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 ms)
{
    int retval;
    struct timespec tv;

    if (sem == NULL) {
        return SDL_InvalidParamError("sem");
    }

    /* Try the easy cases first */
    if (ms == 0) {
        return SDL_SemTryWait(sem);
    }
    if (ms == SDL_MUTEX_MAXWAIT) {
        return SDL_SemWait(sem);
    }

    tv.tv_sec = ms / TB_MSPERSEC;
    tv.tv_nsec = (ms % TB_MSPERSEC) * TB_NSPERMS;

    retval = LWP_SemTimedWait(sem->sem, &tv);
    if (retval != 0) {
        if (retval == ETIMEDOUT) {
            retval = SDL_MUTEX_TIMEDOUT;
        } else {
            retval = SDL_SetError("LWP_SemTimedWait() failed");
        }
    }
    return retval;
}

/* Returns the current count of the semaphore */
Uint32 SDL_SemValue(SDL_sem *sem)
{
    Uint32 value;

    if (sem == NULL) {
        SDL_InvalidParamError("sem");
        return 0;
    }

    value = 0;
    LWP_SemGetValue(sem->sem, &value);
    return value;
}

/* Atomically increases the semaphore's count (not blocking) */
int SDL_SemPost(SDL_sem *sem)
{
    if (sem == NULL) {
        return SDL_InvalidParamError("sem");
    }

    if (LWP_SemPost(sem->sem) != 0) {
        return SDL_SetError("LWP_SemPost() failed");
    }
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */

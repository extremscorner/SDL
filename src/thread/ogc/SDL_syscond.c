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
#include <ogc/cond.h>
#include <ogc/timesupp.h>

#include "SDL_thread.h"
#include "SDL_sysmutex_c.h"

struct SDL_cond
{
    cond_t cond;
};

/* Create a condition variable */
SDL_cond *SDL_CreateCond(void)
{
    SDL_cond *cond;

    cond = (SDL_cond *)SDL_malloc(sizeof(SDL_cond));
    if (cond != NULL) {
        if (LWP_CondInit(&cond->cond) != 0) {
            SDL_SetError("LWP_CondInit() failed");
            SDL_free(cond);
            cond = NULL;
        }
    } else {
        SDL_OutOfMemory();
    }
    return cond;
}

/* Destroy a condition variable */
void SDL_DestroyCond(SDL_cond *cond)
{
    if (cond != NULL) {
        LWP_CondDestroy(cond->cond);
        SDL_free(cond);
    }
}

/* Restart one of the threads that are waiting on the condition variable */
int SDL_CondSignal(SDL_cond *cond)
{
    if (cond == NULL) {
        return SDL_InvalidParamError("cond");
    }

    if (LWP_CondSignal(cond->cond) != 0) {
        return SDL_SetError("LWP_CondSignal() failed");
    }
    return 0;
}

/* Restart all threads that are waiting on the condition variable */
int SDL_CondBroadcast(SDL_cond *cond)
{
    if (cond == NULL) {
        return SDL_InvalidParamError("cond");
    }

    if (LWP_CondBroadcast(cond->cond) != 0) {
        return SDL_SetError("LWP_CondBroadcast() failed");
    }
    return 0;
}

/* Wait on the condition variable for at most 'ms' milliseconds.
   The mutex must be locked before entering this function!
   The mutex is unlocked during the wait, and locked again after the wait.

Typical use:

Thread A:
    SDL_LockMutex(lock);
    while ( ! condition ) {
        SDL_CondWait(cond, lock);
    }
    SDL_UnlockMutex(lock);

Thread B:
    SDL_LockMutex(lock);
    ...
    condition = true;
    ...
    SDL_CondSignal(cond);
    SDL_UnlockMutex(lock);
 */
int SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, Uint32 ms)
{
    int retval;
    struct timespec tv;

    if (cond == NULL) {
        return SDL_InvalidParamError("cond");
    }
    if (mutex == NULL) {
        return SDL_InvalidParamError("mutex");
    }

    tv.tv_sec = ms / TB_MSPERSEC;
    tv.tv_nsec = (ms % TB_MSPERSEC) * TB_NSPERMS;

    retval = LWP_CondTimedWait(cond->cond, mutex->id, &tv);
    if (retval != 0) {
        if (retval == ETIMEDOUT) {
            retval = SDL_MUTEX_TIMEDOUT;
        } else {
            retval = SDL_SetError("LWP_CondTimedWait() failed");
        }
    }
    return retval;
}

/* Wait on the condition variable, unlocking the provided mutex.
   The mutex must be locked before entering this function!
 */
int SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex)
{
    if (cond == NULL) {
        return SDL_InvalidParamError("cond");
    }
    if (mutex == NULL) {
        return SDL_InvalidParamError("mutex");
    }

    if (LWP_CondWait(cond->cond, mutex->id) != 0) {
        return SDL_SetError("LWP_CondWait() failed");
    }
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */

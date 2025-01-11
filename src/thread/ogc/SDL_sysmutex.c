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
#include <ogc/mutex.h>

#include "SDL_thread.h"

struct SDL_mutex
{
    mutex_t id;
};

/* Create a mutex */
SDL_mutex *SDL_CreateMutex(void)
{
    SDL_mutex *mutex;

    mutex = (SDL_mutex *)SDL_malloc(sizeof(SDL_mutex));
    if (mutex != NULL) {
        if (LWP_MutexInit(&mutex->id, true) != 0) {
            SDL_SetError("LWP_MutexInit() failed");
            SDL_free(mutex);
            mutex = NULL;
        }
    } else {
        SDL_OutOfMemory();
    }
    return mutex;
}

/* Destroy a mutex */
void SDL_DestroyMutex(SDL_mutex *mutex)
{
    if (mutex != NULL) {
        LWP_MutexDestroy(mutex->id);
        SDL_free(mutex);
    }
}

/* Lock the mutex */
int SDL_LockMutex(SDL_mutex *mutex) SDL_NO_THREAD_SAFETY_ANALYSIS /* clang doesn't know about NULL mutexes */
{
    if (mutex == NULL) {
        return 0;
    }

    if (LWP_MutexLock(mutex->id) != 0) {
        return SDL_SetError("LWP_MutexLock() failed");
    }
    return 0;
}

/* try Lock the mutex */
int SDL_TryLockMutex(SDL_mutex *mutex)
{
    int retval;

    if (mutex == NULL) {
        return 0;
    }

    retval = LWP_MutexTryLock(mutex->id);
    if (retval != 0) {
        if (retval == EBUSY) {
            retval = SDL_MUTEX_TIMEDOUT;
        } else {
            retval = SDL_SetError("LWP_MutexTryLock() failed");
        }
    }
    return retval;
}

/* Unlock the mutex */
int SDL_UnlockMutex(SDL_mutex *mutex) SDL_NO_THREAD_SAFETY_ANALYSIS /* clang doesn't know about NULL mutexes */
{
    if (mutex == NULL) {
        return 0;
    }

    if (LWP_MutexUnlock(mutex->id) != 0) {
        return SDL_SetError("LWP_MutexUnlock() failed");
    }
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */

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

#include <ogc/lwp.h>

#include "SDL_thread.h"
#include "../SDL_thread_c.h"
#include "../SDL_systhread.h"

static void *RunThread(void *data)
{
    SDL_RunThread((SDL_Thread *)data);
    return NULL;
}

int SDL_SYS_CreateThread(SDL_Thread *thread)
{
    int priority = LWP_GetThreadPriority(LWP_THREAD_NULL);

    if (thread->stacksize == 0) {
        thread->stacksize = LWP_GetThreadStackSize(LWP_THREAD_NULL);
    }

    /* Create the thread and go! */
    if (LWP_CreateThread(&thread->handle, RunThread, thread, NULL, thread->stacksize, priority) != 0) {
        return SDL_SetError("Not enough resources to create thread");
    }
    return 0;
}

void SDL_SYS_SetupThread(const char *name)
{
    return;
}

SDL_threadID SDL_ThreadID(void)
{
    return (SDL_threadID)LWP_GetSelf();
}

int SDL_SYS_SetThreadPriority(SDL_ThreadPriority priority)
{
    int value;

    /* Range is 1 (lowest) to 127 (highest) */
    if (priority == SDL_THREAD_PRIORITY_LOW) {
        value = LWP_PRIO_LOWEST;
    } else if (priority == SDL_THREAD_PRIORITY_HIGH) {
        value = LWP_PRIO_HIGHEST;
    } else if (priority == SDL_THREAD_PRIORITY_TIME_CRITICAL) {
        value = LWP_PRIO_TIME_CRITICAL;
    } else {
        value = LWP_PRIO_NORMAL;
    }
    if (LWP_SetThreadPriority(LWP_THREAD_NULL, value) < 0) {
        return SDL_SetError("LWP_SetThreadPriority() failed");
    }
    return 0;
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
    LWP_JoinThread(thread->handle, NULL);
}

void SDL_SYS_DetachThread(SDL_Thread *thread)
{
    LWP_DetachThread(thread->handle);
}

/* vi: set ts=4 sw=4 expandtab: */

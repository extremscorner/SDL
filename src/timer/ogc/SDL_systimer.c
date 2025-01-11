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

#ifdef SDL_TIMER_OGC

#include <ogc/timesupp.h>
#include <ogcsys.h>

static SDL_bool ticks_started = SDL_FALSE;
static u64 start;

void SDL_TicksInit(void)
{
    if (ticks_started) {
        return;
    }
    ticks_started = SDL_TRUE;

    start = SYS_GetSystemTime();
}

void SDL_TicksQuit(void)
{
    ticks_started = SDL_FALSE;
}

Uint64 SDL_GetTicks64(void)
{
    const u64 ticks = SYS_GetSystemTime() - start;
    const u64 ms = ticks / TB_TIMER_CLOCK;
    return ms;
}

Uint64 SDL_GetPerformanceCounter(void)
{
    return SYS_GetSystemTime();
}

Uint64 SDL_GetPerformanceFrequency(void)
{
    return secs_to_ticks(1);
}

void SDL_Delay(Uint32 ms)
{
    struct timespec tv;
    tv.tv_sec = ms / TB_MSPERSEC;
    tv.tv_nsec = (ms % TB_MSPERSEC) * TB_NSPERMS;
    nanosleep(&tv, NULL);
}

#endif /* SDL_TIMER_OGC */

/* vi: set sts=4 ts=4 sw=4 expandtab: */

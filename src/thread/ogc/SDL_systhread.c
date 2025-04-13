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

#include <ogc/lwp.h>

#include "SDL_thread.h"
#include "../SDL_thread_c.h"
#include "../SDL_systhread.h"

static void *RunThread(void *data)
{
	SDL_RunThread(data);
	return(NULL);
}

int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	size_t stacksize = LWP_GetThreadStackSize(LWP_THREAD_NULL);

	/* Create the thread and go! */
	if ( LWP_CreateThread(&thread->handle, RunThread, args, NULL, stacksize, LWP_PRIO_NORMAL) != 0 ) {
		SDL_SetError("Not enough resources to create thread");
		return(-1);
	}
	return(0);
}

void SDL_SYS_SetupThread(void)
{
	return;
}

Uint32 SDL_ThreadID(void)
{
	return((Uint32)LWP_GetSelf());
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	LWP_JoinThread(thread->handle, NULL);
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	return;
}

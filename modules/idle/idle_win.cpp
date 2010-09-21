/*
 * idle_win.cpp - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "idle.h"

#include <qlibrary.h>
#include <windows.h>

#if defined(Q_OS_WIN32) && !defined(Q_CC_GNU) && (_WIN32_WINNT < 0x0500)
typedef struct tagLASTINPUTINFO
{
	UINT cbSize;
	DWORD dwTime;
} LASTINPUTINFO, *PLASTINPUTINFO;
#endif

BOOL (__stdcall * GetLastInputInfoFun)(PLASTINPUTINFO);
DWORD (__stdcall * IdleUIGetLastInputTime)(void);
QLibrary *lib;

Idle::Idle()
{
	void *p;

	GetLastInputInfoFun = 0;
	IdleUIGetLastInputTime = 0;
	lib = 0;

	if (lib == 0)
	{
		// try to find the built-in Windows 2000 function
		lib = new QLibrary("user32");
		if(lib->load() && (p = lib->resolve("GetLastInputInfo")))
		{
			GetLastInputInfoFun = (BOOL (__stdcall *)(PLASTINPUTINFO))p;
		}
		else
		{
			delete lib;

			// fall back on idleui
			lib = new QLibrary("idleui");
			if(lib->load() && (p = lib->resolve("IdleUIGetLastInputTime")))
			{
				IdleUIGetLastInputTime = (DWORD (__stdcall *)(void))p;
			}
			else
			{
				delete lib;
				lib = 0;
			}
		}
	}
}

Idle::~Idle()
{
	if (lib)
	{
		delete lib;
		lib = 0;
	}
}

int Idle::secondsIdle()
{
	int i;
	if (GetLastInputInfoFun != 0)
	{
		LASTINPUTINFO li;
		li.cbSize = sizeof(LASTINPUTINFO);
		bool ok = GetLastInputInfoFun(&li);
		if (!ok)
			return -1;
		i = li.dwTime;
	}
	else if (IdleUIGetLastInputTime)
	{
		i = IdleUIGetLastInputTime();
	}
	else
		return -1;

	return (GetTickCount() - i) / 1000;
}

bool Idle::isActive()
{
	return (secondsIdle() == 0);
}

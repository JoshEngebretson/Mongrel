//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: sys_win.cpp 4422 2011-05-17 00:00:56Z firebrand_kh $
//**
//**	Copyright (C) 1999-2006 Jānis Legzdiņš
//**
//**	This program is free software; you can redistribute it and/or
//**  modify it under the terms of the GNU General Public License
//**  as published by the Free Software Foundation; either version 2
//**  of the License, or (at your option) any later version.
//**
//**	This program is distributed in the hope that it will be useful,
//**  but WITHOUT ANY WARRANTY; without even the implied warranty of
//**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//**  GNU General Public License for more details.
//**
//**************************************************************************

// HEADER FILES ------------------------------------------------------------

#ifdef _MSC_VER
#include "winlocal.h"
#include "gamedefs.h"
#include <signal.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>

#include <SDL.h>

// SDL is defining main on windows, look into this
#undef main

// MACROS ------------------------------------------------------------------

#define R_OK	4

#define PAUSE_SLEEP		50				// sleep time on pause or minimization
#define NOT_FOCUS_SLEEP	20				// sleep time when not focus

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------
static HANDLE			dir_handle;
static WIN32_FIND_DATA	dir_buf;
static bool				dir_already_got;

static double		pfreq;
static double		curtime = 0.0;
static double		lastcurtime = 0.0;
static vuint32		oldtime;
static int			lowshift;

static HANDLE		tevent;

static bool			ActiveApp, Minimized;

static VCvarI		win_priority("win_priority", "0", CVAR_Archive);
static VCvarI		win_sys_keys("win_sys_keys", "1", CVAR_Archive);

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	Sys_FileExists
//
//==========================================================================

int Sys_FileExists(const VStr& filename)
{
#ifdef WIN32
	return !_access(*filename, R_OK);
#else
	return !access(*filename, R_OK);
#endif
}
//==========================================================================
//
//	Sys_FileTime
//
//	Returns -1 if not present
//
//==========================================================================

int	Sys_FileTime(const VStr& path)
{
	struct	stat	buf;
	
	if (stat(*path, &buf) == -1)
		return -1;

	return buf.st_mtime;
}

//==========================================================================
//
//	Sys_CreateDirectory
//
//==========================================================================

int Sys_CreateDirectory(const VStr& path)
{
#ifdef WIN32
	return _mkdir(*path);
#else
	return mkdir(*path);
#endif
}

//==========================================================================
//
//	Sys_OpenDir
//
//==========================================================================

int Sys_OpenDir(const VStr& dirname)
{
	dir_handle = FindFirstFile(va("%s/*.*", *dirname), &dir_buf);
	if (dir_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	dir_already_got = true;
	return true;
}

//==========================================================================
//
//	Sys_ReadDir
//
//==========================================================================

VStr Sys_ReadDir()
{
	if (!dir_already_got)
	{
		if (FindNextFile(dir_handle, &dir_buf) != TRUE)
		{
			return VStr();
		}
	}
	dir_already_got = false;
	return dir_buf.cFileName;
}

//==========================================================================
//
//	Sys_CloseDir
//
//==========================================================================

void Sys_CloseDir()
{
    FindClose(dir_handle);
}

//==========================================================================
//
//	Sys_DirExists
//
//==========================================================================

bool Sys_DirExists(const VStr& path)
{
	struct stat s;
	
	if (stat(*path, &s) == -1)
		return false;
	
	return !!(s.st_mode & S_IFDIR);
}

//**************************************************************************
//**
//**	TIME
//**
//**************************************************************************

//==========================================================================
//
//  Sys_Time
//
//==========================================================================

double Sys_Time()
{
	static int			sametimecount;
	LARGE_INTEGER		PerformanceCount;
	vuint32				temp, t2;
	double				time;

	QueryPerformanceCounter(&PerformanceCount);

	temp = ((unsigned int)PerformanceCount.u.LowPart >> lowshift) |
		   ((unsigned int)PerformanceCount.u.HighPart << (32 - lowshift));

	// check for turnover or backward time
	if ((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
	{
		oldtime = temp;	// so we can't get stuck
	}
	else
	{
		t2 = temp - oldtime;

		time = (double)t2 * pfreq;
		oldtime = temp;

		curtime += time;

		if (curtime == lastcurtime)
		{
			sametimecount++;

			if (sametimecount > 100000)
			{
				curtime += 1.0;
				sametimecount = 0;
			}
		}
		else
		{
			sametimecount = 0;
		}

		lastcurtime = curtime;
	}

	return curtime;
}

//==========================================================================
//
//	Sys_Sleep
//
//==========================================================================

void Sys_Sleep()
{
	Sleep(1);
}

//==========================================================================
//
//  Sys_InitTime
//
//==========================================================================

static void Sys_InitTime()
{
	LARGE_INTEGER	PerformanceFreq;
	LARGE_INTEGER	PerformanceCount;
	vuint32			lowpart, highpart;

	if (!QueryPerformanceFrequency(&PerformanceFreq))
		Sys_Error("No hardware timer available");

	// get 32 out of the 64 time bits such that we have around
	// 1 microsecond resolution
	lowpart = (vuint32)PerformanceFreq.u.LowPart;
	highpart = (vuint32)PerformanceFreq.u.HighPart;
	lowshift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;

	//	Read current time and set old time.
	QueryPerformanceCounter(&PerformanceCount);

	oldtime = ((vuint32)PerformanceCount.u.LowPart >> lowshift) |
			  ((vuint32)PerformanceCount.u.HighPart << (32 - lowshift));

	//	Set start time
	const char* p = GArgs.CheckValue("-starttime");

	if (p)
	{
		curtime = (double)atof(p);
	}
	else
	{
		curtime = 0.0;
	}

	lastcurtime = curtime;
}

//==========================================================================
//
//	Sys_Shutdown
//
//==========================================================================

void Sys_Shutdown()
{
	CoUninitialize();
	ShowCursor(TRUE);

	if (tevent)
	{
		CloseHandle(tevent);
	}
}

//==========================================================================
//
// 	Sys_Quit
//
// 	Shuts down net game, saves defaults, prints the exit text message,
// goes to text mode, and exits.
//
//==========================================================================

void Sys_Quit(const char*)
{
	dprintf("==========================================================================\n");
	dprintf("			Shuting down VAVOOM\n");
	dprintf("==========================================================================\n");

    // Shutdown system
	Host_Shutdown();

    SDL_Quit();

	exit(0);
}

//==========================================================================
//
// 	signal_handler
//
// 	Shuts down system, on error signal
//
//==========================================================================

static void signal_handler(int s)
{
	signal(s, SIG_IGN);  // Ignore future instances of this signal.

	switch (s)
	{
	 case SIGINT:	throw VavoomError("Interrupted by User");
	 case SIGILL:	throw VavoomError("Illegal Instruction");
	 case SIGFPE:	throw VavoomError("Floating Point Exception");
	 case SIGSEGV:	throw VavoomError("Segmentation Violation");
	 case SIGTERM:	throw VavoomError("Software termination signal from kill");
	 case SIGBREAK:	throw VavoomError("Ctrl-Break sequence");
	 case SIGABRT:	throw VavoomError("Abnormal termination triggered by abort call");
	 default:		throw VavoomError("Terminated by signal");
	}
}

//==========================================================================
//
//	Sys_ConsoleInput
//
//==========================================================================

char *Sys_ConsoleInput()
{
//FIXME
	return NULL;
}


//==========================================================================
//
//	WinMain
//
// 	Main program
//
//==========================================================================

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int iCmdShow)
int main (int argc, char *argv[])
{

     _chdir("C:\\Dev\\Mongrel\\artifacts");

	try
	{
		GArgs.Init(__argc, __argv);

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            Sys_Error("SDL_InitSubSystem(): %s\n",SDL_GetError());
        }

#ifndef _DEBUG
		//	Install signal handlers
		signal(SIGINT,  signal_handler);
		signal(SIGILL,  signal_handler);
		signal(SIGFPE,  signal_handler);
		signal(SIGSEGV, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGBREAK,signal_handler);
		signal(SIGABRT, signal_handler);
#endif

		Sys_InitTime();

		Host_Init();
		while (1)
		{
			Host_Frame();
		}
	}
	catch (VavoomError &e)
	{
		char *tmp_msg;

		Host_Shutdown();

		dprintf("\n\nERROR: %s\n", e.message);
		tmp_msg = new char[VStr::Length(e.message) +
			VStr::Length(Host_GetCoreDump()) + 4];
		sprintf(tmp_msg, "%s\n\n%s", e.message, Host_GetCoreDump());
		delete[] tmp_msg;
		tmp_msg = NULL;

        SDL_Quit();
		return 1;
	}
#ifndef _DEBUG
	catch (...)
	{
		char *tmp_msg;

		Host_Shutdown();
		dprintf("\n\nExiting due to external exception\n");

		tmp_msg = new char[VStr::Length(Host_GetCoreDump()) + 32];
		sprintf(tmp_msg, "Received external exception\n\n%s", Host_GetCoreDump());
		delete[] tmp_msg;
		tmp_msg = NULL;

//		throw;
        SDL_Quit();
		return 1;
	}
#endif
}

#endif

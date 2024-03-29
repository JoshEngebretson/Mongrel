//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: host.cpp 4428 2011-09-03 14:41:19Z firebrand_kh $
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

#include "gamedefs.h"
#include "cl_local.h"
//#include "svnrev.h"
#include <time.h>

// MACROS ------------------------------------------------------------------

// State updates, number of tics / second.
#define TICRATE		35

// TYPES -------------------------------------------------------------------

class EndGame : public VavoomError
{
public:
	explicit EndGame(const char *txt) : VavoomError(txt) { }
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

void Host_Quit();

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

#ifdef DEVELOPER
VCvarI			developer("developer", "1", CVAR_Archive);
#else
VCvarI			developer("developer", "0", CVAR_Archive);
#endif

int				host_frametics;
double			host_frametime;
double			host_time;
double			realtime;
double			oldrealtime;
int				host_framecount;

bool			host_initialised = false;
bool			host_request_exit = false;

vuint32			host_cycles[16];

#ifndef CLIENT
class VDedLog : public VLogListener
{
public:
	void Serialise(const char* Text, EName)
	{
		printf("%s", Text);
	}
};
static VDedLog	DedLog;
#endif

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static VCvarF	host_framerate("framerate", "0");

static double	last_time;

static VCvarI	respawnparm("RespawnMonsters", "0");	// checkparm of -respawn
static VCvarI	randomclass("RandomClass", "0");		// checkparm of -randclass
static VCvarI	fastparm("Fast", "0");					// checkparm of -fast

static VCvarI	show_time("show_time", "0");

static VCvarS	configfile("configfile", "config.cfg", CVAR_Archive);

static char		CurrentLanguage[4];
static VCvarS	Language("language", "en", CVAR_Archive);

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	Host_Init
//
//==========================================================================

void Host_Init()
{
	guard(Host_Init);
#ifdef CLIENT
	C_Init();
#else
	GLog.AddListener(&DedLog);
#endif

#if !defined(_WIN32)
	const char* HomeDir = getenv("HOME");
	if (HomeDir)
	{
		Sys_CreateDirectory(va("%s/.vavoom", HomeDir));
		OpenDebugFile(va("%s/.vavoom/debug.txt", HomeDir));
	}
	else
	{
		OpenDebugFile("basev/debug.txt");
	}
#else
	OpenDebugFile("basev/debug.txt");
#endif

   // Seed the random-number generator with the current time so that
   // the numbers will be different every time we run.
   srand((unsigned)time(NULL));

	// init subsystems

	M_InitByteOrder();

	VName::StaticInit();
	VObject::StaticInit();

	VCvar::Init();
	VCommand::Init();

	FL_Init();

	PR_Init();

	GLanguage.LoadStrings("en");
	strcpy(CurrentLanguage, "en");

	GSoundManager = new VSoundManager;
	GSoundManager->Init();
	R_InitData();
	R_InitTexture();

	ReadLineSpecialInfos();

#ifdef SERVER
	SV_Init();
#endif

#ifdef CLIENT
	CL_Init();

	GInput = VInputPublic::Create();
	GInput->Init();
	GAudio = VAudioPublic::Create();
	GAudio->Init();
	SCR_Init();
	V_Init();

	R_Init();

	T_Init();

	AM_Init();
#endif

	VCommand::ProcessKeyConf();

	R_ParseEffectDefs();

	InitMapInfo();	

	host_initialised = true;
	unguard;
}

//==========================================================================
//
//	Host_GetConsoleCommands
//
//	Add them exactly as if they had been typed at the console
//
//==========================================================================

static void Host_GetConsoleCommands()
{
	guard(Host_GetConsoleCommands);
	char	*cmd;

#ifdef CLIENT
	if (GGameInfo->NetMode != NM_DedicatedServer)
		return;
#endif

	for (cmd = Sys_ConsoleInput(); cmd; cmd = Sys_ConsoleInput())
	{
		GCmdBuf << cmd << "\n";
	}
	unguard;
}

//==========================================================================
//
//	FilterTime
//
//	Returns false if the time is too short to run a frame
//
//==========================================================================

extern VCvarI real_time;

static bool FilterTime()
{
	guard(FilterTime);
	double curr_time = Sys_Time();
	double time = curr_time - last_time;
	last_time = curr_time;

	realtime += time;

	if (real_time)
	{
		if (realtime - oldrealtime < 1.0 / 90.0)
		{
			return false;		// framerate is too high
		}
	}
	else
	{
		if (realtime - oldrealtime < 1.0 / 35.0)
		{
			return false;		// framerate is too high
		}
	}

	host_frametime = realtime - oldrealtime;

	if (host_framerate > 0)
	{
		host_frametime = host_framerate;
	}
	else
	{	// don't allow really long or short frames
		if (host_frametime > 0.1)
		{
			host_frametime = 0.1;
		}
		if (host_frametime < 0.001)
		{
			host_frametime = 0.001;
		}
	}
	
	int			thistime;
	static int	lasttime;

	thistime = (int)(realtime * TICRATE);
	host_frametics = thistime - lasttime;
	if (!real_time && host_frametics < 1)
	{
		return false;		//	No tics to run
	}
	if (host_frametics > 3)
	{
		host_frametics = 3;	//	Don't run too slow
	}
	oldrealtime = realtime;
	lasttime = thistime;

	return true;
	unguard;
}

//==========================================================================
//
//  Host_UpdateLanguage
//
//==========================================================================

static void Host_UpdateLanguage()
{
	guard(Host_UpdateLanguage);
	if (!Language.IsModified())
	{
		return;
	}

	VStr NewLang = VStr((const char*)Language).ToLower();
	if (NewLang.Length() != 2 && NewLang.Length() != 3)
	{
		GCon->Log("Language identifier must be 2 or 3 characters long");
		Language = CurrentLanguage;
		return;
	}

	if (Language == CurrentLanguage)
	{
		return;
	}

	GLanguage.LoadStrings(*NewLang);
	VStr::Cpy(CurrentLanguage, *NewLang);
	unguard;
}

//==========================================================================
//
//  Host_Frame
//
//	Runs all active servers
//
//==========================================================================

void Host_Frame()
{
	guard(Host_Frame);
	static double time1 = 0;
	static double time2 = 0;
	static double time3 = 0;
	int pass1, pass2, pass3;

	try
	{
		//	Keep the random time dependent
		rand();

		//	Decide the simulation time
		if (!FilterTime())
		{
			//	Don't run too fast, or packets will flood out
#ifndef CLIENT
			Sys_Sleep();
#endif
			return;
		}

		Host_UpdateLanguage();

#ifdef CLIENT
		//	Get new key, mice and joystick events
		GInput->ProcessEvents();
#endif

		//	Check for commands typed to the host
		Host_GetConsoleCommands();

		if (host_request_exit)
		{
			Host_Quit();
		}

#ifdef CLIENT
		//	Make intentions
		CL_SendMove();
#endif

#ifdef SERVER
		if (GGameInfo->NetMode != NM_None &&
			GGameInfo->NetMode != NM_Client)
		{
clock_cycle(host_cycles[0]);
			//	Server operations
			ServerFrame(host_frametics);
unclock_cycle(host_cycles[0]);
		}
#endif

		host_time += host_frametime;

#ifdef CLIENT
		//	Fetch results from server
		CL_ReadFromServer();

		//	Collect all garbage
		VObject::CollectGarbage();

		//	Update video
		if (show_time)
		{
			time1 = Sys_Time();
		}

		SCR_Update();

		if (show_time)
		{
			time2 = Sys_Time();
		}

		if (cls.signon)
		{
			CL_DecayLights();
		}

		//	Update audio
		GAudio->UpdateSounds();
#endif

		if (show_time)
		{
			pass1 = (int)((time1 - time3) * 1000);
			time3 = Sys_Time();
			pass2 = (int)((time2 - time1) * 1000);
			pass3 = (int)((time3 - time2) * 1000);
			GCon->Logf("%d tot | %d server | %d gfx | %d snd",
				pass1 + pass2 + pass3, pass1, pass2, pass3);
		}

		host_framecount++;
	}
	catch (RecoverableError &e)
	{
		GCon->Logf("Host_Error: %s", e.message);

		//	Reset progs virtual machine
		PR_OnAbort();
		//	Make sure, that we use primary wad files
		W_CloseAuxiliary();

#ifdef CLIENT
		if (GGameInfo->NetMode == NM_DedicatedServer)
		{
			SV_ShutdownGame();
			Sys_Error("Host_Error: %s\n", e.message);	// dedicated servers exit
		}

		SV_ShutdownGame();
		GClGame->eventOnHostError();
		C_StartFull();
#else
		SV_ShutdownGame();
		Sys_Error("Host_Error: %s\n", e.message);	// dedicated servers exit
#endif
	}
	catch (EndGame &e)
	{
		GCon->Logf(NAME_Dev, "Host_EndGame: %s", e.message);

		//	Reset progs virtual machine
		PR_OnAbort();
		//	Make sure, that we use primary wad files
		W_CloseAuxiliary();

#ifdef CLIENT
		if (GGameInfo->NetMode == NM_DedicatedServer)
		{
			SV_ShutdownGame();
			Sys_Error("Host_EndGame: %s\n", e.message);	// dedicated servers exit
		}

		SV_ShutdownGame();
		GClGame->eventOnHostEndGame();
#else
		SV_ShutdownGame();
		Sys_Error("Host_EndGame: %s\n", e.message);	// dedicated servers exit
#endif
	}
	unguard;
}

//==========================================================================
//
//	Host_EndGame
//
//==========================================================================

void Host_EndGame(const char *message, ...)
{
	va_list argptr;
	char string[1024];
	
	va_start(argptr,message);
	vsprintf(string,message,argptr);
	va_end(argptr);

	throw EndGame(string);
}

//==========================================================================
//
//	Host_Error
//
//	This shuts down both the client and server
//
//==========================================================================

void Host_Error(const char *error, ...)
{
	va_list argptr;
	char string[1024];
	
	va_start(argptr, error);
	vsprintf(string, error, argptr);
	va_end(argptr);

	throw RecoverableError(string);
}

//==========================================================================
//
//  Version_f
//
//==========================================================================

COMMAND(Version)
{
    GCon->Log("VAVOOM version " VERSION_TEXT" (r133+).");
	GCon->Log("Compiled "__DATE__" "__TIME__".");
}

//==========================================================================
//
//	Host_SaveConfiguration
//
//  Saves game variables
//
//==========================================================================

#ifdef CLIENT
void Host_SaveConfiguration()
{
	if (!host_initialised)
		return;

	FILE *f;
	if (fl_savedir.IsNotEmpty())
	{
		FL_CreatePath(fl_savedir + "/" + fl_gamedir);
		f = fopen(*(fl_savedir + "/" + fl_gamedir + "/" +
			(const char*)configfile), "w");
	}
	else
	{
		FL_CreatePath(fl_basedir + "/" + fl_gamedir);
		f = fopen(*(fl_basedir + "/" + fl_gamedir + "/" +
			(const char*)configfile), "w");
	}
	if (!f)
	{
		GCon->Logf("Host_SaveConfiguration: Failed to open config file \"%s\"",
			(const char*)configfile);
		return; // can't write the file, but don't complain
	}

	fprintf(f, "// Generated by Vavoom\n");
	fprintf(f, "//\n// Bindings\n//\n");
	GInput->WriteBindings(f);
	fprintf(f, "//\n// Aliases\n//\n");
	VCommand::WriteAlias(f);
	fprintf(f, "//\n// Variables\n//\n");
	VCvar::WriteVariables(f);

	fclose(f);
}
#endif

//==========================================================================
//
//  Host_Quit
//
//==========================================================================

void Host_Quit()
{
	guard(Host_Quit);
	SV_ShutdownGame();
#ifdef CLIENT
	// Save game configyration
	Host_SaveConfiguration();
#endif

	//	Get the lump with the end text.
	//	If option -noendtxt is set, don't print the text.
	bool GotEndText = false;
	char EndText[80 * 25 * 2];
	if (!GArgs.CheckParm("-noendtxt"))
	{
		//	Find end text lump.
		VStream* Strm = NULL;
		if (W_CheckNumForName(NAME_endoom) >= 0)
		{
			Strm = W_CreateLumpReaderName(NAME_endoom);
		}
		else if (W_CheckNumForName(NAME_endtext) >= 0)
		{
			Strm = W_CreateLumpReaderName(NAME_endtext);
		}
		else if (W_CheckNumForName(NAME_endstrf) >= 0)
		{
			Strm = W_CreateLumpReaderName(NAME_endstrf);
		}

		//	Read it, if found.
		if (Strm)
		{
			int Len = 80 * 25 * 2;
			if (Strm->TotalSize() < Len)
			{
				memset(EndText, 0, Len);
				Len = Strm->TotalSize();
			}
			Strm->Serialise(EndText, Len);
			delete Strm;
			Strm = NULL;
			GotEndText = true;
		}
	}

	Sys_Quit(GotEndText ? EndText : NULL);
	unguard;
}

//==========================================================================
//
//  Quit
//
//==========================================================================

COMMAND(Quit)
{
	host_request_exit = true;
}

//==========================================================================
//
//	Host_Shutdown
//
//	Return to default system state
//
//==========================================================================

void Host_Shutdown()
{
	static bool		shutting_down = false;

	if (shutting_down)
	{
		GCon->Log("Recursive shutdown");
		return;
	}
	shutting_down = true;

#define SAFE_SHUTDOWN(name, args) \
	try { GCon->Log("Doing "#name); name args; } catch (...) { GCon->Log(#name" failed"); }

#ifdef CLIENT
	SAFE_SHUTDOWN(C_Shutdown, ())
	SAFE_SHUTDOWN(CL_Shutdown, ())
#endif
#ifdef SERVER
	SAFE_SHUTDOWN(SV_Shutdown, ())
#endif
#ifdef CLIENT
	if (GInput)
	{
		SAFE_SHUTDOWN(delete GInput,)
		GInput = NULL;
	}
	SAFE_SHUTDOWN(V_Shutdown, ())
	if (GAudio)
	{
		SAFE_SHUTDOWN(delete GAudio,)
		GAudio = NULL;
	}
	SAFE_SHUTDOWN(T_Shutdown, ())
#endif
	SAFE_SHUTDOWN(Sys_Shutdown, ())

	if (GSoundManager)
	{
		SAFE_SHUTDOWN(delete GSoundManager,)
		GSoundManager = NULL;
	}
	SAFE_SHUTDOWN(R_ShutdownTexture, ())
	SAFE_SHUTDOWN(R_ShutdownData, ())
	SAFE_SHUTDOWN(VCommand::Shutdown, ())
	SAFE_SHUTDOWN(VCvar::Shutdown, ())
	SAFE_SHUTDOWN(ShutdownMapInfo, ())
	SAFE_SHUTDOWN(FL_Shutdown, ())
	SAFE_SHUTDOWN(W_Shutdown, ())
	SAFE_SHUTDOWN(GLanguage.FreeData, ())
	SAFE_SHUTDOWN(ShutdownDecorate, ())

	SAFE_SHUTDOWN(VObject::StaticExit, ())
	SAFE_SHUTDOWN(VName::StaticExit, ())
	SAFE_SHUTDOWN(Z_Shutdown, ())
}

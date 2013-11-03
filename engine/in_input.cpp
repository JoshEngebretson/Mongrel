//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: in_input.cpp 4336 2010-09-17 21:54:34Z firebrand_kh $
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
//**
//**	EVENT HANDLING
//**
//**	Events are asynchronous inputs generally generated by the game user.
//**	Events can be discarded if no responder claims them
//**
//**************************************************************************

// HEADER FILES ------------------------------------------------------------

#include "gamedefs.h"
#include "cl_local.h"
#include "ui.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

//
//	Input subsystem, handles all input events.
//
class VInput : public VInputPublic
{
public:
	VInput();
	~VInput();

	//	System device related functions.
	void Init();
	void Shutdown();

	//	Input event handling.
	void PostEvent(event_t*);
	void KeyEvent(int, int);
	void ProcessEvents();
	int ReadKey();

	//	Handling of key bindings.
	void GetBindingKeys(const VStr&, int&, int&);
	void GetBinding(int, VStr&, VStr&);
	void SetBinding(int, const VStr&, const VStr&, bool);
	void WriteBindings(FILE*);

	int TranslateKey(int);

	int KeyNumForName(const VStr& Name);
	VStr KeyNameForNum(int KeyNr);

private:
	enum { MAXEVENTS = 64 };

	VInputDevice*	Device;

	event_t			Events[MAXEVENTS];
	int				EventHead;
	int				EventTail;

	VStr			KeyBindingsDown[256];
	VStr			KeyBindingsUp[256];
	bool			KeyBindingsSave[256];

	static const char*	KeyNames[SCANCODECOUNT];
	static const char	ShiftXForm[];
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

VInputPublic*		GInput;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

const char*		VInput::KeyNames[SCANCODECOUNT] =
{
	"UP",
	"LEFT",
	"RIGHT",
	"DOWN",
	"INSERT",
	"DELETE",
	"HOME",
	"END",
	"PAGEUP",
	"PAGEDOWN",

	"PAD0",
	"PAD1",
	"PAD2",
	"PAD3",
	"PAD4",
	"PAD5",
	"PAD6",
	"PAD7",
	"PAD8",
	"PAD9",

	"NUMLOCK",
	"PADDIVIDE",
	"PADMULTIPLE",
	"PADMINUS",
	"PADPLUS",
	"PADENTER",
	"PADDOT",

	"ESCAPE",
	"ENTER",
	"TAB",
	"BACKSPACE",
	"CAPSLOCK",

	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

	"LSHIFT",
	"RSHIFT",
	"LCTRL",
	"RCTRL",
	"LALT",
	"RALT",

	"LWIN",
	"RWIN",
	"MENU",

	"PRINTSCREEN",
	"SCROLLLOCK",
	"PAUSE",

	"ABNT_C1",
	"YEN",
	"KANA",
	"CONVERT",
	"NOCONVERT",
	"AT",
	"CIRCUMFLEX",
	"COLON2",
	"KANJI",

	"MOUSE1",
	"MOUSE2",
	"MOUSE3",

	"MOUSED1",
	"MOUSED2",
	"MOUSED3",

	"MWHEELUP",
	"MWHEELDOWN",

	"JOY1",
	"JOY2",
	"JOY3",
	"JOY4",
	"JOY5",
	"JOY6",
	"JOY7",
	"JOY8",
	"JOY9",
	"JOY10",
	"JOY11",
	"JOY12",
	"JOY13",
	"JOY14",
	"JOY15",
	"JOY16"
};

//  Key shifting
const char		VInput::ShiftXForm[] =
{
	0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31,
	' ', '!', '"', '#', '$', '%', '&',
	'"', // shift-'
	'(', ')', '*', '+',
	'<', // shift-,
	'_', // shift--
	'>', // shift-.
	'?', // shift-/
	')', // shift-0
	'!', // shift-1
	'@', // shift-2
	'#', // shift-3
	'$', // shift-4
	'%', // shift-5
	'^', // shift-6
	'&', // shift-7
	'*', // shift-8
	'(', // shift-9
	':',
	':', // shift-;
	'<',
	'+', // shift-=
	'>', '?', '@',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'{', // shift-[
	'|', // shift-backslash - OH MY GOD DOES WATCOM SUCK
	'}', // shift-]
	'"', '_',
	'\'', // shift-`
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'{', '|', '}', '~', 127
};

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	VInputPublic::Create
//
//==========================================================================

VInputPublic* VInputPublic::Create()
{
	return new VInput();
}

//==========================================================================
//
//	VInput::VInput
//
//==========================================================================

VInput::VInput()
: Device(0)
, EventHead(0)
, EventTail(0)
{
	memset(KeyBindingsSave, 0, sizeof(KeyBindingsSave));
}

//==========================================================================
//
//	VInput::~VInput
//
//==========================================================================

VInput::~VInput()
{
	Shutdown();
}

//==========================================================================
//
//  VInput::Init
//
//==========================================================================

void VInput::Init()
{
	guard(VInput::Init);
    Device = VInputDevice::CreateDevice();
	unguard;
}

//==========================================================================
//
//  VInput::Shutdown
//
//==========================================================================

void VInput::Shutdown()
{
	guard(VInput::Shutdown);
	if (Device)
	{
		delete Device;
		Device = NULL;
	}
	unguard;
}

//==========================================================================
//
//	VInput::PostEvent
//
// 	Called by the I/O functions when input is detected
//
//==========================================================================

void VInput::PostEvent(event_t* ev)
{
	guard(VInput::PostEvent);
	Events[EventHead] = *ev;
	EventHead = (EventHead + 1) & (MAXEVENTS - 1);
	unguard;
}

//==========================================================================
//
//	VInput::KeyEvent
//
// 	Called by the I/O functions when a key or button is pressed or released
//
//==========================================================================

void VInput::KeyEvent(int key, int press)
{
	guard(VInput::KeyEvent);
	if (!key)
	{
		return;
	}
	Events[EventHead].type = press ? ev_keydown : ev_keyup;
    Events[EventHead].data1 = key;
	Events[EventHead].data2 = 0;
	Events[EventHead].data3 = 0;
	EventHead = (EventHead + 1) & (MAXEVENTS - 1);
	unguard;
}

//==========================================================================
//
//	VInput::ProcessEvents
//
// 	Send all the Events of the given timestamp down the responder chain
//
//==========================================================================

void VInput::ProcessEvents()
{
	guard(VInput::ProcessEvents);

	Device->ReadInput();

	for (; EventTail != EventHead; EventTail = (EventTail + 1) & (MAXEVENTS - 1))
	{
		event_t* ev = &Events[EventTail];

		// Shift key state
		if (ev->data1 == K_RSHIFT)
		{
			ShiftDown &= ~1;
			if (ev->type == ev_keydown)
				ShiftDown |= 1;
		}
		if (ev->data1 == K_LSHIFT)
		{
			ShiftDown &= ~2;
			if (ev->type == ev_keydown)
				ShiftDown |= 2;
		}

		// Ctrl key state
		if (ev->data1 == K_RCTRL)
		{
			CtrlDown &= ~1;
			if (ev->type == ev_keydown)
				CtrlDown |= 1;
		}
		if (ev->data1 == K_LCTRL)
		{
			CtrlDown &= ~2;
			if (ev->type == ev_keydown)
				CtrlDown |= 2;;
		}

		// Alt key state
		if (ev->data1 == K_RALT)
		{
			AltDown &= ~1;
			if (ev->type == ev_keydown)
				AltDown |= 1;
		}
		if (ev->data1 == K_LALT)
		{
			AltDown &= ~2;
			if (ev->type == ev_keydown)
				AltDown |= 2;;
		}

		if (C_Responder(ev))
			continue;

		if (CT_Responder(ev))
			continue;

		if (MN_Responder(ev))
			continue;

		if (GRoot->Responder(ev))
			continue;

		if (cl && !GClGame->intermission)
		{
			if (SB_Responder(ev))
				continue;	// status window ate it
			if (AM_Responder(ev))
				continue;	// automap ate it
		}

		if (F_Responder(ev))
			continue;

		//
		//	Key bindings
		//
		if (ev->type == ev_keydown)
		{
			VStr kb = KeyBindingsDown[ev->data1];
			if (kb.IsNotEmpty())
			{
				if (kb[0] == '+' || kb[0] == '-')
				{
					// button commands add keynum as a parm
					GCmdBuf << kb << " " << VStr(ev->data1) << "\n";
				}
				else
				{
					GCmdBuf << kb << "\n";
				}
				continue;
			}
		}
		if (ev->type == ev_keyup)
		{
			VStr kb = KeyBindingsUp[ev->data1];
			if (kb.IsNotEmpty())
			{
				if (kb[0] == '+' || kb[0] == '-')
				{
					// button commands add keynum as a parm
					GCmdBuf << kb << " " << VStr(ev->data1) << "\n";
				}
				else
				{
					GCmdBuf << kb << "\n";
				}
				continue;
			}
		}
		if (CL_Responder(ev))
			continue;
	}

	unguard;
}

//==========================================================================
//
//	VInput::ReadKey
//
//==========================================================================

int VInput::ReadKey()
{
	guard(VInput::ReadKey);
	int		ret = 0;

	do
	{
		Device->ReadInput();
		while (EventTail != EventHead && !ret)
		{
			if (Events[EventTail].type == ev_keydown)
			{
				ret = Events[EventTail].data1;
			}
			EventTail = (EventTail + 1) & (MAXEVENTS - 1);
		}
	} while (!ret);

	return ret;
	unguard;
}

//==========================================================================
//
//	VInput::GetBindingKeys
//
//==========================================================================

void VInput::GetBindingKeys(const VStr& Binding, int& Key1, int& Key2)
{
	guard(VInput::GetBindingKeys);
	Key1 = -1;
	Key2 = -1;
	for (int i = 0; i < 256; i++)
	{
		if (!Binding.ICmp(KeyBindingsDown[i]))
		{
			if (Key1 != -1)
			{
				Key2 = i;
				return;
			}
			Key1 = i;
		}
	}
	unguard;
}

//==========================================================================
//
//	VInput::GetBinding
//
//==========================================================================

void VInput::GetBinding(int KeyNum, VStr& Down, VStr& Up)
{
	guard(VInput::GetBinding);
	Down = KeyBindingsDown[KeyNum];
	Up = KeyBindingsUp[KeyNum];
	unguard;
}

//==========================================================================
//
//	VInput::SetBinding
//
//==========================================================================

void VInput::SetBinding(int KeyNum, const VStr& Down, const VStr& Up,
	bool Save)
{
	guard(VInput::SetBinding);
	if (KeyNum == -1)
	{
		return;
	}
	if (Down.IsEmpty() && Up.IsEmpty() && !KeyBindingsSave[KeyNum])
	{
		return;
	}

	KeyBindingsDown[KeyNum] = Down;
	KeyBindingsUp[KeyNum] = Up;
	KeyBindingsSave[KeyNum] = Save;
	unguard;
}

//==========================================================================
//
//	VInput::WriteBindings
//
//	Writes lines containing "bind key value"
//
//==========================================================================

void VInput::WriteBindings(FILE* f)
{
	guard(VInput::WriteBindings);
	fprintf(f, "UnbindAll\n");
	for (int i = 0; i < 256; i++)
	{
		if ((KeyBindingsDown[i].IsNotEmpty() ||
			KeyBindingsUp[i].IsNotEmpty()) && KeyBindingsSave[i])
		{
			fprintf(f, "bind \"%s\" \"%s\" \"%s\"\n", *KeyNameForNum(i),
				*KeyBindingsDown[i], *KeyBindingsUp[i]);
		}
	}
	unguard;
}

//==========================================================================
//
//	VInput::TranslateKey
//
//==========================================================================

int VInput::TranslateKey(int ch)
{
	guard(VInput::TranslateKey);
	int Tmp = ch;
	return ShiftDown ? ShiftXForm[Tmp] : Tmp;
	unguard;
}

//==========================================================================
//
//	VInput::KeyNumForName
//
//  Searches in key names for given name
// return key code
//
//==========================================================================

int VInput::KeyNumForName(const VStr& Name)
{
	guard(VInput::KeyNumForName);
	if (Name.IsEmpty())
		return -1;

	//	Single character.
	if (!Name[1])
		return VStr::ToLower(Name[0]);

	//	Special cases that you can't enter in console.
	if (!Name.ICmp("Space"))
		return ' ';

	if (!Name.ICmp("Tilde"))
		return '`';

	//	Check special key names.
	for (int i = 0; i < SCANCODECOUNT; i++)
		if (!Name.ICmp(KeyNames[i]))
			return i + 0x80;

	//	Not found.
	return -1;
	unguard;
}

//==========================================================================
//
//	VInput::KeyNameForNum
//
//	Writes into given string key name
//
//==========================================================================

VStr VInput::KeyNameForNum(int KeyNr)
{
	guard(VInput::KeyNameForNum);
	if (KeyNr == ' ')
		return "SPACE";
	else if (KeyNr >= 0x80)
		return KeyNames[KeyNr - 0x80];
	else if (KeyNr)
		return VStr((char)KeyNr);
	else
		return VStr();
	unguard;
}

//==========================================================================
//
//	COMMAND Unbind
//
//==========================================================================

COMMAND(Unbind)
{
	guard(COMMAND Unbind);
	if (Args.Num() != 2)
	{
		GCon->Log("unbind <key> : remove commands from a key");
		return;
	}

	int b = GInput->KeyNumForName(Args[1]);
	if (b == -1)
	{
		GCon->Log(VStr("\"") + Args[1] + "\" isn\'t a valid key");
		return;
	}

	GInput->SetBinding(b, VStr(), VStr());
	unguard;
}

//==========================================================================
//
//	COMMAND UnbindAll
//
//==========================================================================

COMMAND(UnbindAll)
{
	guard(COMMAND UnbindAll);
	for (int i = 0; i < 256; i++)
	{
		GInput->SetBinding(i, VStr(), VStr());
	}
	unguard;
}

//==========================================================================
//
//	COMMAND Bind
//
//==========================================================================

COMMAND(Bind)
{
	guard(COMMAND Bind);
	int c = Args.Num();

	if (c != 2 && c != 3 && c != 4)
	{
		GCon->Log("bind <key> [down_command] [up_command]: attach a command to a key");
		return;
	}
	int b = GInput->KeyNumForName(Args[1]);
	if (b == -1)
	{
		GCon->Log(VStr("\"") + Args[1] + "\" isn\'t a valid key");
		return;
	}

	if (c == 2)
	{
		VStr Down, Up;
		GInput->GetBinding(b, Down, Up);
		if (Down.IsNotEmpty() || Up.IsNotEmpty())
			GCon->Log(Args[1] + " = \"" + Down + "\" / \"" + Up + "\"");
		else
			GCon->Logf("%s is not bound", *Args[1]);
		return;
	}
	GInput->SetBinding(b, Args[2], c > 3 ? Args[3] : VStr(), !ParsingKeyConf);
	unguard;
}

//==========================================================================
//
//	COMMAND DefaultBind
//
//==========================================================================

COMMAND(DefaultBind)
{
	guard(COMMAND DefaultBind);
	int c = Args.Num();

	if (c != 2 && c != 3 && c != 4)
	{
		GCon->Log("defaultbind <key> [down_command] [up_command]: attach a command to a key");
		return;
	}
	int b = GInput->KeyNumForName(Args[1]);
	if (b == -1)
	{
		GCon->Log(VStr("\"") + Args[1] + "\" isn\'t a valid key");
		return;
	}

	if (c == 2)
	{
		VStr Down, Up;
		GInput->GetBinding(b, Down, Up);
		if (Down.IsNotEmpty() || Up.IsNotEmpty())
			GCon->Log(Args[1] + " = \"" + Down + "\" / \"" + Up + "\"");
		else
			GCon->Logf("%s is not bound", *Args[1]);
		return;
	}
	GInput->SetBinding(b, Args[2], c > 3 ? Args[3] : VStr(), !ParsingKeyConf);
	unguard;
}

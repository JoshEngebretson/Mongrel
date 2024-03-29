//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: log.h 4204 2010-04-03 15:23:34Z dj_jl $
//**
//**	Copyright (C) 1999-2010 Jānis Legzdiņš
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

//==========================================================================
//
//	VLogListener
//
//==========================================================================

class VLogListener : VInterface
{
public:
	virtual void Serialise(const char* Text, EName Event) = 0;
};

//==========================================================================
//
//	VLog
//
//==========================================================================

class VLog
{
private:
	enum { MAX_LISTENERS	= 8 };

	VLogListener*	Listeners[MAX_LISTENERS];

public:
	VLog();

	void AddListener(VLogListener* Listener);
	void RemoveListener(VLogListener* Listener);

	void Write(EName Type, const char* Fmt, ...);
	void WriteLine(EName Type, const char* Fmt, ...);

	void Write(const char* Fmt, ...);
	void WriteLine(const char* Fmt, ...);

	void DWrite(const char* Fmt, ...);
	void DWriteLine(const char* Fmt, ...);
};

extern VLog			GLog;

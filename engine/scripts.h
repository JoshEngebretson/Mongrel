//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: scripts.h 4324 2010-07-09 19:19:37Z firebrand_kh $
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

class VScriptParser
{
public:
	VScriptParser(const VStr&, VStream*);
	~VScriptParser();

	int 		Line;
	bool		End;
	bool		Crossed;
	bool		QuotedString;
	VStr		String;
	VName		Name8;
	VName		Name;
	int 		Number;
	double 		Float;

	bool IsText();
	void SetCMode(bool);
	void SetEscape(bool);
	bool AtEnd();
	bool GetString();
	void ExpectString();
	void ExpectName8();
	void ExpectName();
	void ExpectIdentifier();
	bool Check(const char*);
	void Expect(const char*);
	bool CheckQuotedString();
	bool CheckIdentifier();
	bool CheckNumber();
	void ExpectNumber();
	bool CheckNumberWithSign();
	void ExpectNumberWithSign();
	bool CheckFloat();
	void ExpectFloat();
	bool CheckFloatWithSign();
	void ExpectFloatWithSign();
	void UnGet();
	void Message(const char*);
	void Error(const char*);
	TLocation GetLoc();

private:
	VStr		ScriptName;
	char*		ScriptBuffer;
	char*		ScriptPtr;
	char*		ScriptEndPtr;
	int			ScriptSize;
	int			SrcIdx;
	bool		AlreadyGot;
	bool		CMode;
	bool		Escape;
};

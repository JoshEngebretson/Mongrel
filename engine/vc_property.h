//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vc_property.h 3893 2008-12-17 20:06:48Z dj_jl $
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

//==========================================================================
//
//	Property flags
//
//==========================================================================

enum
{
	PROP_Native		= 0x0001,	// Native get and set methods
	PROP_Final		= 0x0002,	// Final version of a proeprty
};

//==========================================================================
//
//	VProperty
//
//==========================================================================

class VProperty : public VMemberBase
{
public:
	//	Persistent fields.
	VFieldType		Type;
	VMethod*		GetFunc;
	VMethod*		SetFunc;
	VField*			DefaultField;
	vuint32			Flags;

	//	Compiler fields.
	VExpression*	TypeExpr;
	VName			DefaultFieldName;

	VProperty(VName, VMemberBase*, TLocation);
	~VProperty();

	void Serialise(VStream&);
	bool Define();

	friend inline VStream& operator<<(VStream& Strm, VProperty*& Obj)
	{ return Strm << *(VMemberBase**)&Obj; }
};

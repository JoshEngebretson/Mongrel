//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vc_field.h 3897 2008-12-25 17:11:02Z dj_jl $
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
//	Field flags
//
//==========================================================================

enum
{
	FIELD_Native	= 0x0001,	//	Native serialisation
	FIELD_Transient	= 0x0002,	//	Not to be saved
	FIELD_Private	= 0x0004,	//	Private field
	FIELD_ReadOnly	= 0x0008,	//	Read-only field
	FIELD_Net		= 0x0010,	//	Network replicated field
};

//==========================================================================
//
//	VField
//
//==========================================================================

class VField : public VMemberBase
{
public:
	//	Persistent fields
	VField*			Next;
	VFieldType		Type;
	VMethod*		Func;
	vuint32			Flags;
	VMethod*		ReplCond;

	//	Compiler fields
	VExpression*	TypeExpr;

	//	Run-time fields
	VField*		NextReference;	//	Linked list of reference fields.
	VField*		DestructorLink;
	VField*		NextNetField;
	vint32		Ofs;
	vint32		NetIndex;

	VField(VName, VMemberBase*, TLocation);
	~VField();

	void Serialise(VStream&);
	bool NeedsDestructor() const;
	bool Define();

#ifndef IN_VCC
	static void CopyFieldValue(const vuint8*, vuint8*, const VFieldType&);
	static void SerialiseFieldValue(VStream&, vuint8*, const VFieldType&);
	static void CleanField(vuint8*, const VFieldType&);
	static void DestructField(vuint8*, const VFieldType&);
	static bool IdenticalValue(const vuint8*, const vuint8*, const VFieldType&);
#endif

	friend inline VStream& operator<<(VStream& Strm, VField*& Obj)
	{ return Strm << *(VMemberBase**)&Obj; }

	vuint8* GetFieldPtr(VObject* Obj) const
	{
		return (vuint8*)Obj + Ofs;
	}
	float GetFloat(const VObject* Obj) const
	{
		return *(float*)((vuint8*)Obj + Ofs);
	}
	TVec GetVec(const VObject* Obj) const
	{
		return *(TVec*)((vuint8*)Obj + Ofs);
	}

	void SetByte(VObject* Obj, vuint8 Value) const
	{
		*((vuint8*)Obj + Ofs) = Value;
	}
	void SetInt(VObject* Obj, int Value) const
	{
		*(vint32*)((vuint8*)Obj + Ofs) = Value;
	}
	void SetInt(VObject* Obj, int Value, int Idx) const
	{
		((vint32*)((vuint8*)Obj + Ofs))[Idx] = Value;
	}
	void SetFloat(VObject* Obj, float Value) const
	{
		*(float*)((vuint8*)Obj + Ofs) = Value;
	}
	void SetFloat(VObject* Obj, float Value, int Idx) const
	{
		((float*)((vuint8*)Obj + Ofs))[Idx] = Value;
	}
	void SetName(VObject* Obj, VName Value) const
	{
		*(VName*)((vuint8*)Obj + Ofs) = Value;
	}
	void SetStr(VObject* Obj, const VStr& Value) const
	{
		*(VStr*)((vuint8*)Obj + Ofs) = Value;
	}
	void SetBool(VObject* Obj, int Value) const
	{
		if (Value)
		{
			*(vuint32*)((vuint8*)Obj + Ofs) |= Type.BitMask;
		}
		else
		{
			*(vuint32*)((vuint8*)Obj + Ofs) &= ~Type.BitMask;
		}
	}
	void SetVec(VObject* Obj, const TVec& Value) const
	{
		*(TVec*)((vuint8*)Obj + Ofs) = Value;
	}
	void SetClass(VObject* Obj, VClass* Value) const
	{
		*(VClass**)((vuint8*)Obj + Ofs) = Value;
	}
};

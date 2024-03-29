//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: str.cpp 4352 2010-12-20 03:14:10Z firebrand_kh $
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
//
//	Dynamic string class.
//
//**************************************************************************

// HEADER FILES ------------------------------------------------------------

#include "core.h"
#include <cctype>

// MACROS ------------------------------------------------------------------

#if !defined _WIN32 && !defined DJGPP
#undef stricmp	//	Allegro defines them
#undef strnicmp
#define stricmp		strcasecmp
#define strnicmp	strncasecmp
#endif

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static char		va_buffer[4][1024];
static int		va_bufnum;

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	VStr::VStr
//
//==========================================================================

VStr::VStr(const VStr& InStr, int Start, int Len)
: Str(NULL)
{
	check(Start >= 0);
	check(Start <= (int)InStr.Length());
	check(Len >= 0);
	check(Start + Len <= (int)InStr.Length());
	if (Len)
	{
		Resize(Len);
		NCpy(Str, InStr.Str + Start, Len);
	}
}

//==========================================================================
//
//	VStr::Resize
//
//==========================================================================

void VStr::Resize(int NewLen)
{
	guard(VStr::Resize);
	check(NewLen >= 0);
	if ((size_t)NewLen == Length())
	{
		//	Same length, use existing buffer.
		return;
	}
	if (!NewLen)
	{
		//	Free string.
		if (Str)
		{
			delete[] (Str - sizeof(int));
			Str = NULL;
		}
	}
	else
	{
		//	Allocate memory.
		int AllocLen = sizeof(int) + NewLen + 1;
		char* NewStr = (new char[AllocLen]) + sizeof(int);
		if (Str)
		{
			size_t Len = Min(Length(), (size_t)NewLen);
			NCpy(NewStr, Str, Len);
			delete[] (Str - sizeof(int));
			Str = NULL;
		}
		Str = NewStr;
		//	Set length.
		((int*)Str)[-1] = NewLen;
		//	Set terminator.
		Str[NewLen] = 0;
	}
	unguard;
}

//==========================================================================
//
//	VStr::StartsWith
//
//==========================================================================

bool VStr::StartsWith(const char* S) const
{
	guard(VStr::StartsWith);
	size_t l = Length(S);
	if (l > Length())
	{
		return false;
	}
	return NCmp(**this, S, l) == 0;
	unguard;
}

//==========================================================================
//
//	VStr::StartsWith
//
//==========================================================================

bool VStr::StartsWith(const VStr& S) const
{
	guard(VStr::StartsWith);
	size_t l = S.Length();
	if (l > Length())
	{
		return false;
	}
	return NCmp(**this, *S, l) == 0;
	unguard;
}

//==========================================================================
//
//	VStr::EndsWith
//
//==========================================================================

bool VStr::EndsWith(const char* S) const
{
	guard(VStr::EndsWith);
	size_t l = Length(S);
	if (l > Length())
	{
		return false;
	}
	return NCmp(**this + Length() - l, S, l) == 0;
	unguard;
}

//==========================================================================
//
//	VStr::EndsWith
//
//==========================================================================

bool VStr::EndsWith(const VStr& S) const
{
	guard(VStr::EndsWith);
	size_t l = S.Length();
	if (l > Length())
	{
		return false;
	}
	return NCmp(**this + Length() - l, *S, l) == 0;
	unguard;
}

//==========================================================================
//
//	VStr::ToLower
//
//==========================================================================

VStr VStr::ToLower() const
{
	guard(VStr::ToLower);
	if (!Str)
	{
		return VStr();
	}
	VStr Ret;
	int l = int(Length());
	Ret.Resize(l);
	for (int i = 0; i < l; i++)
	{
		Ret.Str[i] = ToLower(Str[i]);
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::ToUpper
//
//==========================================================================

VStr VStr::ToUpper() const
{
	guard(VStr::ToUpper);
	if (!Str)
	{
		return VStr();
	}
	VStr Ret;
	int l = int(Length());
	Ret.Resize(l);
	for (int i = 0; i < l; i++)
	{
		Ret.Str[i] = ToUpper(Str[i]);
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::IndexOf
//
//==========================================================================

int VStr::IndexOf(char C) const
{
	guard(VStr::IndexOf);
	int l = int(Length());
	for (int i = 0; i < l; i++)
	{
		if (Str[i] == C)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::IndexOf
//
//==========================================================================

int VStr::IndexOf(const char* S) const
{
	guard(VStr::IndexOf);
	int sl = int(Length(S));
	if (!sl)
	{
		return -1;
	}
	int l = int(Length());
	for (int i = 0; i <= l - sl; i++)
	{
		if (NCmp(Str + i, S, sl) == 0)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::IndexOf
//
//==========================================================================

int VStr::IndexOf(const VStr& S) const
{
	guard(VStr::IndexOf);
	int sl = int(S.Length());
	if (!sl)
	{
		return -1;
	}
	int l = int(Length());
	for (int i = 0; i <= l - sl; i++)
	{
		if (NCmp(Str + i, *S, sl) == 0)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::LastIndexOf
//
//==========================================================================

int VStr::LastIndexOf(char C) const
{
	guard(VStr::LastIndexOf);
	for (int i = int(Length()) - 1; i >= 0; i--)
	{
		if (Str[i] == C)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::LastIndexOf
//
//==========================================================================

int VStr::LastIndexOf(const char* S) const
{
	guard(VStr::LastIndexOf);
	int sl = int(Length(S));
	if (!sl)
	{
		return -1;
	}
	int l = int(Length());
	for (int i = l - sl; i >= 0; i--)
	{
		if (NCmp(Str + i, S, sl) == 0)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::LastIndexOf
//
//==========================================================================

int VStr::LastIndexOf(const VStr& S) const
{
	guard(VStr::LastIndexOf);
	int sl = int(S.Length());
	if (!sl)
	{
		return -1;
	}
	int l = int(Length());
	for (int i = l - sl; i >= 0; i--)
	{
		if (NCmp(Str + i, *S, sl) == 0)
		{
			return i;
		}
	}
	return -1;
	unguard;
}

//==========================================================================
//
//	VStr::Replace
//
//==========================================================================

VStr VStr::Replace(const char* Search, const char* Replacement) const
{
	guard(VStr::Replace);
	if (!Length())
	{
		//	Nothing to replace in an empty string.
		return *this;
	}

	size_t SLen = Length(Search);
	size_t RLen = Length(Replacement);
	if (!SLen)
	{
		//	Nothing to search for.
		return *this;
	}

	VStr Ret = *this;
	size_t i = 0;
	while (i <= Ret.Length() - SLen)
	{
		if (!NCmp(Ret.Str + i, Search, SLen))
		{
			//	If search and replace strings are of the same size, we can
			// just copy the data and avoid memory allocations.
			if (SLen == RLen)
			{
				memcpy(Ret.Str + i, Replacement, RLen);
			}
			else
			{
				Ret = VStr(Ret, 0, int(i)) + Replacement +
					VStr(Ret, int(i + SLen), int(Ret.Length() - i - SLen));
			}
			i += RLen;
		}
		else
		{
			i++;
		}
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::Replace
//
//==========================================================================

VStr VStr::Replace(const VStr& Search, const VStr& Replacement) const
{
	guard(VStr::Replace);
	if (!Length())
	{
		//	Nothing to replace in an empty string.
		return *this;
	}

	size_t SLen = Search.Length();
	size_t RLen = Replacement.Length();
	if (!SLen)
	{
		//	Nothing to search for.
		return *this;
	}

	VStr Ret = *this;
	size_t i = 0;
	while (i <= Ret.Length() - SLen)
	{
		if (!NCmp(Ret.Str + i, *Search, SLen))
		{
			//	If search and replace strings are of the same size, we can
			// just copy the data and avoid memory allocations.
			if (SLen == RLen)
			{
				memcpy(Ret.Str + i, *Replacement, RLen);
			}
			else
			{
				Ret = VStr(Ret, 0, int(i)) + Replacement +
					VStr(Ret, int(i + SLen), int(Ret.Length() - i - SLen));
			}
			i += RLen;
		}
		else
		{
			i++;
		}
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::Utf8Substring
//
//==========================================================================

VStr VStr::Utf8Substring(int Start, int Len) const
{
	check(Start >= 0);
	check(Start <= (int)Utf8Length());
	check(Len >= 0);
	check(Start + Len <= (int)Utf8Length());
	if (!Len)
	{
		return VStr();
	}
	int RealStart = int(ByteLengthForUtf8(Str, Start));
	int RealLen = int(ByteLengthForUtf8(Str, Start + Len) - RealStart);
	return VStr(*this, RealStart, RealLen);
}

//==========================================================================
//
//	VStr::Split
//
//==========================================================================

void VStr::Split(char C, TArray<VStr>& A) const
{
	guard(VStr::Split);
	A.Clear();
	if (!Str)
	{
		return;
	}
	int Start = 0;
	int Len = int(Length());
	for (int i = 0; i <= Len; i++)
	{
		if (i == Len || Str[i] == C)
		{
			if (Start != i)
			{
				A.Append(VStr(*this, Start, i - Start));
			}
			Start = i + 1;
		}
	}
	unguard;
}

//==========================================================================
//
//	VStr::Split
//
//==========================================================================

void VStr::Split(const char* Chars, TArray<VStr>& A) const
{
	guard(VStr::Split);
	A.Clear();
	if (!Str)
	{
		return;
	}
	int Start = 0;
	int Len = int(Length());
	for (int i = 0; i <= Len; i++)
	{
		bool DoSplit = i == Len;
		for (const char* pChar = Chars; !DoSplit && *pChar; pChar++)
		{
			DoSplit = Str[i] == *pChar;
		}
		if (DoSplit)
		{
			if (Start != i)
			{
				A.Append(VStr(*this, Start, i - Start));
			}
			Start = i + 1;
		}
	}
	unguard;
}

//==========================================================================
//
//	VStr::IsValidUtf8
//
//==========================================================================

bool VStr::IsValidUtf8() const
{
	guard(VStr::IsValidUtf8);
	if (!Str)
	{
		return true;
	}
	for (const char* c = Str; *c;)
	{
		if ((*c & 0x80) == 0)
		{
			c++;
		}
		else if ((*c & 0xe0) == 0xc0)
		{
			if ((c[1] & 0xc0) != 0x80)
			{
				return false;
			}
			c += 2;
		}
		else if ((*c & 0xf0) == 0xe0)
		{
			if ((c[1] & 0xc0) != 0x80 || (c[2] & 0xc0) != 0x80)
			{
				return false;
			}
			c += 3;
		}
		else if ((*c & 0xf8) == 0xf0)
		{
			if ((c[1] & 0xc0) != 0x80 || (c[2] & 0xc0) != 0x80 ||
				(c[3] & 0xc0) != 0x80)
			{
				return false;
			}
			c += 4;
		}
		else
		{
			return false;
		}
	}
	return true;
	unguard;
}

//==========================================================================
//
//	VStr::Latin1ToUtf8
//
//==========================================================================

VStr VStr::Latin1ToUtf8() const
{
	guard(VStr::Latin1ToUtf8);
	VStr Ret;
	for (size_t i = 0; i < Length(); i++)
	{
		Ret += FromChar((vuint8)Str[i]);
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::EvalEscapeSequences
//
//==========================================================================

VStr VStr::EvalEscapeSequences() const
{
	guard(VStr::EvalEscapeSequences);
	VStr Ret;
	char Val;
	for (const char* c = **this; *c; c++)
	{
		if (*c == '\\')
		{
			c++;
			switch (*c)
			{
			case 't':
				Ret += '\t';
				break;
			case 'n':
				Ret += '\n';
				break;
			case 'r':
				Ret += '\r';
				break;
			case 'c':
				Ret += TEXT_COLOUR_ESCAPE;
				break;
			case 'x':
				Val = 0;
				c++;
				for (int i = 0; i < 2; i++)
				{
					if (*c >= '0' && *c <= '9')
						Val = (Val << 4) + *c - '0';
					else if (*c >= 'a' && *c <= 'f')
						Val = (Val << 4) + 10 + *c - 'a';
					else if (*c >= 'A' && *c <= 'F')
						Val = (Val << 4) + 10 + *c - 'A';
					else
						break;
					c++;
				}
				c--;
				Ret += Val;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				Val = 0;
				for (int i = 0; i < 3; i++)
				{
					if (*c >= '0' && *c <= '7')
						Val = (Val << 3) + *c - '0';
					else
						break;
					c++;
				}
				c--;
				Ret += Val;
				break;
			case '\n':
				break;
			case 0:
				c--;
				break;
			default:
				Ret += *c;
				break;
			}
		}
		else
		{
			Ret += *c;
		}
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::RemoveColours
//
//==========================================================================

VStr VStr::RemoveColours() const
{
	guard(VStr::RemoveColours);
	VStr Ret;
	for (const char* c = **this; *c; c++)
	{
		if (*c == TEXT_COLOUR_ESCAPE)
		{
			if (c[1])
			{
				c++;
			}
			if (*c == '[')
			{
				while (c[1] && *c != ']')
				{
					c++;
				}
			}
			continue;
		}
		Ret += *c;
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::ExtractFilePath
//
//==========================================================================

VStr VStr::ExtractFilePath() const
{
	guard(FL_ExtractFilePath);
	const char* src = Str + Length() - 1;

	//
	// back up until a \ or the start
	//
	while (src != Str && *(src - 1) != '/' && *(src - 1) != '\\')
		src--;

	return VStr(*this, 0, src - Str);
	unguard;
}

//==========================================================================
//
//	VStr:ExtractFileName
//
//==========================================================================

VStr VStr::ExtractFileName() const
{
	guard(VStr:ExtractFileName);
	const char* src = Str + Length() - 1;

	//
	// back up until a \ or the start
	//
	while (src != Str && *(src - 1) != '/' && *(src - 1) != '\\')
		src--;

	return src;
	unguard;
}

//==========================================================================
//
//	VStr::ExtractFileBase
//
//==========================================================================

VStr VStr::ExtractFileBase() const
{
	guard(VStr::ExtractFileBase);
	int i = int(Length() - 1);

	// back up until a \ or the start
	while (i && Str[i - 1] != '\\' && Str[i - 1] != '/')
	{
		i--;
	}

	// copy up to eight characters
	int start = i;
	int length = 0;
	while (Str[i] && Str[i] != '.')
	{
		if (++length == 9)
			Sys_Error("Filename base of %s >8 chars", Str);
		i++;
	}
	return VStr(*this, start, length);
	unguard;
}

//==========================================================================
//
//	VStr::ExtractFileExtension
//
//==========================================================================

VStr VStr::ExtractFileExtension() const
{
	guard(VStr::ExtractFileExtension);
	const char* src = Str + Length() - 1;

	//
	// back up until a . or the start
	//
	while (src != Str && *(src - 1) != '.')
		src--;
	if (src == Str)
	{
		return VStr();	// no extension
	}

	return src;
	unguard;
}

//==========================================================================
//
//	VStr::StripExtension
//
//==========================================================================

VStr VStr::StripExtension() const
{
	guard(VStr::StripExtension);
	const char* search = Str + Length() - 1;
	while (*search != '/' && *search != '\\' && search != Str)
	{
		if (*search == '.')
		{
			return VStr(*this, 0, search - Str);
		}
		search--;
	}
	return *this;
	unguard;
}

//==========================================================================
//
//	VStr::DefaultPath
//
//==========================================================================

VStr VStr::DefaultPath(const VStr& basepath) const
{
	guard(VStr::DefaultPath);
	if (Str[0] == '/')
	{
		return *this;	// absolute path location
	}
	return basepath + *this;
	unguard;
}

//==========================================================================
//
//	VStr.DefaultExtension
//
//==========================================================================

VStr VStr::DefaultExtension(const VStr& extension) const
{
	guard(VStr::DefaultExtension);
	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	const char* src = Str + Length() - 1;

	while (*src != '/' && *src != '\\' && src != Str)
	{
		if (*src == '.')
        {
			return *this;	// it has an extension
		}
		src--;
	}

	return *this + extension;
	unguard;
}

//==========================================================================
//
//	VStr::FixFileSlashes
//
//==========================================================================

VStr VStr::FixFileSlashes() const
{
	guard(VStr::FixFileSlashes);
	VStr Ret(*this);
	int l = int(Length());
	for (int i = 0; i < l; i++)
	{
		if (Ret[i] == '\\')
			Ret[i] = '/';
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::Length
//
//==========================================================================

size_t VStr::Length(const char* S)
{
	return strlen(S);
}

//==========================================================================
//
//	VStr::Utf8Length
//
//==========================================================================

size_t VStr::Utf8Length(const char* S)
{
	guard(VStr::Utf8Length);
	size_t Count = 0;
	for (const char* c = S; *c; c++)
		if ((*c & 0xc0) != 0x80)
			Count++;
	return Count;
	unguard;
}

//==========================================================================
//
//	VStr::ByteLengthForUtf8
//
//==========================================================================

size_t VStr::ByteLengthForUtf8(const char* S, size_t N)
{
	guard(VStr::ByteLengthForUtf8);
	size_t Count = 0;
	const char* c;
	for (c = S; *c; c++)
	{
		if ((*c & 0xc0) != 0x80)
		{
			if (Count == N)
			{
				return c - S;
			}
			Count++;
		}
	}
	check(N == Count);
	return c - S;
	unguard;
}

//==========================================================================
//
//	VStr::GetChar
//
//==========================================================================

int VStr::GetChar(const char*& S)
{
	guard(VStr::GetChar);
	if ((vuint8)*S < 128)
	{
		return *S++;
	}
	int Cnt;
	int Val;
	if ((*S & 0xe0) == 0xc0)
	{
		Val = *S & 0x1f;
		Cnt = 1;
	}
	else if ((*S & 0xf0) == 0xe0)
	{
		Val = *S & 0x0f;
		Cnt = 2;
	}
	else if ((*S & 0xf8) == 0xf0)
	{
		Val = *S & 0x07;
		Cnt = 3;
	}
	else
	{
		Sys_Error("Not a valid UTF-8");
		return 0;
	}
	S++;

	do
	{
		if ((*S & 0xc0) != 0x80)
			Sys_Error("Not a valid UTF-8");
		Val = (Val << 6) | (*S & 0x3f);
		S++;
	}
	while (--Cnt);
	return Val;
	unguard;
}

//==========================================================================
//
//	VStr::FromChar
//
//==========================================================================

VStr VStr::FromChar(int C)
{
	guard(VStr::FromChar);
	char Ret[8];
	if (C < 0x80)
	{
		Ret[0] = C;
		Ret[1] = 0;
	}
	else if (C < 0x800)
	{
		Ret[0] = 0xc0 | (C & 0x1f);
		Ret[1] = 0x80 | ((C >> 5) & 0x3f);
		Ret[2] = 0;
	}
	else if (C < 0x10000)
	{
		Ret[0] = 0xe0 | (C & 0x0f);
		Ret[1] = 0x80 | ((C >> 4) & 0x3f);
		Ret[2] = 0x80 | ((C >> 10) & 0x3f);
		Ret[3] = 0;
	}
	else
	{
		Ret[0] = 0xf0 | (C & 0x07);
		Ret[1] = 0x80 | ((C >> 3) & 0x3f);
		Ret[2] = 0x80 | ((C >> 9) & 0x3f);
		Ret[3] = 0x80 | ((C >> 15) & 0x3f);
		Ret[4] = 0;
	}
	return Ret;
	unguard;
}

//==========================================================================
//
//	VStr::Cmp
//
//==========================================================================

int VStr::Cmp(const char* S1, const char* S2)
{
	return strcmp(S1, S2);
}

//==========================================================================
//
//	VStr::NCmp
//
//==========================================================================

int VStr::NCmp(const char* S1, const char* S2, size_t N)
{
	return strncmp(S1, S2, N);
}

//==========================================================================
//
//	VStr::ICmp
//
//==========================================================================

int VStr::ICmp(const char* S1, const char* S2)
{
	#ifdef WIN32
		return _stricmp(S1, S2);
	#else
		return stricmp(S1, S2);
	#endif
}

//==========================================================================
//
//	VStr::NICmp
//
//==========================================================================

int VStr::NICmp(const char* S1, const char* S2, size_t N)
{
	#ifdef WIN32
		return _strnicmp(S1, S2, N);
	#else
		return strnicmp(S1, S2, N);
	#endif
}

//==========================================================================
//
//	VStr::Cpy
//
//==========================================================================

void VStr::Cpy(char* Dst, const char* Src)
{
	strcpy(Dst, Src);
}

//==========================================================================
//
//	VStr::NCpy
//
//==========================================================================

void VStr::NCpy(char* Dst, const char* Src, size_t N)
{
	strncpy(Dst, Src, N);
}

//==========================================================================
//
//	VStr::ToUpper
//
//==========================================================================

char VStr::ToUpper(char C)
{
	return toupper(C);
}

//==========================================================================
//
//	VStr::ToLower
//
//==========================================================================

char VStr::ToLower(char C)
{
	return tolower(C);
}

//==========================================================================
//
//	va
//
//	Very usefull function from QUAKE
//	Does a varargs printf into a temp buffer, so I don't need to have
// varargs versions of all text functions.
//	FIXME: make this buffer size safe someday
//
//==========================================================================

char *va(const char *text, ...)
{
	va_list args;

	va_bufnum = (va_bufnum + 1) & 3;
	va_start(args, text);
	vsprintf(va_buffer[va_bufnum], text, args);
	va_end(args);

	return va_buffer[va_bufnum];
}

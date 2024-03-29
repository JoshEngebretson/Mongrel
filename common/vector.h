//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vector.h 4212 2010-04-03 20:13:36Z dj_jl $
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

// HEADER FILES ------------------------------------------------------------

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

class  TVec
{
public:
	float		x;
	float		y;
	float		z;

	TVec(void) { }

	TVec(float Ax, float Ay, float Az)
	{
		x = Ax;
		y = Ay;
		z = Az;
	}

	TVec (const float f[3])
	{
    	x = f[0];
    	y = f[1];
    	z = f[2];
	}

	const float& operator[](int i) const
	{
    	return (&x)[i];
	}

	float& operator[](int i)
	{
	    return (&x)[i];
	}

	TVec &operator += (const TVec &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	TVec &operator -= (const TVec &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	TVec &operator *= (float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	TVec &operator /= (float scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}

	TVec operator + (void) const
	{
		return *this;
	}

	TVec operator - (void) const
	{
		return TVec(-x,	-y, -z);
	}

	float Length(void) const
	{
		return sqrt(x * x + y * y + z * z);
	}
};

inline TVec operator + (const TVec &v1, const TVec &v2)
{
	return TVec(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline TVec operator - (const TVec &v1, const TVec &v2)
{
	return TVec(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline TVec operator * (const TVec& v, float s)
{
	return TVec(s * v.x, s * v.y, s * v.z);
}

inline TVec operator * (float s, const TVec& v)
{
	return TVec(s * v.x, s * v.y, s * v.z);
}

inline TVec operator / (const TVec& v, float s)
{
	return TVec(v.x / s, v.y / s, v.z / s);
}

inline bool operator == (const TVec& v1, const TVec& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline bool operator != (const TVec& v1, const TVec& v2)
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

inline float Length(const TVec &v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline TVec Normalise(const TVec& v)
{
   return v / v.Length();
}

inline float DotProduct(const TVec& v1, const TVec& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline TVec CrossProduct(const TVec &v1, const TVec &v2)
{
	return TVec(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x);
}

inline VStream& operator << (VStream &Strm, TVec &v)
{
	return Strm << v.x << v.y << v.z;
}

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PUBLIC DATA DECLARATIONS ------------------------------------------------

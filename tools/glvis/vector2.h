//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vector.h 1741 2006-09-26 23:26:48Z dj_jl $
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

class  TVec2
{
 public:
	double		x;
	double		y;

	TVec2() { }

	TVec2(double Ax, double Ay)
	{
		x = Ax;
		y = Ay;
	}

	TVec2 (const double f[2])
	{
		x = f[0];
		y = f[1];
	}

	const double& operator[](int i) const
	{
		return (&x)[i];
	}

	double& operator[](int i)
	{
		return (&x)[i];
	}

	TVec2 &operator += (const TVec2 &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	TVec2 &operator -= (const TVec2 &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	TVec2 &operator *= (double scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	TVec2 &operator /= (double scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}

	TVec2 operator + () const
	{
		return *this;
	}

	TVec2 operator - () const
	{
		return TVec2(-x,	-y);
	}

	double Length() const
	{
		return sqrt(x * x + y * y);
	}
};

inline TVec2 operator + (const TVec2 &v1, const TVec2 &v2)
{
	return TVec2(v1.x + v2.x, v1.y + v2.y);
}

inline TVec2 operator - (const TVec2 &v1, const TVec2 &v2)
{
	return TVec2(v1.x - v2.x, v1.y - v2.y);
}

inline TVec2 operator * (const TVec2& v, double s)
{
	return TVec2(s * v.x, s * v.y);
}

inline TVec2 operator * (double s, const TVec2& v)
{
	return TVec2(s * v.x, s * v.y);
}

inline TVec2 operator / (const TVec2& v, double s)
{
	return TVec2(v.x / s, v.y / s);
}

inline bool operator == (const TVec2& v1, const TVec2& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator != (const TVec2& v1, const TVec2& v2)
{
	return v1.x != v2.x || v1.y != v2.y;
}

inline double Length(const TVec2 &v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

inline TVec2 Normalise(const TVec2& v)
{
   return v / v.Length();
}

inline double DotProduct(const TVec2& v1, const TVec2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

//==========================================================================
//
//								PLANES
//
//==========================================================================

class TPlane
{
 public:
	TVec2		normal;
	double		dist;

	void Set(const TVec2 &Anormal, double Adist)
	{
		normal = Anormal;
		dist = Adist;
	}

	//	Initialises vertical plane from point and direction
	void SetPointDir(const TVec2 &point, const TVec2 &dir)
	{
		normal = Normalise(TVec2(dir.y, -dir.x));
		dist = DotProduct(point, normal);
	}

	//	Initialises vertical plane from 2 points
	void Set2Points(const TVec2 &v1, const TVec2 &v2)
	{
		SetPointDir(v1, v2 - v1);
	}
};

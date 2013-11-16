//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: r_light.cpp 4428 2011-09-03 14:41:19Z firebrand_kh $
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
#include "r_local.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

int					r_dlightframecount;
bool				r_light_add;

vuint32				blocklights[18 * 18];
vuint32				blocklightsr[18 * 18];
vuint32				blocklightsg[18 * 18];
vuint32				blocklightsb[18 * 18];
vuint32				blockaddlightsr[18 * 18];
vuint32				blockaddlightsg[18 * 18];
vuint32				blockaddlightsb[18 * 18];

byte				light_remap[256];
VCvarI				r_darken("r_darken", "0", CVAR_Archive);
VCvarI				r_ambient("r_ambient", "0");
int					light_mem;
VCvarI				r_extrasamples("r_extrasamples", "0", CVAR_Archive);
VCvarI				r_dynamic("r_dynamic", "1", CVAR_Archive);
VCvarI				r_dynamic_clip("r_dynamic_clip", "0", CVAR_Archive);
VCvarI				r_static_lights("r_static_lights", "1", CVAR_Archive);
VCvarI				r_static_add("r_static_add", "0", CVAR_Archive);
VCvarF				r_specular("r_specular", "0.1", CVAR_Archive);

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static TVec			smins, smaxs;
static TVec			worldtotex[2];
static TVec			textoworld[2];
static TVec			texorg;
static TVec			surfpt[18 * 18 * 4];
static int			numsurfpt;
static bool			points_calculated;
static float		lightmap[18 * 18 * 4];
static float		lightmapr[18 * 18 * 4];
static float		lightmapg[18 * 18 * 4];
static float		lightmapb[18 * 18 * 4];
static bool			light_hit;
static byte			*facevis;
static bool			is_coloured;

static int			c_bad;

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	VRenderLevelShared::AddStaticLight
//
//==========================================================================

void VRenderLevelShared::AddStaticLight(const TVec &origin, float radius,
	vuint32 colour)
{
	guard(VRenderLevelShared::AddStaticLight);
	light_t& L = Lights.Alloc();
	L.origin = origin;
	L.radius = radius;
	L.colour = colour;
	L.leafnum = Level->PointInSubsector(origin) - Level->Subsectors;
	unguard;
}

//**************************************************************************
//**
//**	DYNAMIC LIGHTS
//**
//**************************************************************************

//==========================================================================
//
//	VRenderLevelShared::AllocDlight
//
//==========================================================================

dlight_t* VRenderLevelShared::AllocDlight(VThinker* Owner)
{
	guard(VRenderLevelShared::AllocDlight);
	int			i;
	dlight_t*	dl;

	// first look for an exact key match
	if (Owner)
	{
		dl = DLights;
		for (i = 0; i < MAX_DLIGHTS; i++, dl++)
		{
            // 64 bit fix
            if (dl->OwnerIndex == (Owner->GetIndex() + 1) )
			{
				memset(dl, 0, sizeof(*dl));
                dl->OwnerIndex = dl->OwnerIndex = Owner->GetIndex() + 1;
				return dl;
			}
		}
	}

	// then look for anything else
	dl = DLights;
	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (dl->die < Level->Time)
		{
			memset(dl, 0, sizeof(*dl));
            if (Owner)
                dl->OwnerIndex = Owner->GetIndex() + 1;
			return dl;
		}
	}

	int bestnum = 0;
	float bestdist = 0.0;
	dl = DLights;
	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		float dist = Length(dl->origin - cl->ViewOrg);
		if (dist > bestdist)
		{
			bestnum = i;
			bestdist = dist;
		}
	}
	dl = &DLights[bestnum];
	memset(dl, 0, sizeof(*dl));
    if (Owner)
        dl->OwnerIndex = Owner->GetIndex() + 1;
	return dl;
	unguard;
}

//==========================================================================
//
//	VRenderLevelShared::DecayLights
//
//==========================================================================

void VRenderLevelShared::DecayLights(float time)
{
	guard(VRenderLevelShared::DecayLights);
	dlight_t* dl = DLights;
	for (int i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (dl->die < Level->Time || !dl->radius)
		{
			continue;
		}

		dl->radius -= time * dl->decay;
		if (dl->radius < 0)
		{
			dl->radius = 0;
		}
	}
	unguard;
}




//==========================================================================
//
//	VRenderLevelShared::FreeSurfCache
//
//==========================================================================

void VRenderLevelShared::FreeSurfCache(surfcache_t*)
{
}



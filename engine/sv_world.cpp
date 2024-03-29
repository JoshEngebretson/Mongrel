//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: sv_world.cpp 4428 2011-09-03 14:41:19Z firebrand_kh $
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
//**	Movement/collision utility functions, as used by function in
//**  p_map.c. BLOCKMAP Iterator functions, and some PIT_* functions to use
//**  for iteration.
//**
//**************************************************************************

// HEADER FILES ------------------------------------------------------------

#include "gamedefs.h"
#include "sv_local.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static opening_t	openings[32];

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	SV_LineOpenings
//
//	Sets opentop and openbottom to the window through a two sided line.
//
//==========================================================================

opening_t *SV_LineOpenings(const line_t* linedef, const TVec& point, int NoBlockFlags)
{
	guard(SV_LineOpenings);
	opening_t		*op;
	int				opsused;
	sec_region_t	*frontreg;
	sec_region_t	*backreg;
	sec_plane_t		*frontfloor = NULL;
	sec_plane_t		*backfloor = NULL;
	sec_plane_t		*frontceil = NULL;
	sec_plane_t		*backceil = NULL;
	float			frontfloorz;
	float			backfloorz;
	float			frontceilz;
	float			backceilz;

	if (linedef->sidenum[1] == -1)
	{
		// single sided line
		return NULL;
	}

	op = NULL;
	opsused = 0;
	frontreg = linedef->frontsector->botregion;
	backreg = linedef->backsector->botregion;

	while (frontreg && backreg)
	{
		if (!(frontreg->floor->flags & NoBlockFlags))
			frontfloor = frontreg->floor;
		if (!(backreg->floor->flags & NoBlockFlags))
			backfloor = backreg->floor;
		if (!(frontreg->ceiling->flags & NoBlockFlags))
			frontceil = frontreg->ceiling;
		if (!(backreg->ceiling->flags & NoBlockFlags))
			backceil = backreg->ceiling;
		if (backreg->ceiling->flags & NoBlockFlags)
		{
			backreg = backreg->next;
			continue;
		}
		if (frontreg->ceiling->flags & NoBlockFlags)
		{
			frontreg = frontreg->next;
			continue;
		}
		frontfloorz = frontfloor->GetPointZ(point);
		backfloorz = backfloor->GetPointZ(point);
		frontceilz = frontceil->GetPointZ(point);
		backceilz = backceil->GetPointZ(point);
		if (frontfloorz >= backceilz)
		{
			backreg = backreg->next;
			continue;
		}
		if (backfloorz >= frontceilz)
		{
			frontreg = frontreg->next;
			continue;
		}
		openings[opsused].next = op;
		op = &openings[opsused];
		opsused++;
		if (frontfloorz > backfloorz)
		{
			op->bottom = frontfloorz;
			op->lowfloor = backfloorz;
			op->floor = frontfloor;
			op->lowfloorplane = backfloor;
		}
		else
		{
			op->bottom = backfloorz;
			op->lowfloor = frontfloorz;
			op->floor = backfloor;
			op->lowfloorplane = frontfloor;
		}
		if (frontceilz < backceilz)
		{
			op->top = frontceilz;
			op->highceiling = backceilz;
			op->ceiling = frontceil;
			op->highceilingplane = backceil;
			frontreg = frontreg->next;
		}
		else
		{
			op->top = backceilz;
			op->highceiling = frontceilz;
			op->ceiling = backceil;
			op->highceilingplane = frontceil;
			backreg = backreg->next;
		}
		op->range = op->top - op->bottom;
	}
	return op;
	unguard;
}

//==========================================================================
//
//	P_BoxOnLineSide
//
//	Considers the line to be infinite
//	Returns side 0 or 1, -1 if box crosses the line.
//
//==========================================================================

int P_BoxOnLineSide(float* tmbox, line_t* ld)
{
	guard(P_BoxOnLineSide);
	int		p1 = 0;
	int		p2 = 0;
	
	switch (ld->slopetype)
	{
	case ST_HORIZONTAL:
		p1 = tmbox[BOXTOP] > ld->v1->y;
		p2 = tmbox[BOXBOTTOM] > ld->v1->y;
		if (ld->dir.x < 0)
		{
			p1 ^= 1;
			p2 ^= 1;
		}
		break;
	
	case ST_VERTICAL:
		p1 = tmbox[BOXRIGHT] < ld->v1->x;
		p2 = tmbox[BOXLEFT] < ld->v1->x;
		if (ld->dir.y < 0)
		{
			p1 ^= 1;
			p2 ^= 1;
		}
		break;
	
	case ST_POSITIVE:
		p1 = ld->PointOnSide(TVec(tmbox[BOXLEFT], tmbox[BOXTOP], 0));
		p2 = ld->PointOnSide(TVec(tmbox[BOXRIGHT], tmbox[BOXBOTTOM], 0));
		break;
	
	case ST_NEGATIVE:
		p1 = ld->PointOnSide(TVec(tmbox[BOXRIGHT], tmbox[BOXTOP], 0));
		p2 = ld->PointOnSide(TVec(tmbox[BOXLEFT], tmbox[BOXBOTTOM], 0));
		break;
	}

	if (p1 == p2)
		return p1;
	return -1;
	unguard;
}

//==========================================================================
//
//	SV_FindThingGap
//
// Find the best gap that the thing could fit in, given a certain Z
// position (z1 is foot, z2 is head).  Assuming at least two gaps exist,
// the best gap is chosen as follows:
//
// 1. if the thing fits in one of the gaps without moving vertically,
//    then choose that gap.
//
// 2. if there is only *one* gap which the thing could fit in, then
//    choose that gap.
//
// 3. if there is multiple gaps which the thing could fit in, choose
//    the gap whose floor is closest to the thing's current Z.
//
// 4. if there is no gaps which the thing could fit in, do the same.
//
// Returns the gap number, or -1 if there are no gaps at all.
//
//==========================================================================

sec_region_t *SV_FindThingGap(sec_region_t* InGaps, const TVec &point, float z1, float z2)
{
	guard(SV_FindThingGap);
	sec_region_t* gaps = InGaps;
	float dist;

	int fit_num = 0;
	sec_region_t *fit_last = NULL;

	sec_region_t *fit_closest = NULL;
	float fit_mindist = 200000.0;

	sec_region_t *nofit_closest = NULL;
	float nofit_mindist = 200000.0;

	// check for trivial gaps...
	if (gaps == NULL)
	{
		return NULL;
	}
	if (gaps->next == NULL)
	{
		return gaps;
	}

	sec_plane_t *floor = NULL;
	sec_plane_t *ceil = NULL;

	// There are 2 or more gaps. Now it gets interesting :-)
	while (gaps != NULL)
	{
		float	f;
		float	c;

		if (gaps->floor->flags == 0)
		{
			floor = gaps->floor;
		}
		if (gaps->ceiling->flags == 0)
		{
			ceil = gaps->ceiling;
		}
		if (gaps->ceiling->flags)
		{
			gaps = gaps->next;
			continue;
		}
		f = floor->GetPointZ(point);
		c = ceil->GetPointZ(point);
		if (z1 >= f && z2 <= c)
		{ // [1]
			return gaps;
		}

		dist = fabs(z1 - f);

		if (z2 - z1 <= c - f)
		{ // [2]
			fit_num++;

			fit_last = gaps;
			if (dist < fit_mindist)
			{ // [3]
				fit_mindist = dist;
				fit_closest = gaps;
			}
		}
		else
		{
			if (dist < nofit_mindist)
			{ // [4]
				nofit_mindist = dist;
				nofit_closest = gaps;
			}
		}
		gaps = gaps->next;
	}

	if (fit_num == 1)
	{
		return fit_last;
	}
	if (fit_num > 1)
	{
		return fit_closest;
	}
	return nofit_closest;
	unguard;
}

//==========================================================================
//
//	SV_FindOpening
//
// Find the best gap that the thing could fit in, given a certain Z
// position (z1 is foot, z2 is head).  Assuming at least two gaps exist,
// the best gap is chosen as follows:
//
// 1. if the thing fits in one of the gaps without moving vertically,
//    then choose that gap.
//
// 2. if there is only *one* gap which the thing could fit in, then
//    choose that gap.
//
// 3. if there is multiple gaps which the thing could fit in, choose
//    the gap whose floor is closest to the thing's current Z.
//
// 4. if there is no gaps which the thing could fit in, do the same.
//
// Returns the gap number, or -1 if there are no gaps at all.
//
//==========================================================================

opening_t *SV_FindOpening(opening_t* InGaps, float z1, float z2)
{
	guard(SV_FindOpening);
	opening_t* gaps = InGaps;
	float dist;

	int fit_num = 0;
	opening_t *fit_last = NULL;

	opening_t *fit_closest = NULL;
	float fit_mindist = 99999.0;

	opening_t *nofit_closest = NULL;
	float nofit_mindist = 99999.0;

	// check for trivial gaps...
	if (!gaps)
	{
		return NULL;
	}
	if (!gaps->next)
	{
		return gaps;
	}

	// There are 2 or more gaps. Now it gets interesting :-)
	while (gaps)
	{
		float	f;
		float	c;

		f = gaps->bottom;
		c = gaps->top;
		if (z1 >= f && z2 <= c)
		{ // [1]
			return gaps;
		}

		dist = fabs(z1 - f);

		if (z2 - z1 <= c - f)
		{ // [2]
			fit_num++;

			fit_last = gaps;
			if (dist < fit_mindist)
			{ // [3]
				fit_mindist = dist;
				fit_closest = gaps;
			}
		}
		else
		{
			if (dist < nofit_mindist)
			{ // [4]
				nofit_mindist = dist;
				nofit_closest = gaps;
			}
		}
		gaps = gaps->next;
	}

	if (fit_num == 1)
		return fit_last;
	if (fit_num > 1)
		return fit_closest;
	return nofit_closest;
	unguard;
}

//==========================================================================
//
//	SV_PointInRegions
//
//==========================================================================

sec_region_t *SV_PointInRegion(sector_t *sector, const TVec &p)
{
	guard(SV_PointInRegion);
	sec_region_t *reg;

	// logic: find matching region, otherwise return highest one.
	for (reg = sector->botregion; reg && reg->next; reg = reg->next)
	{
		if (p.z < reg->ceiling->GetPointZ(p))
			break;
	}

	return reg;
	unguard;
}

//==========================================================================
//
//	SV_PointContents
//
//==========================================================================

int SV_PointContents(const sector_t *sector, const TVec &p)
{
	guard(SV_PointContents);
	check(sector);
	if (sector->heightsec &&
		(sector->heightsec->SectorFlags & sector_t::SF_UnderWater) &&
		p.z <= sector->heightsec->floor.GetPointZ(p))
	{
		return 9;
	}
	if (sector->SectorFlags & sector_t::SF_UnderWater)
	{
		return 9;
	}

	if (sector->SectorFlags & sector_t::SF_HasExtrafloors)
	{
		for (sec_region_t* reg = sector->botregion; reg; reg = reg->next)
		{
			if (p.z <= reg->ceiling->GetPointZ(p) &&
				p.z >= reg->floor->GetPointZ(p))
			{
				return reg->params->contents;
			}
		}
		return -1;
	}
	else
	{
		return sector->botregion->params->contents;
	}
	unguard;
}

//**************************************************************************
//
//  SECTOR HEIGHT CHANGING
//
//  After modifying a sectors floor or ceiling height, call this routine to
// adjust the positions of all things that touch the sector.
//  If anything doesn't fit anymore, true will be returned. If crunch is
// true, they will take damage as they are being crushed.
//  If Crunch is false, you should set the sector height back the way it was
// and call P_ChangeSector again to undo the changes.
//
//**************************************************************************

//==========================================================================
//
//	VLevel::ChangeSector
//
//	jff 3/19/98 added to just check monsters on the periphery of a moving
// sector instead of all in bounding box of the sector. Both more accurate
// and faster.
//
//==========================================================================

bool VLevel::ChangeSector(sector_t* sector, int crunch)
{
	guard(VLevel::ChangeSector);
	sector_t* sec2;
	sec_region_t* reg;
	msecnode_t* n;

	CalcSecMinMaxs(sector);

	bool ret = false;

	//	killough 4/4/98: scan list front-to-back until empty or exhausted,
	// restarting from beginning after each thing is processed. Avoids
	// crashes, and is sure to examine all things in the sector, and only
	// the things which are in the sector, until a steady-state is reached.
	// Things can arbitrarily be inserted and removed and it won't mess up.
	//
	// killough 4/7/98: simplified to avoid using complicated counter

	//	Mark all things invalid.
	for (n = sector->TouchingThingList; n; n = n->SNext)
	{
		n->Visited = false;
	}

	do
	{
		//	Go through list.
		for (n = sector->TouchingThingList; n; n = n->SNext)
		{
			if (!n->Visited)
			{
				//	Unprocessed thing found, mark thing as processed.
				n->Visited = true;
				//	jff 4/7/98 Don't do these.
				if (!(n->Thing->EntityFlags & VEntity::EF_NoBlockmap))
				{
					//	Process it.
					if (!n->Thing->eventSectorChanged(crunch))
					{
						//	Doesn't fit, keep checking (crush other things)
						ret = true;
					}
				}
				//	Exit and start over
				break;
			}
		}
	}
	//	Repeat from scratch until all things left are marked valid
	while (n);

	if (sector->SectorFlags & sector_t::SF_ExtrafloorSource)
	{
		for (int i = 0; i < NumSectors; i++)
		{
			sec2 = &Sectors[i];
			if (sec2->SectorFlags & sector_t::SF_HasExtrafloors && sec2 != sector)
			{
				for (reg = sec2->botregion; reg; reg = reg->next)
				{
					if (reg->floor == &sector->floor ||
						reg->ceiling == &sector->ceiling)
					{
						ret |= ChangeSector(sec2, crunch);
						break;
					}
				}
			}
		}
	}
	return ret;
	unguard;
}

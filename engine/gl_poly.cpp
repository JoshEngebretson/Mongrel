//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: gl_poly.cpp 4428 2011-09-03 14:41:19Z firebrand_kh $
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

#include "gl_local.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	glVertex
//
//==========================================================================

inline void glVertex(const TVec &v)
{
	glVertex3f(v.x, v.y, v.z);
}

//==========================================================================
//
//	VOpenGLDrawer::WorldDrawingShaders
//
//==========================================================================

void VOpenGLDrawer::WorldDrawing()
{
    guard(VOpenGLDrawer::WorldDrawing);
	surfcache_t	*cache;
	surface_t	*surf;
	texinfo_t	*tex;

	//	First draw horizons.
	if (RendLev->HorizonPortalsHead)
	{
		for (surf = RendLev->HorizonPortalsHead; surf; surf = surf->DrawNext)
		{
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}

			DoHorizonPolygon(surf);
		}
	}

    RenderProcSky(NULL, NULL);

	//	For sky areas we just write to the depth buffer to prevent drawing
	// polygons behind the sky.
    /*
	if (RendLev->SkyPortalsHead)
    {

		p_glUseProgramObjectARB(SurfZBufProgram);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		for (surf = RendLev->SkyPortalsHead; surf; surf = surf->DrawNext)
		{
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}

			glBegin(GL_POLYGON);
			for (int i = 0; i < surf->count; i++)
			{
				glVertex(surf->verts[i]);
			}
			glEnd();
		}
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    }
      */

	//	Draw surfaces.
	if (RendLev->SimpleSurfsHead)
	{
		p_glUseProgramObjectARB(SurfSimpleProgram);
		p_glUniform1iARB(SurfSimpleTextureLoc, 0);
		p_glUniform1iARB(SurfSimpleFogTypeLoc, r_fog & 3);

		for (surf = RendLev->SimpleSurfsHead; surf; surf = surf->DrawNext)
		{
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}

			texinfo_t* tex = surf->texinfo;
			SetTexture(tex->Tex, tex->ColourMap);
			p_glUniform3fvARB(SurfSimpleSAxisLoc, 1, &tex->saxis.x);
			p_glUniform1fARB(SurfSimpleSOffsLoc, tex->soffs);
			p_glUniform1fARB(SurfSimpleTexIWLoc, tex_iw);
			p_glUniform3fvARB(SurfSimpleTAxisLoc, 1, &tex->taxis.x);
			p_glUniform1fARB(SurfSimpleTOffsLoc, tex->toffs);
			p_glUniform1fARB(SurfSimpleTexIHLoc, tex_ih);

			float lev = float(surf->Light >> 24) / 255.0;
			p_glUniform4fARB(SurfSimpleLightLoc,
				((surf->Light >> 16) & 255) * lev / 255.0,
				((surf->Light >> 8) & 255) * lev / 255.0,
				(surf->Light & 255) * lev / 255.0, 1.0);
			if (surf->Fade)
			{
				p_glUniform1iARB(SurfSimpleFogEnabledLoc, GL_TRUE);
				p_glUniform4fARB(SurfSimpleFogColourLoc,
					((surf->Fade >> 16) & 255) / 255.0,
					((surf->Fade >> 8) & 255) / 255.0,
					(surf->Fade & 255) / 255.0, 1.0);
				p_glUniform1fARB(SurfSimpleFogDensityLoc, surf->Fade == FADE_LIGHT ? 0.3 : r_fog_density);
				p_glUniform1fARB(SurfSimpleFogStartLoc, surf->Fade == FADE_LIGHT ? 1.0 : r_fog_start);
				p_glUniform1fARB(SurfSimpleFogEndLoc, surf->Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);
			}
			else
			{
				p_glUniform1iARB(SurfSimpleFogEnabledLoc, GL_FALSE);
			}

			glBegin(GL_POLYGON);
			for (int i = 0; i < surf->count; i++)
			{
				glVertex(surf->verts[i]);
			}
			glEnd();
		}
	}

	p_glUseProgramObjectARB(SurfLightmapProgram);
	p_glUniform1iARB(SurfLightmapTextureLoc, 0);
	p_glUniform1iARB(SurfLightmapLightMapLoc, 1);
	p_glUniform1iARB(SurfLightmapSpecularMapLoc, 2);
	p_glUniform1iARB(SurfLightmapFogTypeLoc, r_fog & 3);

	for (int lb = 0; lb < NUM_BLOCK_SURFS; lb++)
	{
		if (!RendLev->light_chain[lb])
		{
			continue;
		}

		SelectTexture(1);
		glBindTexture(GL_TEXTURE_2D, lmap_id[lb]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (RendLev->block_changed[lb])
		{
			RendLev->block_changed[lb] = false;
			glTexImage2D(GL_TEXTURE_2D, 0, 4, BLOCK_WIDTH, BLOCK_HEIGHT,
				0, GL_RGBA, GL_UNSIGNED_BYTE, RendLev->light_block[lb]);
			RendLev->add_changed[lb] = true;
		}

		SelectTexture(2);
		glBindTexture(GL_TEXTURE_2D, addmap_id[lb]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (RendLev->add_changed[lb])
		{
			RendLev->add_changed[lb] = false;
			glTexImage2D(GL_TEXTURE_2D, 0, 4, BLOCK_WIDTH, BLOCK_HEIGHT,
				0, GL_RGBA, GL_UNSIGNED_BYTE, RendLev->add_block[lb]);
		}

		SelectTexture(0);

		for (cache = RendLev->light_chain[lb]; cache; cache = cache->chain)
		{
			surf = cache->surf;
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}
			tex = surf->texinfo;
			SetTexture(tex->Tex, tex->ColourMap);

			p_glUniform3fvARB(SurfLightmapSAxisLoc, 1, &tex->saxis.x);
			p_glUniform1fARB(SurfLightmapSOffsLoc, tex->soffs);
			p_glUniform1fARB(SurfLightmapTexIWLoc, tex_iw);
			p_glUniform3fvARB(SurfLightmapTAxisLoc, 1, &tex->taxis.x);
			p_glUniform1fARB(SurfLightmapTOffsLoc, tex->toffs);
			p_glUniform1fARB(SurfLightmapTexIHLoc, tex_ih);
			p_glUniform1fARB(SurfLightmapTexMinSLoc, surf->texturemins[0]);
			p_glUniform1fARB(SurfLightmapTexMinTLoc, surf->texturemins[1]);
			p_glUniform1fARB(SurfLightmapCacheSLoc, cache->s);
			p_glUniform1fARB(SurfLightmapCacheTLoc, cache->t);

			if (surf->Fade)
			{
				p_glUniform1iARB(SurfLightmapFogEnabledLoc, GL_TRUE);
				p_glUniform4fARB(SurfLightmapFogColourLoc,
					((surf->Fade >> 16) & 255) / 255.0,
					((surf->Fade >> 8) & 255) / 255.0,
					(surf->Fade & 255) / 255.0, 1.0);
				p_glUniform1fARB(SurfLightmapFogDensityLoc, surf->Fade == FADE_LIGHT ? 0.3 : r_fog_density);
				p_glUniform1fARB(SurfLightmapFogStartLoc, surf->Fade == FADE_LIGHT ? 1.0 : r_fog_start);
				p_glUniform1fARB(SurfLightmapFogEndLoc, surf->Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);
			}
			else
			{
				p_glUniform1iARB(SurfLightmapFogEnabledLoc, GL_FALSE);
			}

			glBegin(GL_POLYGON);
			for (int i = 0; i < surf->count; i++)
			{
				glVertex(surf->verts[i]);
			}
			glEnd();
		}
	}
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawWorldAmbientPass
//
//==========================================================================

void VOpenGLDrawer::DrawWorldAmbientPass()
{
	guard(VOpenGLDrawer::DrawWorldAmbientPass);
	//	First draw horizons.
	if (RendLev->HorizonPortalsHead)
	{
		for (surface_t* surf = RendLev->HorizonPortalsHead; surf; surf = surf->DrawNext)
		{
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}

			DoHorizonPolygon(surf);
		}
	}

	if (RendLev->SkyPortalsHead)
	{
		p_glUseProgramObjectARB(SurfZBufProgram);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		for (surface_t* surf = RendLev->SkyPortalsHead; surf; surf = surf->DrawNext)
		{
			float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
			if (dist <= 0)
			{
				//	Viewer is in back side or on plane
				continue;
			}

			glBegin(GL_POLYGON);
			for (int i = 0; i < surf->count; i++)
			{
				glVertex(surf->verts[i]);
			}
			glEnd();
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	p_glUseProgramObjectARB(ShadowsAmbientProgram);
	p_glUniform1iARB(ShadowsAmbientTextureLoc, 0);
	for (surface_t* surf = RendLev->SimpleSurfsHead; surf; surf = surf->DrawNext)
	{
		float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
		if (dist <= 0)
		{
			//	Viewer is in back side or on plane
			continue;
		}
		texinfo_t* tex = surf->texinfo;
		SetTexture(tex->Tex, tex->ColourMap);
		p_glUniform3fvARB(ShadowsAmbientSAxisLoc, 1, &tex->saxis.x);
		p_glUniform1fARB(ShadowsAmbientSOffsLoc, tex->soffs);
		p_glUniform1fARB(ShadowsAmbientTexIWLoc, tex_iw);
		p_glUniform3fvARB(ShadowsAmbientTAxisLoc, 1, &tex->taxis.x);
		p_glUniform1fARB(ShadowsAmbientTOffsLoc, tex->toffs);
		p_glUniform1fARB(ShadowsAmbientTexIHLoc, tex_ih);

		float lev = float(surf->Light >> 24) / 255.0;
		p_glUniform4fARB(ShadowsAmbientLightLoc,
			((surf->Light >> 16) & 255) * lev / 255.0,
			((surf->Light >> 8) & 255) * lev / 255.0,
			(surf->Light & 255) * lev / 255.0, 1.0);
		glBegin(GL_POLYGON);
		for (int i = 0; i < surf->count; i++)
		{
			glVertex(surf->verts[i]);
		}
		glEnd();
	}
	unguard;
}


//==========================================================================
//
//	VOpenGLDrawer::BeginShadowVolumesPass
//
//==========================================================================

void VOpenGLDrawer::BeginShadowVolumesPass()
{
	guard(VOpenGLDrawer::BeginShadowVolumesPass);
	//	Set up for shadow volume rendering.
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::BeginLightShadowVolumes
//
//==========================================================================

void VOpenGLDrawer::BeginLightShadowVolumes()
{
	guard(VOpenGLDrawer::BeginLightShadowVolumes);
	//	Set up for shadow volume rendering.
	glClear(GL_STENCIL_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(0.0f, 10.0f);
	glDepthFunc(GL_LESS);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
	p_glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
	p_glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);

	p_glUseProgramObjectARB(SurfZBufProgram);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::RenderSurfaceShadowVolume
//
//==========================================================================

void VOpenGLDrawer::RenderSurfaceShadowVolume(surface_t *surf, TVec& LightPos, float Radius)
{
	guard(VOpenGLDrawer::RenderSurfaceShadowVolume);
	int i;
	TArray<TVec>    v;
	if (surf->plane->PointOnSide(LightPos))
	{
		//	Light is in back side or on plane
		return;
	}
	float dist = DotProduct(LightPos, surf->plane->normal) - surf->plane->dist;
	if (dist >= Radius)
	{
		//	Light is too far away
		return;
	}
	v.SetNum(surf->count);

	for (i = 0; i < surf->count; i++)
	{
		v[i] = Normalise(surf->verts[i] - LightPos);
		v[i] *= M_INFINITY;
		v[i] += LightPos;
	}

	glBegin(GL_POLYGON);
	for (i = surf->count - 1; i >= 0; i--)
	{
		glVertex(v[i]);
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (i = 0; i < surf->count; i++)
	{
		glVertex(surf->verts[i]);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i < surf->count; i++)
	{
		glVertex(surf->verts[i]);
		glVertex(v[i]);
	}
	glVertex(surf->verts[0]);
	glVertex(v[0]);
	glEnd();
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::BeginLightPass
//
//==========================================================================

void VOpenGLDrawer::BeginLightPass(TVec& LightPos, float Radius, vuint32 Colour)
{
	guard(VOpenGLDrawer::BeginLightPass);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDepthFunc(GL_LEQUAL);

	glStencilFunc(GL_EQUAL, 0x0, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	p_glUseProgramObjectARB(ShadowsLightProgram);
	p_glUniform3fARB(ShadowsLightLightPosLoc, LightPos.x, LightPos.y, LightPos.z);
	p_glUniform1fARB(ShadowsLightLightRadiusLoc, Radius);
	p_glUniform3fARB(ShadowsLightLightColourLoc,
		((Colour >> 16) & 255) / 255.0,
		((Colour >> 8) & 255) / 255.0,
		(Colour & 255) / 255.0);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawSurfaceLight
//
//==========================================================================

void VOpenGLDrawer::DrawSurfaceLight(surface_t* Surf, TVec& LightPos, float Radius)
{
	guard(VOpenGLDrawer::DrawSurfaceLight);
	if (Surf->plane->PointOnSide(LightPos))
	{
		//	Light is in back side or on plane
		return;
	}
	float dist = DotProduct(LightPos, Surf->plane->normal) - Surf->plane->dist;
	if (dist >= Radius)
	{
		//	Light is too far away
		return;
	}
	p_glUniform1iARB(ShadowsAmbientTextureLoc, 0);
	texinfo_t* tex = Surf->texinfo;
	SetTexture(tex->Tex, tex->ColourMap);
	p_glUniform3fvARB(ShadowsAmbientSAxisLoc, 1, &tex->saxis.x);
	p_glUniform1fARB(ShadowsAmbientSOffsLoc, tex->soffs);
	p_glUniform1fARB(ShadowsAmbientTexIWLoc, tex_iw);
	p_glUniform3fvARB(ShadowsAmbientTAxisLoc, 1, &tex->taxis.x);
	p_glUniform1fARB(ShadowsAmbientTOffsLoc, tex->toffs);
	p_glUniform1fARB(ShadowsAmbientTexIHLoc, tex_ih);

	glBegin(GL_POLYGON);
	for (int i = 0; i < Surf->count; i++)
	{
		p_glVertexAttrib3fvARB(ShadowsLightSurfNormalLoc, &Surf->plane->normal.x);
		p_glVertexAttrib1fvARB(ShadowsLightSurfDistLoc, &Surf->plane->dist);
		glVertex(Surf->verts[i]);
	}
	glEnd();
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawWorldTexturesPass
//
//==========================================================================

void VOpenGLDrawer::DrawWorldTexturesPass()
{
	guard(VOpenGLDrawer::DrawWorldTexturesPass);
	//	Stop stenciling now.
	glDisable(GL_STENCIL_TEST);

	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glEnable(GL_BLEND);

	p_glUseProgramObjectARB(ShadowsTextureProgram);
	p_glUniform1iARB(ShadowsTextureTextureLoc, 0);

	for (surface_t* surf = RendLev->SimpleSurfsHead; surf; surf = surf->DrawNext)
	{
		float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
		if (dist <= 0)
		{
			//	Viewer is in back side or on plane
			continue;
		}

		texinfo_t* tex = surf->texinfo;
		SetTexture(tex->Tex, tex->ColourMap);
		glBegin(GL_POLYGON);
		for (int i = 0; i < surf->count; i++)
		{
			p_glVertexAttrib2fARB(ShadowsTextureTexCoordLoc,
				(DotProduct(surf->verts[i], tex->saxis) + tex->soffs) * tex_iw,
				(DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
			glVertex(surf->verts[i]);
		}
		glEnd();
	}
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawWorldFogPass
//
//==========================================================================

void VOpenGLDrawer::DrawWorldFogPass()
{
	guard(VOpenGLDrawer::DrawWorldFogPass);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	Draw surfaces.
	p_glUseProgramObjectARB(ShadowsFogProgram);
	p_glUniform1iARB(ShadowsFogFogTypeLoc, r_fog & 3);

	for (surface_t* surf = RendLev->SimpleSurfsHead; surf; surf = surf->DrawNext)
	{
		float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
		if (dist <= 0)
		{
			//	Viewer is in back side or on plane
			continue;
		}
		if (!surf->Fade)
		{
			continue;
		}

		p_glUniform4fARB(ShadowsFogFogColourLoc,
			((surf->Fade >> 16) & 255) / 255.0,
			((surf->Fade >> 8) & 255) / 255.0,
			(surf->Fade & 255) / 255.0, 1.0);
		p_glUniform1fARB(ShadowsFogFogDensityLoc, surf->Fade == FADE_LIGHT ? 0.3 : r_fog_density);
		p_glUniform1fARB(ShadowsFogFogStartLoc, surf->Fade == FADE_LIGHT ? 1.0 : r_fog_start);
		p_glUniform1fARB(ShadowsFogFogEndLoc, surf->Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);

		glBegin(GL_POLYGON);
		for (int i = 0; i < surf->count; i++)
		{
			glVertex(surf->verts[i]);
		}
		glEnd();
	}
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::EndFogPass
//
//==========================================================================

void VOpenGLDrawer::EndFogPass()
{
	guard(VOpenGLDrawer::EndFogPass);
	glDisable(GL_BLEND);

	//	Back to normal z-buffering.
	glDepthMask(GL_TRUE);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DoHorizonPolygon
//
//==========================================================================

void VOpenGLDrawer::DoHorizonPolygon(surface_t* Surf)
{
	guard(VOpenGLDrawer::DoHorizonPolygon);
	float Dist = 4096.0;
	TVec v[4];
	if (Surf->HorizonPlane->normal.z > 0.0)
	{
		v[0] = Surf->verts[0];
		v[3] = Surf->verts[3];
		TVec HDir = -Surf->plane->normal;

		TVec Dir1 = Normalise(vieworg - Surf->verts[1]);
		TVec Dir2 = Normalise(vieworg - Surf->verts[2]);
		float Mul1 = 1.0 / DotProduct(HDir, Dir1);
		v[1] = Surf->verts[1] + Dir1 * Mul1 * Dist;
		float Mul2 = 1.0 / DotProduct(HDir, Dir2);
		v[2] = Surf->verts[2] + Dir2 * Mul2 * Dist;
		if (v[1].z < v[0].z)
		{
			v[1] = Surf->verts[1] + Dir1 * Mul1 * Dist * (Surf->verts[1].z -
				Surf->verts[0].z) / (Surf->verts[1].z - v[1].z);
			v[2] = Surf->verts[2] + Dir2 * Mul2 * Dist * (Surf->verts[2].z -
				Surf->verts[3].z) / (Surf->verts[2].z - v[2].z);
		}
	}
	else
	{
		v[1] = Surf->verts[1];
		v[2] = Surf->verts[2];
		TVec HDir = -Surf->plane->normal;

		TVec Dir1 = Normalise(vieworg - Surf->verts[0]);
		TVec Dir2 = Normalise(vieworg - Surf->verts[3]);
		float Mul1 = 1.0 / DotProduct(HDir, Dir1);
		v[0] = Surf->verts[0] + Dir1 * Mul1 * Dist;
		float Mul2 = 1.0 / DotProduct(HDir, Dir2);
		v[3] = Surf->verts[3] + Dir2 * Mul2 * Dist;
		if (v[1].z < v[0].z)
		{
			v[0] = Surf->verts[0] + Dir1 * Mul1 * Dist * (Surf->verts[1].z -
				Surf->verts[0].z) / (v[0].z - Surf->verts[0].z);
			v[3] = Surf->verts[3] + Dir2 * Mul2 * Dist * (Surf->verts[2].z -
				Surf->verts[3].z) / (v[3].z - Surf->verts[3].z);
		}
	}

	texinfo_t* Tex = Surf->texinfo;
	SetTexture(Tex->Tex, Tex->ColourMap);

    p_glUseProgramObjectARB(SurfSimpleProgram);
    p_glUniform1iARB(SurfSimpleTextureLoc, 0);
    p_glUniform1iARB(SurfSimpleFogTypeLoc, r_fog & 3);

    p_glUniform3fvARB(SurfSimpleSAxisLoc, 1, &Tex->saxis.x);
    p_glUniform1fARB(SurfSimpleSOffsLoc, Tex->soffs);
    p_glUniform1fARB(SurfSimpleTexIWLoc, tex_iw);
    p_glUniform3fvARB(SurfSimpleTAxisLoc, 1, &Tex->taxis.x);
    p_glUniform1fARB(SurfSimpleTOffsLoc, Tex->toffs);
    p_glUniform1fARB(SurfSimpleTexIHLoc, tex_ih);

    float lev = float(Surf->Light >> 24) / 255.0;
    p_glUniform4fARB(SurfSimpleLightLoc,
        ((Surf->Light >> 16) & 255) * lev / 255.0,
        ((Surf->Light >> 8) & 255) * lev / 255.0,
        (Surf->Light & 255) * lev / 255.0, 1.0);
    if (Surf->Fade)
    {
        p_glUniform1iARB(SurfSimpleFogEnabledLoc, GL_TRUE);
        p_glUniform4fARB(SurfSimpleFogColourLoc,
            ((Surf->Fade >> 16) & 255) / 255.0,
            ((Surf->Fade >> 8) & 255) / 255.0,
            (Surf->Fade & 255) / 255.0, 1.0);
        p_glUniform1fARB(SurfSimpleFogDensityLoc, Surf->Fade == FADE_LIGHT ? 0.3 : r_fog_density);
        p_glUniform1fARB(SurfSimpleFogStartLoc, Surf->Fade == FADE_LIGHT ? 1.0 : r_fog_start);
        p_glUniform1fARB(SurfSimpleFogEndLoc, Surf->Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);
    }
    else
    {
        p_glUniform1iARB(SurfSimpleFogEnabledLoc, GL_FALSE);
    }

    //	Draw it
    glDepthMask(GL_FALSE);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 4; i++)
    {
        glVertex(v[i]);
    }
    glEnd();
    glDepthMask(GL_TRUE);

    //	Write to the depth buffer.
    p_glUseProgramObjectARB(SurfZBufProgram);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glBegin(GL_POLYGON);
    for (int i = 0; i < Surf->count; i++)
    {
        glVertex(Surf->verts[i]);
    }
    glEnd();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawSkyPolygon
//
//==========================================================================

void VOpenGLDrawer::DrawSkyPolygon(surface_t* surf, bool bIsSkyBox,
	VTexture* Texture1, float offs1, VTexture* Texture2, float offs2,
	int CMap)
{
	guard(VOpenGLDrawer::DrawSkyPolygon);

    // TODO: renable as we may want custom skyboxes
    return;

	int		i;
	int		sidx[4];

	SetFade(surf->Fade);
	sidx[0] = 0;
	sidx[1] = 1;
	sidx[2] = 2;
	sidx[3] = 3;
	if (!bIsSkyBox)
	{
		if (surf->verts[1].z > 0)
		{
			sidx[1] = 0;
			sidx[2] = 3;
		}
		else
		{
			sidx[0] = 1;
			sidx[3] = 2;
		}
	}
	texinfo_t *tex = surf->texinfo;
    if (Texture2->Type != TEXTYPE_Null)
    {
        SetTexture(Texture1, CMap);
        SelectTexture(1);
        SetTexture(Texture2, CMap);
        SelectTexture(0);

        p_glUseProgramObjectARB(SurfDSkyProgram);
        p_glUniform1iARB(SurfDSkyTextureLoc, 0);
        p_glUniform1iARB(SurfDSkyTexture2Loc, 1);
        p_glUniform1fARB(SurfDSkyBrightnessLoc, r_sky_bright_factor);

        glBegin(GL_POLYGON);
        for (i = 0; i < surf->count; i++)
        {
            p_glVertexAttrib2fARB(SurfDSkyTexCoordLoc,
                (DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs1) * tex_iw,
                (DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
            p_glVertexAttrib2fARB(SurfDSkyTexCoord2Loc,
                (DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs2) * tex_iw,
                (DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
            glVertex(surf->verts[i]);
        }
        glEnd();
	}
	else
	{
		if (HaveMultiTexture && Texture2->Type != TEXTYPE_Null)
		{
			SetTexture(Texture1, CMap);
			SelectTexture(1);
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			SetTexture(Texture2, CMap);
			SelectTexture(0);

			glColor4f(r_sky_bright_factor * 1, r_sky_bright_factor * 1, r_sky_bright_factor * 1, 1);
			glBegin(GL_POLYGON);
			for (i = 0; i < surf->count; i++)
			{
				MultiTexCoord(0, 
					(DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs1) * tex_iw,
					(DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
				MultiTexCoord(1, 
					(DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs2) * tex_iw,
					(DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
				glVertex(surf->verts[i]);
			}
			glEnd();

			SelectTexture(1);
			glDisable(GL_TEXTURE_2D);
			SelectTexture(0);
		}
		else
		{
			SetTexture(Texture1, CMap);
			glBegin(GL_POLYGON);
			glColor4f(r_sky_bright_factor * 1, r_sky_bright_factor * 1, r_sky_bright_factor * 1, 1);
			for (i = 0; i < surf->count; i++)
			{
				glTexCoord2f(
					(DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs1) * tex_iw,
					(DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
				glVertex(surf->verts[i]);
			}
			glEnd();

			if (Texture2->Type != TEXTYPE_Null)
			{
				SetTexture(Texture2, CMap);
				glEnable(GL_BLEND);
				glBegin(GL_POLYGON);
				glColor4f(1, 1, 1, 1);
				for (i = 0; i < surf->count; i++)
				{
					glTexCoord2f(
						(DotProduct(surf->verts[sidx[i]], tex->saxis) + tex->soffs - offs2) * tex_iw,
						(DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
					glVertex(surf->verts[i]);
				}
				glEnd();
				glDisable(GL_BLEND);
			}
		}
	}
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawMaskedPolygon
//
//==========================================================================

void VOpenGLDrawer::DrawMaskedPolygon(surface_t* surf, float Alpha,
	bool Additive)
{
	guard(VOpenGLDrawer::DrawMaskedPolygon);
	float dist = DotProduct(vieworg, surf->plane->normal) - surf->plane->dist;
	if (dist <= 0)
	{
		//	Viewer is in back side or on plane
		return;
	}

	texinfo_t* tex = surf->texinfo;
	SetTexture(tex->Tex, tex->ColourMap);

    p_glUseProgramObjectARB(SurfMaskedProgram);
    p_glUniform1iARB(SurfMaskedTextureLoc, 0);
    p_glUniform1iARB(SurfMaskedFogTypeLoc, r_fog & 3);

    if (surf->lightmap != NULL ||
        surf->dlightframe == r_dlightframecount)
    {
        RendLev->BuildLightMap(surf, 0);
        int w = (surf->extents[0] >> 4) + 1;
        int h = (surf->extents[1] >> 4) + 1;
        int size = w * h;
        int r = 0;
        int g = 0;
        int b = 0;
        for (int i = 0; i < size; i++)
        {
            r += 255 * 256 - blocklightsr[i];
            g += 255 * 256 - blocklightsg[i];
            b += 255 * 256 - blocklightsb[i];
        }
        double iscale = 1.0 / (size * 255 * 256);
        p_glUniform4fARB(SurfMaskedLightLoc, r * iscale, g * iscale, b * iscale, Alpha);
    }
    else
    {
        float lev = float(surf->Light >> 24) / 255.0;
        p_glUniform4fARB(SurfMaskedLightLoc,
            ((surf->Light >> 16) & 255) * lev / 255.0,
            ((surf->Light >> 8) & 255) * lev / 255.0,
            (surf->Light & 255) * lev / 255.0, Alpha);
    }
    if (surf->Fade)
    {
        p_glUniform1iARB(SurfMaskedFogEnabledLoc, GL_TRUE);
        p_glUniform4fARB(SurfMaskedFogColourLoc,
            ((surf->Fade >> 16) & 255) / 255.0,
            ((surf->Fade >> 8) & 255) / 255.0,
            (surf->Fade & 255) / 255.0, Alpha);
        p_glUniform1fARB(SurfMaskedFogDensityLoc, surf->Fade == FADE_LIGHT ? 0.3 : r_fog_density);
        p_glUniform1fARB(SurfMaskedFogStartLoc, surf->Fade == FADE_LIGHT ? 1.0 : r_fog_start);
        p_glUniform1fARB(SurfMaskedFogEndLoc, surf->Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);
    }
    else
    {
        p_glUniform1iARB(SurfMaskedFogEnabledLoc, GL_FALSE);
    }

    if (blend_sprites || Additive || Alpha < 1.0)
    {
        p_glUniform1fARB(SurfMaskedAlphaRefLoc, 0.111);
        glEnable(GL_BLEND);
    }
    else
    {
        p_glUniform1fARB(SurfMaskedAlphaRefLoc, 0.333);
    }
    if (Additive)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    glBegin(GL_POLYGON);
    for (int i = 0; i < surf->count; i++)
    {
        p_glVertexAttrib2fARB(SurfMaskedTexCoordLoc,
            (DotProduct(surf->verts[i], tex->saxis) + tex->soffs) * tex_iw,
            (DotProduct(surf->verts[i], tex->taxis) + tex->toffs) * tex_ih);
        glVertex(surf->verts[i]);
    }
    glEnd();

    if (blend_sprites || Additive || Alpha < 1.0)
    {
        glDisable(GL_BLEND);
    }
    if (Additive)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawSpritePolygon
//
//==========================================================================

void VOpenGLDrawer::DrawSpritePolygon(TVec *cv, VTexture* Tex, float Alpha,
	bool Additive, VTextureTranslation* Translation, int CMap, vuint32 light,
	vuint32 Fade, const TVec&, float, const TVec& saxis, const TVec& taxis,
	const TVec& texorg)
{
	guard(VOpenGLDrawer::DrawSpritePolygon);
	TVec	texpt;

	SetSpriteLump(Tex, Translation, CMap);

    p_glUseProgramObjectARB(SurfMaskedProgram);
    p_glUniform1iARB(SurfMaskedTextureLoc, 0);
    p_glUniform1iARB(SurfMaskedFogTypeLoc, r_fog & 3);

    p_glUniform4fARB(SurfMaskedLightLoc,
        ((light >> 16) & 255) / 255.0,
        ((light >> 8) & 255) / 255.0,
        (light & 255) / 255.0, Alpha);
    if (Fade)
    {
        p_glUniform1iARB(SurfMaskedFogEnabledLoc, GL_TRUE);
        p_glUniform4fARB(SurfMaskedFogColourLoc,
            ((Fade >> 16) & 255) / 255.0,
            ((Fade >> 8) & 255) / 255.0,
            (Fade & 255) / 255.0, Alpha);
        p_glUniform1fARB(SurfMaskedFogDensityLoc, Fade == FADE_LIGHT ? 0.3 : r_fog_density);
        p_glUniform1fARB(SurfMaskedFogStartLoc, Fade == FADE_LIGHT ? 1.0 : r_fog_start);
        p_glUniform1fARB(SurfMaskedFogEndLoc, Fade == FADE_LIGHT ? 1024.0 * r_fade_factor : r_fog_end);
    }
    else
    {
        p_glUniform1iARB(SurfMaskedFogEnabledLoc, GL_FALSE);
    }

    if (blend_sprites || Additive || Alpha < 1.0)
    {
        p_glUniform1fARB(SurfMaskedAlphaRefLoc, 0.111);
        glEnable(GL_BLEND);
    }
    else
    {
        p_glUniform1fARB(SurfMaskedAlphaRefLoc, 0.333);
    }
    if (Additive)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    glBegin(GL_QUADS);

    texpt = cv[0] - texorg;
    p_glVertexAttrib2fARB(SurfMaskedTexCoordLoc,
        DotProduct(texpt, saxis) * tex_iw,
        DotProduct(texpt, taxis) * tex_ih);
    glVertex(cv[0]);

    texpt = cv[1] - texorg;
    p_glVertexAttrib2fARB(SurfMaskedTexCoordLoc,
        DotProduct(texpt, saxis) * tex_iw,
        DotProduct(texpt, taxis) * tex_ih);
    glVertex(cv[1]);

    texpt = cv[2] - texorg;
    p_glVertexAttrib2fARB(SurfMaskedTexCoordLoc,
        DotProduct(texpt, saxis) * tex_iw,
        DotProduct(texpt, taxis) * tex_ih);
    glVertex(cv[2]);

    texpt = cv[3] - texorg;
    p_glVertexAttrib2fARB(SurfMaskedTexCoordLoc,
        DotProduct(texpt, saxis) * tex_iw,
        DotProduct(texpt, taxis) * tex_ih);
    glVertex(cv[3]);

    glEnd();

    if (blend_sprites || Additive || Alpha < 1.0)
    {
        glDisable(GL_BLEND);
    }
    if (Additive)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::StartParticles
//
//==========================================================================

void VOpenGLDrawer::StartParticles()
{
	guard(VOpenGLDrawer::StartParticles);
	glEnable(GL_BLEND);
    p_glUseProgramObjectARB(SurfPartProgram);
    glBegin(GL_QUADS);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawParticle
//
//==========================================================================

void VOpenGLDrawer::DrawParticle(particle_t *p)
{
	guard(VOpenGLDrawer::DrawParticle);
    float r = ((p->colour >> 16) & 255) / 255.0;
    float g = ((p->colour >> 8) & 255) / 255.0;
    float b = (p->colour & 255) / 255.0;
    float a = ((p->colour >> 24) & 255) / 255.0;
    p_glVertexAttrib4fARB(SurfPartLightValLoc, r, g, b, a);
    p_glVertexAttrib2fARB(SurfPartTexCoordLoc, -1, -1);
    glVertex(p->org - viewright * p->Size + viewup * p->Size);
    p_glVertexAttrib4fARB(SurfPartLightValLoc, r, g, b, a);
    p_glVertexAttrib2fARB(SurfPartTexCoordLoc, 1, -1);
    glVertex(p->org + viewright * p->Size + viewup * p->Size);
    p_glVertexAttrib4fARB(SurfPartLightValLoc, r, g, b, a);
    p_glVertexAttrib2fARB(SurfPartTexCoordLoc, 1, 1);
    glVertex(p->org + viewright * p->Size - viewup * p->Size);
    p_glVertexAttrib4fARB(SurfPartLightValLoc, r, g, b, a);
    p_glVertexAttrib2fARB(SurfPartTexCoordLoc, -1, 1);
    glVertex(p->org - viewright * p->Size - viewup * p->Size);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::EndParticles
//
//==========================================================================

void VOpenGLDrawer::EndParticles()
{
	guard(VOpenGLDrawer::EndParticles);
	glEnd();
	glDisable(GL_BLEND);
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::StartPortal
//
//==========================================================================

bool VOpenGLDrawer::StartPortal(VPortal* Portal, bool UseStencil)
{
	guard(VOpenGLDrawer::StartPortal);
	if (UseStencil)
	{
		//	Doesn't work for now.
		if (RendLev->NeedsInfiniteFarClip)
		{
			return false;
		}

        p_glUseProgramObjectARB(SurfZBufProgram);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		//	Set up stencil test.
		if (!RendLev->PortalDepth)
		{
			glEnable(GL_STENCIL_TEST);
		}
		glStencilFunc(GL_EQUAL, RendLev->PortalDepth, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		//	Mark the portal area.
		DrawPortalArea(Portal);

		//	Set up stencil test for portal
		glStencilFunc(GL_EQUAL, RendLev->PortalDepth + 1, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		if (Portal->NeedsDepthBuffer())
		{
			glDepthMask(GL_TRUE);
			//	Clear depth buffer
			glDepthRange(1, 1);
			glDepthFunc(GL_ALWAYS);
			DrawPortalArea(Portal);
			glDepthFunc(GL_LEQUAL);
			glDepthRange(0, 1);
		}
		else
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}

		//	Enable drawing.
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		RendLev->PortalDepth++;
	}
	else
	{
		if (!Portal->NeedsDepthBuffer())
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}
	}
	return true;
	unguard;
}

//==========================================================================
//
//	VOpenGLDrawer::DrawPortalArea
//
//==========================================================================

void VOpenGLDrawer::DrawPortalArea(VPortal* Portal)
{
	guard(VOpenGLDrawer::DrawPortalArea);
	for (int i = 0; i < Portal->Surfs.Num(); i++)
	{
		const surface_t* Surf = Portal->Surfs[i];
		glBegin(GL_POLYGON);
		for (int j = 0; j < Surf->count; j++)
		{
			glVertex(Surf->verts[j]);
		}
		glEnd();
	}
	unguard;
}

//==========================================================================
//
//	VSoftwareDrawer::EndPortal
//
//==========================================================================

void VOpenGLDrawer::EndPortal(VPortal* Portal, bool UseStencil)
{
	guard(VOpenGLDrawer::EndPortal);
    p_glUseProgramObjectARB(SurfZBufProgram);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	if (UseStencil)
	{
		if (Portal->NeedsDepthBuffer())
		{
			//	Clear depth buffer
			glDepthRange(1, 1);
			glDepthFunc(GL_ALWAYS);
			DrawPortalArea(Portal);
			glDepthFunc(GL_LEQUAL);
			glDepthRange(0, 1);
		}
		else
		{
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
		}

		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

		//	Draw proper z-buffer for the portal area.
		glDepthFunc(GL_ALWAYS);
		DrawPortalArea(Portal);
		glDepthFunc(GL_LEQUAL);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		RendLev->PortalDepth--;
		glStencilFunc(GL_EQUAL, RendLev->PortalDepth, ~0);
		if (!RendLev->PortalDepth)
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
	else
	{
		if (Portal->NeedsDepthBuffer())
		{
			//	Clear depth buffer
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
		}

		//	Draw proper z-buffer for the portal area.
		DrawPortalArea(Portal);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    unguard;
}

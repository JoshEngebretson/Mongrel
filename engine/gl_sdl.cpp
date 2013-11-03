//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: gl_sdl.cpp 4346 2010-12-17 14:27:56Z dj_jl $
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

#include <SDL.h>
#include "gl_local.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

static SDL_Window* window = NULL;

SDL_Window* mgGetSDLWindow()
{
    return window;
}

class VSdlOpenGLDrawer : public VOpenGLDrawer
{
public:
    SDL_Renderer *renderer;
    // Create an OpenGL context associated with the window.
    bool contextInitialized;
    SDL_GLContext context;


	void Init();
	bool SetResolution(int, int, int, bool);
	void* GetExtFuncPtr(const char*);
	void Update();
	void Shutdown();
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

IMPLEMENT_DRAWER(VSdlOpenGLDrawer, DRAWER_OpenGL, "OpenGL",
	"SDL OpenGL rasteriser device", "-opengl");

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------

//==========================================================================
//
//	VSdlOpenGLDrawer::Init
//
// 	Determine the hardware configuration
//
//==========================================================================

void VSdlOpenGLDrawer::Init()
{
    window = NULL;
    renderer = NULL;
    context = NULL;
    contextInitialized = false;
}

//==========================================================================
//
//	VSdlOpenGLDrawer::SetResolution
//
//	Set up the video mode
//
//==========================================================================

bool VSdlOpenGLDrawer::SetResolution(int AWidth, int AHeight, int ABPP,
	bool Windowed)
{
	guard(VSdlOpenGLDrawer::SetResolution);
	int Width = AWidth;
	int Height = AHeight;
	int BPP = ABPP;
	if (!Width || !Height)
	{
		// Set defaults
		Width = 640;
		Height = 480;
		BPP = 16;
	}

	if (BPP == 15)
		BPP = 16;

	if (BPP < 16)
	{
		// True-colour only
		return false;
	}

	// Sut down current mode
	Shutdown();

    //Uint32 flags = SDL_OPENGL;
    //if (!Windowed)
    //{
    //	flags |= SDL_FULLSCREEN;
    //}

	HaveStencil = true;

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "Mongrel Engine",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        Width,                               // width, in pixels
        Height,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        Sys_Error("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (renderer == NULL)
   {
       Sys_Error("Count not create renderer");
       return 1;
   }

    context = SDL_GL_CreateContext(window);

    contextInitialized = true;

    /*
    hw_screen = SDL_SetVideoMode(Width, Height, BPP, flags);
	if (!hw_screen)
	{
		//	Try without stencil.
		HaveStencil = false;
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
		hw_screen = SDL_SetVideoMode(Width, Height, BPP, flags);
		if (hw_screen == NULL)
		{
			return false;
		}
	}
    */

	if (HaveStencil)
	{
		GCon->Logf(NAME_Init, "Stencil buffer available");
	}
#if SDL_MINOR_VERSION > 2 || (SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL >= 10)
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, r_vsync);
#endif

	// Everything is fine, set some globals and finish
	ScreenWidth = Width;
	ScreenHeight = Height;
	ScreenBPP = BPP;

	return true;
	unguard;
}

//==========================================================================
//
//	VSdlOpenGLDrawer::GetExtFuncPtr
//
//==========================================================================

void* VSdlOpenGLDrawer::GetExtFuncPtr(const char* name)
{
	guard(VSdlOpenGLDrawer::GetExtFuncPtr);
	return SDL_GL_GetProcAddress(name);
	unguard;
}

//==========================================================================
//
//	VSdlOpenGLDrawer::Update
//
//	Blit to the screen / Flip surfaces
//
//==========================================================================

void VSdlOpenGLDrawer::Update()
{
	guard(VSdlOpenGLDrawer::Update);

    if (window)
        SDL_GL_SwapWindow(window);

	unguard;
}

//==========================================================================
//
//	VSdlOpenGLDrawer::Shutdown
//
//	Close the graphics
//
//==========================================================================

void VSdlOpenGLDrawer::Shutdown()
{
	guard(VSdlOpenGLDrawer::Shutdown);
	DeleteTextures();        

    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        SDL_GL_DeleteContext(context);
    }

    window = NULL;

	unguard;
}


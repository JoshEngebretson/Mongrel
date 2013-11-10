/* Copyright (c) 2012, Jason Lloyd-Price
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */


#include "gl_local.h"

static float sunAngle;
static float sunSize;

static vuint32 topColor;
static vuint32 horizColor;
static vuint32 fogColor;

static vuint32 skyTexId = 0;
static vuint32 sunTexId = 0;
static vuint32 moonTexId = 0;

inline vuint32 interp255( vuint32 bits1, vuint32 bits2, vuint32 shift, vuint32 t )
{
    bits1 = (bits1 >> shift) & 0xff;
    bits2 = (bits2 >> shift) & 0xff;
    return ((bits1 * (0xff-t) + bits2 * t) >> 8) << shift;
}

static vuint32 interpColor( vuint32 c1, vuint32 c2, vuint32 t )
{
    return interp255( c1, c2, 0, t ) | interp255( c1, c2, 8, t ) | interp255( c1, c2, 16, t ) | interp255( c1, c2, 24, t );
}


float VOpenGLDrawer::ProcSkySetTime(float time) {

    sunAngle = (time+0.5f) * M_PI;

    float daylight = cosf( time * M_PI ) * 0.8f + 0.4f;

    if( daylight > 1.0f )
        daylight = 1.0f;

    if( daylight < 0.0f )
        daylight = 0.0f;

    daylight = sqrtf( daylight );

    float dawnGlow;
    int glowX = 0;

    dawnGlow = sinf( time * M_PI * 1.0f );
    glowX = dawnGlow > 0.0f ? 0 : 7;
    dawnGlow = fmax( 0.0f, fabs( dawnGlow * dawnGlow * dawnGlow ) * 1.1f - 0.1f );

    // Daylight colors:
    //0xff1f4a76 // Dark color at the top of the sky
    //0xff659bca // Light color at the bottom of the sky
    //vuint32 topColor = 0xff1f4a76u;
    //vuint32 horizColor = 0xff659bcau;

    vuint32 daylightT = (vuint32)floorf( daylight * 0xff );
    vuint32 top = interpColor( 0xff000000u, topColor, daylightT );
    vuint32 horiz = interpColor( 0xff000000u, horizColor, daylightT );
    fogColor = horiz;

    vuint32 skyTex[32*8];

    // Bottom row is always black
    for( vuint32 x = 0; x < 8; x++ )
        skyTex[x+31*8] = 0xff000000;

    for( vuint32 z = 0; z < 31; z++ )
    {
        for( vuint32 x = 0; x < 8; x++ )
        {
            float xGlow = fmax( 0, -abs((int)x-glowX) + 5 ) / 5.0f;
            float hGlow = z/31.0f;
            vuint32 glow = (vuint32)(dawnGlow * hGlow * hGlow * sqrtf( xGlow ) * 0xff);

            // DEBUG float casts
            vuint32 baseColor = interpColor( top, horiz, (vuint32) fmin( (float) 0xffu, (float) z*11 ) );

            skyTex[x+z*8] = interpColor( baseColor, 0xff9b4016, glow );
        }
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (vuint32 i=  0; i < sizeof(skyTex) / sizeof(*skyTex); ++i)
    skyTex[i] = uint32_t(LittleLong(uint32_t(skyTex[i]))); // DEBUG
#endif

    glBindTexture( GL_TEXTURE_2D, skyTexId );
    glTexImage2D( GL_TEXTURE_2D, 0, 4, 8, 32, 0, GL_BGRA, GL_UNSIGNED_BYTE, &skyTex[0] );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return fmax( 4.0f/15.0f, daylight );
}

void VOpenGLDrawer::RenderProcSky( TPlane* frustum, TVec* nOffset)
{
    TVec n;

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glDisable( GL_DEPTH_TEST );
    glDepthMask( 0 );

    p_glUseProgramObjectARB(ProcSkyProgram);
    p_glUniform1iARB(ProcSkyTextureLoc, 0);
    p_glUniform3fvARB(ProcSkyOffsetVLoc, 1, &nOffset->x);

    glBindTexture( GL_TEXTURE_2D, skyTexId );

    // Planes are in order: lower, upper, left, right
    glBegin( GL_QUADS );

    // Upper left corner
    n = CrossProduct( frustum[2].normal, frustum[1].normal );
    glNormal3fv( &n.x );
    glVertex3f( -1.0f, 1.0f, 0.5f );

    // Lower left corner
    n = CrossProduct( frustum[0].normal, frustum[2].normal );
    glNormal3fv( &n.x );
    glVertex3f( -1.0f, -1.0f, 0.5f );

    // Lower right corner
    n = CrossProduct( frustum[3].normal, frustum[0].normal );
    glNormal3fv( &n.x );
    glVertex3f( 1.0f, -1.0f, 0.5f );

    // Upper right corner
    n = CrossProduct( frustum[1].normal, frustum[3].normal );
    glNormal3fv( &n.x );
    glVertex3f( 1.0f, 1.0f, 0.5f );
    glEnd();

    glEnable( GL_DEPTH_TEST );
    glDepthMask( 1 );

    //skyShader.unbind();

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
}

void VOpenGLDrawer::InitProcSky()
{
    topColor = 0xff1f4a76u;
    horizColor = 0xff659bcau;
    fogColor = horizColor;

    sunSize = 0.5f;

    skyTexId = 0;
    sunTexId = 0;
    moonTexId = 0;

    // create the sky tex
    glGenTextures( 1, &skyTexId );
    glBindTexture( GL_TEXTURE_2D, skyTexId );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    ProcSkySetTime( 0.0f );

}

void VOpenGLDrawer::ShutdownProcSky()
{
    if (skyTexId)
        glDeleteTextures( 1, &skyTexId );

    if( sunTexId )
        glDeleteTextures( 1, &sunTexId );
    if( moonTexId )
        glDeleteTextures( 1, &moonTexId );

    skyTexId = 0;
    sunTexId = 0;
    moonTexId = 0;


}


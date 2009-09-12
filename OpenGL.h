#ifndef OPENGL_H
#define OPENGL_H

# include "winlnxdefs.h"
# include "SDL.h"

//#include "glATI.h"
#include "gSP.h"

#if 0
#define LOG(...)    fprintf(stdout, __VA_ARGS__); fflush(stdout)
#else
#define LOG(...)
#endif

struct GLVertex
{
    float x, y, z, w;
    struct
    {
        float r, g, b, a;
    } color, secondaryColor;
    float s0, t0, s1, t1;
    float fog;
};

struct GLInfo
{
#ifndef __LINUX__
    HGLRC   hRC, hPbufferRC;
    HDC     hDC, hPbufferDC;
    HWND    hWnd;
#endif // __LINUX__

    SDL_Surface *hScreen;

    DWORD   fullscreenWidth, fullscreenHeight, fullscreenBits, fullscreenRefresh;
    DWORD   width, height, windowedWidth, windowedHeight;
    int     heightOffset;

    BOOL    fullscreen, forceBilinear, fog;

    float   scaleX, scaleY;

    BOOL    ATI_texture_env_combine3;   // Radeon
    BOOL    ATIX_texture_env_route;     // Radeon

    BOOL    ARB_multitexture;           // TNT, GeForce, Rage 128, Radeon
    BOOL    ARB_texture_env_combine;    // GeForce, Rage 128, Radeon
    BOOL    ARB_texture_env_crossbar;   // Radeon (GeForce supports it, but doesn't report it)

    BOOL    EXT_fog_coord;              // TNT, GeForce, Rage 128, Radeon
    BOOL    EXT_texture_env_combine;    // TNT, GeForce, Rage 128, Radeon
    BOOL    EXT_secondary_color;        // GeForce, Radeon

    BOOL    NV_texture_env_combine4;    // TNT, GeForce
    BOOL    NV_register_combiners;      // GeForce
    BOOL    ARB_buffer_region;
    BOOL    ARB_pbuffer;
    BOOL    ARB_render_texture;
    BOOL    ARB_pixel_format;

    GLint   maxTextureUnits;            // TNT = 2, GeForce = 2-4, Rage 128 = 2, Radeon = 3-6
    GLint   maxGeneralCombiners;

    BOOL    enable2xSaI;
    BOOL    enableAnisotropicFiltering;
    BOOL    frameBufferTextures;
    int     textureBitDepth;
    float   originAdjust;

    GLVertex vertices[256];
    BYTE    triangles[80][3];
    BYTE    numTriangles;
    BYTE    numVertices;
#ifndef __LINUX__
    HWND    hFullscreenWnd;
#endif
    BOOL    usePolygonStipple;
    GLubyte stipplePattern[32][8][128];
    BYTE    lastStipple;

    BYTE    combiner;
};

extern GLInfo OGL;

struct GLcolor
{
    float r, g, b, a;
};


bool OGL_Start();
void OGL_Stop();
void OGL_AddTriangle( SPVertex *vertices, int v0, int v1, int v2 );
void OGL_DrawTriangles();
void OGL_DrawLine( SPVertex *vertices, int v0, int v1, float width );
void OGL_DrawRect( int ulx, int uly, int lrx, int lry, float *color );
void OGL_DrawTexturedRect( float ulx, float uly, float lrx, float lry, float uls, float ult, float lrs, float lrt, bool flip );
void OGL_UpdateScale();
void OGL_ClearDepthBuffer();
void OGL_ClearColorBuffer( float *color );
void OGL_ResizeWindow();
void OGL_SaveScreenshot();
void OGL_SwapBuffers();
void OGL_ReadScreen( void **dest, int *width, int *height );

#endif


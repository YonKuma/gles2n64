
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <png.h>

#include <SDL.h>
#include <SDL/SDL_syswm.h>
#include <EGL/egl.h>

#define GL_GLEXT_PROTOTYPES
#include <wes_gl.h>

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#define timeGetTime() time(NULL)

#include "winlnxdefs.h"

#include "glN64.h"
#include "OpenGL.h"
#include "Types.h"
#include "N64.h"
#include "gSP.h"
#include "gDP.h"
#include "Textures.h"
#include "Combiner.h"
#include "VI.h"

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

GLInfo OGL;

BOOL isExtensionSupported( const char *extension )
{
    LOG("isExtensionSupported(%d) \n", extension);

    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    extensions = glGetString(GL_EXTENSIONS);

    start = extensions;
    for (;;)
    {
        where = (GLubyte *) strstr((const char *) start, extension);
        if (!where)
            break;

        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return TRUE;

        start = terminator;
    }

    return FALSE;
}

void OGL_InitExtensions()
{
    LOG("OGL_InitExtensions() \n");

    OGL.NV_register_combiners = 0;
    OGL.maxGeneralCombiners = 0;
    OGL.ARB_multitexture = 1;
    OGL.maxTextureUnits = 4;
    OGL.EXT_fog_coord = 0;
    OGL.EXT_secondary_color = 1;

    OGL.ARB_texture_env_combine = 1;
    OGL.ARB_texture_env_crossbar = 0;
    OGL.EXT_texture_env_combine = 1;
    OGL.ATI_texture_env_combine3 = 1;
    OGL.ATIX_texture_env_route = 0;
    OGL.NV_texture_env_combine4 = 0;
}

void OGL_InitStates()
{
    LOG("OGL_InitStates() \n");

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glVertexPointer( 4, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].x );
    glEnableClientState( GL_VERTEX_ARRAY );

    glColorPointer( 4, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].color.r );
    glEnableClientState( GL_COLOR_ARRAY );

    if (OGL.EXT_secondary_color)
    {
        glSecondaryColorPointerEXT( 3, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].secondaryColor.r );
        glEnableClientState( GL_SECONDARY_COLOR_ARRAY_EXT );
    }
    if (OGL.ARB_multitexture)
    {
        glClientActiveTextureARB( GL_TEXTURE0_ARB );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].s0 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        glClientActiveTextureARB( GL_TEXTURE1_ARB );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].s1 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    }
    else
    {
        glTexCoordPointer( 2, GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].s0 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    }

    if (OGL.EXT_fog_coord)
    {
        glFogi( GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT );

        glFogi( GL_FOG_MODE, GL_LINEAR );
        glFogf( GL_FOG_START, 0.0f );
        glFogf( GL_FOG_END, 255.0f );

        glFogCoordPointerEXT( GL_FLOAT, sizeof( GLVertex ), &OGL.vertices[0].fog );
        glEnableClientState( GL_FOG_COORDINATE_ARRAY_EXT );
    }

    glPolygonOffset( -3.0f, -3.0f );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    srand( timeGetTime() );

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
            for (int k = 0; k < 128; k++)
                OGL.stipplePattern[i][j][k] =((i > (rand() >> 10)) << 7) |
                                            ((i > (rand() >> 10)) << 6) |
                                            ((i > (rand() >> 10)) << 5) |
                                            ((i > (rand() >> 10)) << 4) |
                                            ((i > (rand() >> 10)) << 3) |
                                            ((i > (rand() >> 10)) << 2) |
                                            ((i > (rand() >> 10)) << 1) |
                                            ((i > (rand() >> 10)) << 0);
    }

    OGL_SwapBuffers();
}

void OGL_UpdateScale()
{
    LOG("OGL_UpdateScale() \n");

    OGL.scaleX = (float)OGL.width / (float)VI.width;
    OGL.scaleY = (float)OGL.height / (float)VI.height;
}

void OGL_ResizeWindow()
{
    LOG("OGL_ResizeWindow() \n");

#if 0
    RECT    windowRect, statusRect, toolRect;

    if (OGL.fullscreen)
    {
        OGL.width = OGL.fullscreenWidth;
        OGL.height = OGL.fullscreenHeight;
        OGL.heightOffset = 0;

        SetWindowPos( hWnd, NULL, 0, 0, OGL.width, OGL.height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW );
    }
    else
    {
        OGL.width = OGL.windowedWidth;
        OGL.height = OGL.windowedHeight;

        GetClientRect( hWnd, &windowRect );
        GetWindowRect( hStatusBar, &statusRect );

        if (hToolBar)
            GetWindowRect( hToolBar, &toolRect );
        else
            toolRect.bottom = toolRect.top = 0;

        OGL.heightOffset = (statusRect.bottom - statusRect.top);
        windowRect.right = windowRect.left + OGL.windowedWidth - 1;
        windowRect.bottom = windowRect.top + OGL.windowedHeight - 1 + OGL.heightOffset;

        AdjustWindowRect( &windowRect, GetWindowLong( hWnd, GWL_STYLE ), GetMenu( hWnd ) != NULL );

        SetWindowPos( hWnd, NULL, 0, 0, windowRect.right - windowRect.left + 1,
                        windowRect.bottom - windowRect.top + 1 + toolRect.bottom - toolRect.top + 1, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE );
    }
#else // !__LINUX__
#endif // __LINUX__
}

EGLint		VersionMajor;
EGLint		VersionMinor;

EGLDisplay  Display;
EGLContext  Context;
EGLConfig   Config;
EGLSurface  Surface;

EGLNativeDisplayType    Device;
EGLNativeWindowType     Handle;

const EGLint ConfigAttribs[] = {
	EGL_LEVEL,				0,
	EGL_DEPTH_SIZE,         16,
	EGL_STENCIL_SIZE,       0,
	EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
	EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
	EGL_NATIVE_RENDERABLE,	EGL_FALSE,
	EGL_NONE
};

const EGLint ContextAttribs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 	2,
	EGL_NONE
};

const char* EGLErrorString(){
	EGLint nErr = eglGetError();
	switch(nErr){
		case EGL_SUCCESS: 				return "EGL_SUCCESS";
		case EGL_BAD_DISPLAY:			return "EGL_BAD_DISPLAY";
		case EGL_NOT_INITIALIZED:		return "EGL_NOT_INITIALIZED";
		case EGL_BAD_ACCESS:			return "EGL_BAD_ACCESS";
		case EGL_BAD_ALLOC:				return "EGL_BAD_ALLOC";
		case EGL_BAD_ATTRIBUTE:			return "EGL_BAD_ATTRIBUTE";
		case EGL_BAD_CONFIG:			return "EGL_BAD_CONFIG";
		case EGL_BAD_CONTEXT:			return "EGL_BAD_CONTEXT";
		case EGL_BAD_CURRENT_SURFACE:	return "EGL_BAD_CURRENT_SURFACE";
		case EGL_BAD_MATCH:				return "EGL_BAD_MATCH";
		case EGL_BAD_NATIVE_PIXMAP:		return "EGL_BAD_NATIVE_PIXMAP";
		case EGL_BAD_NATIVE_WINDOW:		return "EGL_BAD_NATIVE_WINDOW";
		case EGL_BAD_PARAMETER:			return "EGL_BAD_PARAMETER";
		case EGL_BAD_SURFACE:			return "EGL_BAD_SURFACE";
		default:						return "unknown";
	}
};

bool OGL_Start()
{
    LOG("OGL_Start() \n");
    EGLint nConfigs;

    const SDL_VideoInfo *videoInfo;

    if (OGL.fullscreen)
    {
        OGL.width = OGL.fullscreenWidth;
        OGL.height = OGL.fullscreenHeight;
    }
    else
    {
        OGL.width = OGL.windowedWidth;
        OGL.height = OGL.windowedHeight;
    }
    OGL.heightOffset = -16;

    /* Initialize SDL */
    printf( "[glN64]: (II) Initializing SDL video subsystem...\n" );
    if (SDL_InitSubSystem( SDL_INIT_VIDEO ) == -1)
    {
        printf( "[glN64]: (EE) Error initializing SDL video subsystem: %s\n", SDL_GetError() );
        return FALSE;
    }

    /* Video Info */
    printf( "[glN64]: (II) Getting video info...\n" );
    if (!(videoInfo = SDL_GetVideoInfo()))
    {
        printf( "[glN64]: (EE) Video query failed: %s\n", SDL_GetError() );
        SDL_QuitSubSystem( SDL_INIT_VIDEO );
        return FALSE;
    }

    /* Set the video mode */
    printf( "[glN64]: (II) Setting video mode %dx%d...\n", (int)OGL.width, (int)OGL.height );
    if (!(OGL.hScreen = SDL_SetVideoMode( OGL.width, OGL.height - 32, 0, SDL_SWSURFACE )))
    {
        printf( "[glN64]: (EE) Error setting videomode %dx%d: %s\n", (int)OGL.width, (int)OGL.height, SDL_GetError() );
        SDL_QuitSubSystem( SDL_INIT_VIDEO );
        return FALSE;
    }
    SDL_WM_SetCaption( pluginName, pluginName );

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	Handle = (EGLNativeWindowType) info.window;

#ifdef _WIN32
	Device = GetDC(Handle);
#else
    Device = EGL_DEFAULT_DEVICE;
#endif

    Display = eglGetDisplay((EGLNativeDisplayType) Device);
    if (Display == EGL_NO_DISPLAY){
        LOG( "\nDisplay Get failed: %s", EGLErrorString()); fflush(stdout);
    }

    if (!eglInitialize(Display, &VersionMajor, &VersionMinor)){
        LOG( "\nDisplay Initialize failed: %s", EGLErrorString()); fflush(stdout);
    }

    if (!eglChooseConfig(Display, ConfigAttribs, &Config, 1, &nConfigs)){
        LOG( "\nConfiguration failed: %s", EGLErrorString()); fflush(stdout);
    } else if (nConfigs != 1){
        LOG( "\nConfiguration failed: nconfig %i, %s", nConfigs, EGLErrorString()); fflush(stdout);
    }

    Surface = eglCreateWindowSurface(Display, Config, Handle, NULL);
    if (Surface == EGL_NO_SURFACE){
		LOG( "\nSurface Creation failed: %s will attempt without window...", EGLErrorString()); fflush(stdout);
        Surface = eglCreateWindowSurface(Display, Config, NULL, NULL);
        if (Surface == EGL_NO_SURFACE){
             LOG( "\nSurface Creation failed: %s", EGLErrorString()); fflush(stdout);
        }
    }
    eglBindAPI(EGL_OPENGL_ES_API);

    Context = eglCreateContext(Display, Config, EGL_NO_CONTEXT, ContextAttribs);
    if (Context == EGL_NO_CONTEXT){
        LOG( "\nContext Creation failed: %s", EGLErrorString()); fflush(stdout);
    }

    if (!eglMakeCurrent(Display, Surface, Surface, Context)){
        LOG( "\nMake Current failed: %s", EGLErrorString()); fflush(stdout);
    };

    eglSwapInterval(Display, 0);

    switch(eglGetError())
    {
        case EGL_BAD_ALLOC:     LOG( "EGL BAD ALLOC");  break;
        case EGL_BAD_CONFIG:    LOG( "EGL BAD CONFIG"); break;
        case EGL_BAD_PARAMETER: LOG( "EGL BAD PARAM");  break;
        case EGL_BAD_MATCH:     LOG( "EGL BAD MATCH");  break;
    }

    EGLint attrib;
    LOG( "GL Context Done\n");
    LOG( "Width: %i Height:%i \n", OGL.width, OGL.height);
    eglGetConfigAttrib(Display, Config, EGL_DEPTH_SIZE, &attrib);
    LOG( "Depth Size: %i \n", attrib);
    eglGetConfigAttrib(Display, Config, EGL_BUFFER_SIZE, &attrib);
    LOG( "Color Buffer Size: %i \n", attrib);

    printf( "[glN64]: (II) Initialize WES...\n");
    wes_init("libGLESv2.dll");

    OGL_InitExtensions();
    OGL_InitStates();

    TextureCache_Init();
    FrameBuffer_Init();
    Combiner_Init();

    gSP.changed = gDP.changed = 0xFFFFFFFF;
    OGL_UpdateScale();

    return TRUE;
}

void OGL_Stop()
{
    LOG("OGL_Stop() \n");

    Combiner_Destroy();
    FrameBuffer_Destroy();
    TextureCache_Destroy();

    SDL_QuitSubSystem( SDL_INIT_VIDEO );
    OGL.hScreen = NULL;
/*
    wes_destroy();
    eglSwapBuffers(Display, Surface);
	eglMakeCurrent(Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
 	eglDestroyContext(Display, Context);
	eglDestroySurface(Display, Surface);
   	eglTerminate(Display);*/
}

void OGL_UpdateCullFace()
{
    LOG("OGL_UpdateCullFace() \n");

    if (gSP.geometryMode & G_CULL_BOTH)
    {
        glEnable( GL_CULL_FACE );
        if (gSP.geometryMode & G_CULL_BACK)
            glCullFace( GL_BACK );
        else
            glCullFace( GL_FRONT );
    }
    else
        glDisable( GL_CULL_FACE );
}

void OGL_UpdateViewport()
{
    LOG("OGL_UpdateViewport() \n");

    glViewport( (int)(gSP.viewport.x * OGL.scaleX), (int)((VI.height - (gSP.viewport.y + gSP.viewport.height)) * OGL.scaleY + OGL.heightOffset),
                (int)(gSP.viewport.width * OGL.scaleX), (int)(gSP.viewport.height * OGL.scaleY) );
    glDepthRange( 0.0f, 1.0f );
}

void OGL_UpdateDepthUpdate()
{
    LOG("OGL_UpdateDepthUpdate() \n");

    if (gDP.otherMode.depthUpdate)
        glDepthMask( TRUE );
    else
        glDepthMask( FALSE );
}

void OGL_UpdateStates()
{
    LOG("OGL_UpdateStates() \n");

    if (gSP.changed & CHANGED_GEOMETRYMODE)
    {
        OGL_UpdateCullFace();

        if ((gSP.geometryMode & G_FOG) && OGL.EXT_fog_coord && OGL.fog)
            glEnable( GL_FOG );
        else
            glDisable( GL_FOG );

        gSP.changed &= ~CHANGED_GEOMETRYMODE;
    }

    if (gSP.geometryMode & G_ZBUFFER)
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );

    if (gDP.changed & CHANGED_RENDERMODE)
    {
        if (gDP.otherMode.depthCompare)
            glDepthFunc( GL_LEQUAL );
        else
            glDepthFunc( GL_ALWAYS );

        OGL_UpdateDepthUpdate();

        if (gDP.otherMode.depthMode == ZMODE_DEC)
            glEnable( GL_POLYGON_OFFSET_FILL );
        else
        {
//          glPolygonOffset( -3.0f, -3.0f );
            glDisable( GL_POLYGON_OFFSET_FILL );
        }
    }

    if ((gDP.changed & CHANGED_ALPHACOMPARE) || (gDP.changed & CHANGED_RENDERMODE))
    {
        // Enable alpha test for threshold mode
        if ((gDP.otherMode.alphaCompare == G_AC_THRESHOLD) && !(gDP.otherMode.alphaCvgSel))
        {
            glEnable( GL_ALPHA_TEST );

            glAlphaFunc( (gDP.blendColor.a > 0.0f) ? GL_GEQUAL : GL_GREATER, gDP.blendColor.a );
        }
        // Used in TEX_EDGE and similar render modes
        else if (gDP.otherMode.cvgXAlpha)
        {
            glEnable( GL_ALPHA_TEST );

            // Arbitrary number -- gives nice results though
            glAlphaFunc( GL_GEQUAL, 0.5f );
        }
        else
            glDisable( GL_ALPHA_TEST );

#if 0
        if (OGL.usePolygonStipple && (gDP.otherMode.alphaCompare == G_AC_DITHER) && !(gDP.otherMode.alphaCvgSel))
            glEnable( GL_POLYGON_STIPPLE );
        else
            glDisable( GL_POLYGON_STIPPLE );
#endif
    }

    if (gDP.changed & CHANGED_SCISSOR)
    {
        glScissor( (int)(gDP.scissor.ulx * OGL.scaleX), (int)((VI.height - gDP.scissor.lry) * OGL.scaleY + OGL.heightOffset),
            (int)((gDP.scissor.lrx - gDP.scissor.ulx) * OGL.scaleX), (int)((gDP.scissor.lry - gDP.scissor.uly) * OGL.scaleY) );
    }

    if (gSP.changed & CHANGED_VIEWPORT)
    {
        OGL_UpdateViewport();
    }

    if ((gDP.changed & CHANGED_COMBINE) || (gDP.changed & CHANGED_CYCLETYPE))
    {
        if (gDP.otherMode.cycleType == G_CYC_COPY)
            Combiner_SetCombine( EncodeCombineMode( 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL0 ) );
        else if (gDP.otherMode.cycleType == G_CYC_FILL)
            Combiner_SetCombine( EncodeCombineMode( 0, 0, 0, SHADE, 0, 0, 0, 1, 0, 0, 0, SHADE, 0, 0, 0, 1 ) );
        else
            Combiner_SetCombine( gDP.combine.mux );
    }

    if (gDP.changed & CHANGED_COMBINE_COLORS)
    {
        Combiner_UpdateCombineColors();
    }

    if ((gSP.changed & CHANGED_TEXTURE) || (gDP.changed & CHANGED_TILE) || (gDP.changed & CHANGED_TMEM))
    {
        Combiner_BeginTextureUpdate();

        if (combiner.usesT0)
        {
            TextureCache_Update( 0 );

            gSP.changed &= ~CHANGED_TEXTURE;
            gDP.changed &= ~CHANGED_TILE;
            gDP.changed &= ~CHANGED_TMEM;
        }
        else
        {
            TextureCache_ActivateDummy( 0 );
        }

        if (combiner.usesT1)
        {
            TextureCache_Update( 1 );

            gSP.changed &= ~CHANGED_TEXTURE;
            gDP.changed &= ~CHANGED_TILE;
            gDP.changed &= ~CHANGED_TMEM;
        }
        else
        {
            TextureCache_ActivateDummy( 1 );
        }

        Combiner_EndTextureUpdate();
    }

    if ((gDP.changed & CHANGED_FOGCOLOR) && OGL.fog)
        glFogfv( GL_FOG_COLOR, &gDP.fogColor.r );

    if ((gDP.changed & CHANGED_RENDERMODE) || (gDP.changed & CHANGED_CYCLETYPE))
    {
        if ((gDP.otherMode.forceBlender) &&
            (gDP.otherMode.cycleType != G_CYC_COPY) &&
            (gDP.otherMode.cycleType != G_CYC_FILL) &&
            !(gDP.otherMode.alphaCvgSel))
        {
            glEnable( GL_BLEND );

            switch (gDP.otherMode.l >> 16)
            {
                case 0x0448: // Add
                case 0x055A:
                    glBlendFunc( GL_ONE, GL_ONE );
                    break;
                case 0x0C08: // 1080 Sky
                case 0x0F0A: // Used LOTS of places
                    glBlendFunc( GL_ONE, GL_ZERO );
                    break;
                case 0xC810: // Blends fog
                case 0xC811: // Blends fog
                case 0x0C18: // Standard interpolated blend
                case 0x0C19: // Used for antialiasing
                case 0x0050: // Standard interpolated blend
                case 0x0055: // Used for antialiasing
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                    break;
                case 0x0FA5: // Seems to be doing just blend color - maybe combiner can be used for this?
                case 0x5055: // Used in Paper Mario intro, I'm not sure if this is right...
                    glBlendFunc( GL_ZERO, GL_ONE );
                    break;
                default:
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                    break;
            }
        }
        else
        {
            glDisable( GL_BLEND );
        }

        if (gDP.otherMode.cycleType == G_CYC_FILL)
        {
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glEnable( GL_BLEND );
        }
    }

    gDP.changed &= CHANGED_TILE | CHANGED_TMEM;
    gSP.changed &= CHANGED_TEXTURE | CHANGED_MATRIX;
}

void OGL_AddTriangle( SPVertex *vertices, int v0, int v1, int v2 )
{
    LOG("OGL_AddTriangle(%d, %i, %i, %i) \n", vertices, v0, v1, v2);

    int v[] = { v0, v1, v2 };
    if (gSP.changed || gDP.changed)
        OGL_UpdateStates();

    for (int i = 0; i < 3; i++)
    {
        OGL.vertices[OGL.numVertices].x = vertices[v[i]].x;
        OGL.vertices[OGL.numVertices].y = vertices[v[i]].y;
        OGL.vertices[OGL.numVertices].z = gDP.otherMode.depthSource == G_ZS_PRIM ? gDP.primDepth.z * vertices[v[i]].w : vertices[v[i]].z;
        OGL.vertices[OGL.numVertices].w = vertices[v[i]].w;

        OGL.vertices[OGL.numVertices].color.r = vertices[v[i]].r;
        OGL.vertices[OGL.numVertices].color.g = vertices[v[i]].g;
        OGL.vertices[OGL.numVertices].color.b = vertices[v[i]].b;
        OGL.vertices[OGL.numVertices].color.a = vertices[v[i]].a;
        SetConstant( OGL.vertices[OGL.numVertices].color, combiner.vertex.color, combiner.vertex.alpha );
        //SetConstant( OGL.vertices[OGL.numVertices].secondaryColor, combiner.vertex.secondaryColor, ONE );

        if (OGL.EXT_secondary_color)
        {
            OGL.vertices[OGL.numVertices].secondaryColor.r = 0.0f;//lod_fraction; //vertices[v[i]].r;
            OGL.vertices[OGL.numVertices].secondaryColor.g = 0.0f;//lod_fraction; //vertices[v[i]].g;
            OGL.vertices[OGL.numVertices].secondaryColor.b = 0.0f;//lod_fraction; //vertices[v[i]].b;
            OGL.vertices[OGL.numVertices].secondaryColor.a = 1.0f;
            SetConstant( OGL.vertices[OGL.numVertices].secondaryColor, combiner.vertex.secondaryColor, ONE );
        }

        if ((gSP.geometryMode & G_FOG) && OGL.EXT_fog_coord && OGL.fog)
        {
            if (vertices[v[i]].z < -vertices[v[i]].w)
                OGL.vertices[OGL.numVertices].fog = max( 0.0f, -(float)gSP.fog.multiplier + (float)gSP.fog.offset );
            else
                OGL.vertices[OGL.numVertices].fog = max( 0.0f, vertices[v[i]].z / vertices[v[i]].w * (float)gSP.fog.multiplier + (float)gSP.fog.offset );
        }

        if (combiner.usesT0)
        {
            if (cache.current[0]->frameBufferTexture)
            {
/*              OGL.vertices[OGL.numVertices].s0 = (cache.current[0]->offsetS + (vertices[v[i]].s * cache.current[0]->shiftScaleS * gSP.texture.scales - gSP.textureTile[0]->fuls)) * cache.current[0]->scaleS;
                OGL.vertices[OGL.numVertices].t0 = (cache.current[0]->offsetT - (vertices[v[i]].t * cache.current[0]->shiftScaleT * gSP.texture.scalet - gSP.textureTile[0]->fult)) * cache.current[0]->scaleT;*/

                if (gSP.textureTile[0]->masks)
                    OGL.vertices[OGL.numVertices].s0 = (cache.current[0]->offsetS + (vertices[v[i]].s * cache.current[0]->shiftScaleS * gSP.texture.scales - fmod( gSP.textureTile[0]->fuls, 1 << gSP.textureTile[0]->masks ))) * cache.current[0]->scaleS;
                else
                    OGL.vertices[OGL.numVertices].s0 = (cache.current[0]->offsetS + (vertices[v[i]].s * cache.current[0]->shiftScaleS * gSP.texture.scales - gSP.textureTile[0]->fuls)) * cache.current[0]->scaleS;

                if (gSP.textureTile[0]->maskt)
                    OGL.vertices[OGL.numVertices].t0 = (cache.current[0]->offsetT - (vertices[v[i]].t * cache.current[0]->shiftScaleT * gSP.texture.scalet - fmod( gSP.textureTile[0]->fult, 1 << gSP.textureTile[0]->maskt ))) * cache.current[0]->scaleT;
                else
                    OGL.vertices[OGL.numVertices].t0 = (cache.current[0]->offsetT - (vertices[v[i]].t * cache.current[0]->shiftScaleT * gSP.texture.scalet - gSP.textureTile[0]->fult)) * cache.current[0]->scaleT;
            }
            else
            {
                OGL.vertices[OGL.numVertices].s0 = (vertices[v[i]].s * cache.current[0]->shiftScaleS * gSP.texture.scales - gSP.textureTile[0]->fuls + cache.current[0]->offsetS) * cache.current[0]->scaleS;
                OGL.vertices[OGL.numVertices].t0 = (vertices[v[i]].t * cache.current[0]->shiftScaleT * gSP.texture.scalet - gSP.textureTile[0]->fult + cache.current[0]->offsetT) * cache.current[0]->scaleT;
            }
        }

        if (combiner.usesT1)
        {
            if (cache.current[0]->frameBufferTexture)
            {
                OGL.vertices[OGL.numVertices].s1 = (cache.current[1]->offsetS + (vertices[v[i]].s * cache.current[1]->shiftScaleS * gSP.texture.scales - gSP.textureTile[1]->fuls)) * cache.current[1]->scaleS;
                OGL.vertices[OGL.numVertices].t1 = (cache.current[1]->offsetT - (vertices[v[i]].t * cache.current[1]->shiftScaleT * gSP.texture.scalet - gSP.textureTile[1]->fult)) * cache.current[1]->scaleT;
            }
            else
            {
                OGL.vertices[OGL.numVertices].s1 = (vertices[v[i]].s * cache.current[1]->shiftScaleS * gSP.texture.scales - gSP.textureTile[1]->fuls + cache.current[1]->offsetS) * cache.current[1]->scaleS;
                OGL.vertices[OGL.numVertices].t1 = (vertices[v[i]].t * cache.current[1]->shiftScaleT * gSP.texture.scalet - gSP.textureTile[1]->fult + cache.current[1]->offsetT) * cache.current[1]->scaleT;
            }
        }
        OGL.numVertices++;
    }

#if 0
    OGL.numTriangles++;

    if (OGL.numVertices >= 255)
        OGL_DrawTriangles();
#else
    OGL.numVertices -= 3;
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < 3; i++)
    {
        if (combiner.usesT0){
            glTexCoord2f(OGL.vertices[i].s0, OGL.vertices[i].t0);
        }
        if (combiner.usesT1){
            glMultiTexCoord2f(GL_TEXTURE1, OGL.vertices[i].s1, OGL.vertices[i].t1);
        }
        glColor4f(OGL.vertices[i].color.r, OGL.vertices[i].color.g, OGL.vertices[i].color.b, OGL.vertices[i].color.a);
        glVertex4f(OGL.vertices[i].x, OGL.vertices[i].y, OGL.vertices[i].z, OGL.vertices[i].w);
    }
    glEnd();
#endif
}

void OGL_DrawTriangles()
{
    LOG("OGL_DrawTriangles() \n");

#if 0
    if (OGL.usePolygonStipple && (gDP.otherMode.alphaCompare == G_AC_DITHER) && !(gDP.otherMode.alphaCvgSel))
    {
        OGL.lastStipple = (OGL.lastStipple + 1) & 0x7;
        glPolygonStipple( OGL.stipplePattern[(BYTE)(gDP.envColor.a * 255.0f) >> 3][OGL.lastStipple] );
    }
    glDrawArrays( GL_TRIANGLES, 0, OGL.numVertices );
    OGL.numTriangles = OGL.numVertices = 0;
#endif
}

void OGL_DrawLine( SPVertex *vertices, int v0, int v1, float width )
{
    LOG("OGL_DrawLine(%d, %i, %i, %f) \n", vertices, v0, v1, width);

    int v[] = { v0, v1 };

    GLcolor color;

    if (gSP.changed || gDP.changed)
        OGL_UpdateStates();

    glLineWidth( width * OGL.scaleX );

    glBegin( GL_LINES );
        for (int i = 0; i < 2; i++)
        {
            color.r = vertices[v[i]].r;
            color.g = vertices[v[i]].g;
            color.b = vertices[v[i]].b;
            color.a = vertices[v[i]].a;
            SetConstant( color, combiner.vertex.color, combiner.vertex.alpha );
            glColor4fv( &color.r );

            if (OGL.EXT_secondary_color)
            {
                color.r = vertices[v[i]].r;
                color.g = vertices[v[i]].g;
                color.b = vertices[v[i]].b;
                color.a = vertices[v[i]].a;
                SetConstant( color, combiner.vertex.secondaryColor, combiner.vertex.alpha );
                glSecondaryColor3fvEXT( &color.r );
            }
            glVertex4f( vertices[v[i]].x, vertices[v[i]].y, vertices[v[i]].z, vertices[v[i]].w );
        }
    glEnd();
}

void OGL_DrawRect( int ulx, int uly, int lrx, int lry, float *color )
{
    LOG("OGL_DrawRect(%i, %i, %i, %i, %d) \n", ulx, uly, lrx, lry, color);

    OGL_UpdateStates();

    glDisable( GL_SCISSOR_TEST );
    glDisable( GL_CULL_FACE );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, VI.width, VI.height, 0, 1.0f, -1.0f );
    glViewport( 0, OGL.heightOffset, OGL.width, OGL.height );
    glDepthRange( 0.0f, 1.0f );

    glColor4f(color[0], color[1], color[2], color[3]);

    glBegin( GL_QUADS );
        glVertex3f( ulx, uly, (gDP.otherMode.depthSource == G_ZS_PRIM) ? gDP.primDepth.z : gSP.viewport.nearz);
        glVertex3f( lrx, uly, (gDP.otherMode.depthSource == G_ZS_PRIM) ? gDP.primDepth.z : gSP.viewport.nearz);
        glVertex3f( lrx, lry, (gDP.otherMode.depthSource == G_ZS_PRIM) ? gDP.primDepth.z : gSP.viewport.nearz);
        glVertex3f( ulx, lry, (gDP.otherMode.depthSource == G_ZS_PRIM) ? gDP.primDepth.z : gSP.viewport.nearz);
    glEnd();

    glLoadIdentity();
    OGL_UpdateCullFace();
    OGL_UpdateViewport();
    glEnable( GL_SCISSOR_TEST );
}

void OGL_DrawTexturedRect( float ulx, float uly, float lrx, float lry, float uls, float ult, float lrs, float lrt, bool flip )
{
    LOG("OGL_DrawTexturedRect(%f, %f, %f, %f, %f, %f, %f, %f, %d) \n", ulx, uly, lrx, lry, uls, ult, lrs, lrt, flip );

    GLVertex rect[2] =
    {
        { ulx, uly, gDP.otherMode.depthSource == G_ZS_PRIM ? gDP.primDepth.z : gSP.viewport.nearz, 1.0f, { /*gDP.blendColor.r, gDP.blendColor.g, gDP.blendColor.b, gDP.blendColor.a */1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, uls, ult, uls, ult, 0.0f },
        { lrx, lry, gDP.otherMode.depthSource == G_ZS_PRIM ? gDP.primDepth.z : gSP.viewport.nearz, 1.0f, { /*gDP.blendColor.r, gDP.blendColor.g, gDP.blendColor.b, gDP.blendColor.a*/1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, lrs, lrt, lrs, lrt, 0.0f },
    };

    OGL_UpdateStates();

    glDisable( GL_CULL_FACE );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, VI.width, VI.height, 0, 1.0f, -1.0f );
    glViewport( 0, OGL.heightOffset, OGL.width, OGL.height );

    if (combiner.usesT0)
    {
        rect[0].s0 = rect[0].s0 * cache.current[0]->shiftScaleS - gSP.textureTile[0]->fuls;
        rect[0].t0 = rect[0].t0 * cache.current[0]->shiftScaleT - gSP.textureTile[0]->fult;
        rect[1].s0 = (rect[1].s0 + 1.0f) * cache.current[0]->shiftScaleS - gSP.textureTile[0]->fuls;
        rect[1].t0 = (rect[1].t0 + 1.0f) * cache.current[0]->shiftScaleT - gSP.textureTile[0]->fult;

        if ((cache.current[0]->maskS) && (fmod( rect[0].s0, cache.current[0]->width ) == 0.0f) && !(cache.current[0]->mirrorS))
        {
            rect[1].s0 -= rect[0].s0;
            rect[0].s0 = 0.0f;
        }

        if ((cache.current[0]->maskT) && (fmod( rect[0].t0, cache.current[0]->height ) == 0.0f) && !(cache.current[0]->mirrorT))
        {
            rect[1].t0 -= rect[0].t0;
            rect[0].t0 = 0.0f;
        }

        if (cache.current[0]->frameBufferTexture)
        {
            rect[0].s0 = cache.current[0]->offsetS + rect[0].s0;
            rect[0].t0 = cache.current[0]->offsetT - rect[0].t0;
            rect[1].s0 = cache.current[0]->offsetS + rect[1].s0;
            rect[1].t0 = cache.current[0]->offsetT - rect[1].t0;
        }

        if (OGL.ARB_multitexture)
            glActiveTextureARB( GL_TEXTURE0_ARB );

        if ((rect[0].s0 >= 0.0f) && (rect[1].s0 <= cache.current[0]->width))
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        if ((rect[0].t0 >= 0.0f) && (rect[1].t0 <= cache.current[0]->height))
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

//      GLint height;

//      glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );

        rect[0].s0 *= cache.current[0]->scaleS;
        rect[0].t0 *= cache.current[0]->scaleT;
        rect[1].s0 *= cache.current[0]->scaleS;
        rect[1].t0 *= cache.current[0]->scaleT;
    }

    if (combiner.usesT1 && OGL.ARB_multitexture)
    {
        rect[0].s1 = rect[0].s1 * cache.current[1]->shiftScaleS - gSP.textureTile[1]->fuls;
        rect[0].t1 = rect[0].t1 * cache.current[1]->shiftScaleT - gSP.textureTile[1]->fult;
        rect[1].s1 = (rect[1].s1 + 1.0f) * cache.current[1]->shiftScaleS - gSP.textureTile[1]->fuls;
        rect[1].t1 = (rect[1].t1 + 1.0f) * cache.current[1]->shiftScaleT - gSP.textureTile[1]->fult;

        if ((cache.current[1]->maskS) && (fmod( rect[0].s1, cache.current[1]->width ) == 0.0f) && !(cache.current[1]->mirrorS))
        {
            rect[1].s1 -= rect[0].s1;
            rect[0].s1 = 0.0f;
        }

        if ((cache.current[1]->maskT) && (fmod( rect[0].t1, cache.current[1]->height ) == 0.0f) && !(cache.current[1]->mirrorT))
        {
            rect[1].t1 -= rect[0].t1;
            rect[0].t1 = 0.0f;
        }

        if (cache.current[1]->frameBufferTexture)
        {
            rect[0].s1 = cache.current[1]->offsetS + rect[0].s1;
            rect[0].t1 = cache.current[1]->offsetT - rect[0].t1;
            rect[1].s1 = cache.current[1]->offsetS + rect[1].s1;
            rect[1].t1 = cache.current[1]->offsetT - rect[1].t1;
        }

        glActiveTextureARB( GL_TEXTURE1_ARB );

        if ((rect[0].s1 == 0.0f) && (rect[1].s1 <= cache.current[1]->width))
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );

        if ((rect[0].t1 == 0.0f) && (rect[1].t1 <= cache.current[1]->height))
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

        rect[0].s1 *= cache.current[1]->scaleS;
        rect[0].t1 *= cache.current[1]->scaleT;
        rect[1].s1 *= cache.current[1]->scaleS;
        rect[1].t1 *= cache.current[1]->scaleT;
    }

    if ((gDP.otherMode.cycleType == G_CYC_COPY) && !OGL.forceBilinear)
    {
        if (OGL.ARB_multitexture)
            glActiveTextureARB( GL_TEXTURE0_ARB );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }

    SetConstant( rect[0].color, combiner.vertex.color, combiner.vertex.alpha );

    if (OGL.EXT_secondary_color)
        SetConstant( rect[0].secondaryColor, combiner.vertex.secondaryColor, combiner.vertex.alpha );

    glBegin( GL_QUADS );
        glColor4f( rect[0].color.r, rect[0].color.g, rect[0].color.b, rect[0].color.a );

        if (OGL.EXT_secondary_color)
            glSecondaryColor3fEXT( rect[0].secondaryColor.r, rect[0].secondaryColor.g, rect[0].secondaryColor.b );

        if (OGL.ARB_multitexture)
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, rect[0].s0, rect[0].t0 );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, rect[0].s1, rect[0].t1 );
            glVertex4f( rect[0].x, rect[0].y, rect[0].z, 1.0f );

            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, rect[1].s0, rect[0].t0 );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, rect[1].s1, rect[0].t1 );
            glVertex4f( rect[1].x, rect[0].y, rect[0].z, 1.0f );

            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, rect[1].s0, rect[1].t0 );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, rect[1].s1, rect[1].t1 );
            glVertex4f( rect[1].x, rect[1].y, rect[0].z, 1.0f );

            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, rect[0].s0, rect[1].t0 );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, rect[0].s1, rect[1].t1 );
            glVertex4f( rect[0].x, rect[1].y, rect[0].z, 1.0f );
        }
        else
        {
            glTexCoord2f( rect[0].s0, rect[0].t0 );
            glVertex4f( rect[0].x, rect[0].y, rect[0].z, 1.0f );

            if (flip)
                glTexCoord2f( rect[1].s0, rect[0].t0 );
            else
                glTexCoord2f( rect[0].s0, rect[1].t0 );

            glVertex4f( rect[1].x, rect[0].y, rect[0].z, 1.0f );

            glTexCoord2f( rect[1].s0, rect[1].t0 );
            glVertex4f( rect[1].x, rect[1].y, rect[0].z, 1.0f );

            if (flip)
                glTexCoord2f( rect[1].s0, rect[0].t0 );
            else
                glTexCoord2f( rect[1].s0, rect[0].t0 );
            glVertex4f( rect[0].x, rect[1].y, rect[0].z, 1.0f );
        }
    glEnd();

    glLoadIdentity();
    OGL_UpdateCullFace();
    OGL_UpdateViewport();
}

void OGL_ClearDepthBuffer()
{
    LOG("OGL_ClearDepthBuffer() \n");

    glDisable( GL_SCISSOR_TEST );

    OGL_UpdateStates();
    glDepthMask( TRUE );
    glClear( GL_DEPTH_BUFFER_BIT );

    OGL_UpdateDepthUpdate();

    glEnable( GL_SCISSOR_TEST );
}

void OGL_ClearColorBuffer( float *color )
{
    LOG("OGL_ClearColorBuffer(%d) \n", color);

    glDisable( GL_SCISSOR_TEST );

    glClearColor( color[0], color[1], color[2], color[3] );
    glClear( GL_COLOR_BUFFER_BIT );

    glEnable( GL_SCISSOR_TEST );
}


static void OGL_png_error(png_structp png_write, const char *message)
{
    LOG("OGL_png_error() \n", png_write,message);
    printf("PNG Error: %s\n", message);
}

static void OGL_png_warn(png_structp png_write, const char *message)
{
    LOG("OGL_png_warn() \n", png_write,message);

    printf("PNG Warning: %s\n", message);
}

void OGL_SaveScreenshot()
{
    LOG("OGL_SaveScreenshot() \n");

#if 0
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    HANDLE hBitmapFile;

    char *pixelData = (char*)malloc( OGL.width * OGL.height * 3 );
#if 0
    glReadBuffer( GL_FRONT );
#endif
    glReadPixels( 0, OGL.heightOffset, OGL.width, OGL.height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelData );
#if 0
    glReadBuffer( GL_BACK );
#endif
    infoHeader.biSize = sizeof( BITMAPINFOHEADER );
    infoHeader.biWidth = OGL.width;
    infoHeader.biHeight = OGL.height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = OGL.width * OGL.height * 3;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    fileHeader.bfType = 19778;
    fileHeader.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + infoHeader.biSizeImage;
    fileHeader.bfReserved1 = fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );

    char filename[256];

    CreateDirectory( screenDirectory, NULL );

    int i = 0;
    do
    {
        sprintf( filename, "%sscreen%02i.bmp", screenDirectory, i );
        i++;

        if (i > 99)
            return;

        hBitmapFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
    }
    while (hBitmapFile == INVALID_HANDLE_VALUE);

    DWORD written;

    WriteFile( hBitmapFile, &fileHeader, sizeof( BITMAPFILEHEADER ), &written, NULL );
    WriteFile( hBitmapFile, &infoHeader, sizeof( BITMAPINFOHEADER ), &written, NULL );
    WriteFile( hBitmapFile, pixelData, infoHeader.biSizeImage, &written, NULL );

    CloseHandle( hBitmapFile );
    free( pixelData );
#endif
    // start by getting the base file path
    char filepath[2048], filename[2048];
    filepath[0] = 0;
    filename[0] = 0;
    strcpy(filepath, screenDirectory);
    if (strlen(filepath) > 0 && filepath[strlen(filepath)-1] != '/')
        strcat(filepath, "/");
    strcat(filepath, "mupen64");
    // look for a file
    int i;
    for (i = 0; i < 100; i++)
    {
        sprintf(filename, "%s_%03i.png", filepath, i);
        FILE *pFile = fopen(filename, "r");
        if (pFile == NULL)
            break;
        fclose(pFile);
    }
    if (i == 100) return;
    // allocate PNG structures
    png_structp png_write = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, OGL_png_error, OGL_png_warn);
    if (!png_write)
    {
        printf("Error creating PNG write struct.\n");
        return;
    }
    png_infop png_info = png_create_info_struct(png_write);
    if (!png_info)
    {
        png_destroy_write_struct(&png_write, (png_infopp)NULL);
        printf("Error creating PNG info struct.\n");
        return;
    }
    // Set the jumpback
    if (setjmp(png_jmpbuf(png_write)))
    {
        png_destroy_write_struct(&png_write, &png_info);
        printf("Error calling setjmp()\n");
        return;
    }
    // open the file to write
    FILE *savefile = fopen(filename, "wb");
    if (savefile == NULL)
    {
        printf("Error opening '%s' to save screenshot.\n", filename);
        return;
    }
    // give the file handle to the PNG compressor
    png_init_io(png_write, savefile);
    // read pixel data from OpenGL
    char *pixels = (char*)malloc( OGL.width * OGL.height * 3 );
#if 0
    glReadBuffer( GL_FRONT );
#endif
    glReadPixels( 0, OGL.heightOffset, OGL.width, OGL.height, GL_RGB, GL_UNSIGNED_BYTE, pixels );
#if 0
    glReadBuffer( GL_BACK );
#endif
    // set the info
    int width = OGL.width;
    int height = OGL.height;
    png_set_IHDR(png_write, png_info, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    // lock the screen, get a pointer and row pitch
    long pitch = width * 3;
    // allocate row pointers and scale each row to 24-bit color
    png_byte **row_pointers;
    row_pointers = (png_byte **) malloc(height * sizeof(png_bytep));
    for (int i = 0; i < height; i++)
    {
        row_pointers[i] = (png_byte *) (pixels + (height - 1 - i) * pitch);
    }
    // set the row pointers
    png_set_rows(png_write, png_info, row_pointers);
    // write the picture to disk
    png_write_png(png_write, png_info, 0, NULL);
    // free memory
    free(row_pointers);
    png_destroy_write_struct(&png_write, &png_info);
    free(pixels);
    // all done
}

void
OGL_SwapBuffers()
{
    LOG("OGL_SwapBuffers() \n");

    static int frames[5] = { 0, 0, 0, 0, 0 };
    static int framesIndex = 0;
    static Uint32 lastTicks = 0;
    Uint32 ticks = SDL_GetTicks();

    frames[framesIndex]++;
    if (ticks >= (lastTicks + 1000))
    {
        char caption[500];
        float fps = 0.0;
        for (int i = 0; i < 5; i++)
            fps += frames[i];
        fps /= 5.0;
        snprintf( caption, 500, "%s - %.2f fps", pluginName, fps );
        SDL_WM_SetCaption( caption, pluginName );
        framesIndex = (framesIndex + 1) % 5;
        frames[framesIndex] = 0;
        lastTicks = ticks;
    }

    // if emulator defined a render callback function, call it before buffer swap

    if(renderCallback)
        (*renderCallback)();


    eglSwapBuffers(Display, Surface);
}

void OGL_ReadScreen( void **dest, int *width, int *height )
{
    LOG("OGL_ReadScreen(%d, %d, %d) \n", dest, width, height);

    *width = OGL.width;
    *height = OGL.height;

    *dest = malloc( OGL.height * OGL.width * 3 );
    if (*dest == 0)
        return;

    GLint oldMode;
#if 0
    glGetIntegerv( GL_READ_BUFFER, &oldMode );
    glReadBuffer( GL_FRONT );
//  glReadBuffer( GL_BACK );
#endif
    glReadPixels( 0, 0, OGL.width, OGL.height,
                  GL_RGB, GL_UNSIGNED_BYTE, *dest );
#if 0
    glReadBuffer( oldMode );
#endif
}



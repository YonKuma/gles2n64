/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - Config_nogui.cpp                                        *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2008 Tillin9                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "Config.h"
#include "gles2N64.h"
#include "RSP.h"
#include "Textures.h"
#include "OpenGL.h"

#include "winlnxdefs.h"
#include "Config.h"

static const char *pluginDir = 0;

static inline const char *GetPluginDir()
{
    static char path[PATH_MAX];

    if(strlen(configdir) > 0)
    {
        strncpy(path, configdir, PATH_MAX);
        // remove trailing '/'
        if(path[strlen(path)-1] == '/')
            path[strlen(path)-1] = '\0';
    }
    else
    {
        strcat(path, ".");
    }

    return path;
}

void Config_WriteConfig(const char *filename)
{
    FILE* f = fopen(filename, "w");
    if (!f)
    {
        printf("Could Not Open %s for writing\n", filename);
    }
    fprintf(f, "#gles2n64 Graphics Plugin for N64\n");
    fprintf(f, "#by Orkin / glN64 developers and Adventus               \n\n");

    fprintf(f, "#These values are the physical pixel dimensions of      \n");
    fprintf(f, "#your screen. They are only used for centering the      \n");
    fprintf(f, "#window.                                                \n");

    fprintf(f, "screen width=%i\n", OGL.scrWidth);
    fprintf(f, "screen height=%i\n\n", OGL.scrHeight);

    fprintf(f, "#The Window position and dimensions specify how and     \n");
    fprintf(f, "#where the games will appear on the screen. Enabling    \n");
    fprintf(f, "#Centre will ensure that the window is centered         \n");
    fprintf(f, "#within the screen (overriding xpos/ypos).              \n\n");

    fprintf(f, "window centre=%i\n", OGL.winCentre);
    fprintf(f, "window xpos=%i\n", OGL.winXpos);
    fprintf(f, "window ypos=%i\n", OGL.winYpos);
    fprintf(f, "window width=%i\n", OGL.winWidth);
    fprintf(f, "window height=%i\n\n", OGL.winHeight);

    fprintf(f, "#Enabling offscreen frambuffering allows the resulting  \n");
    fprintf(f, "#image to be upscaled to the window dimensions. The     \n");
    fprintf(f, "#framebuffer dimensions specify the resolution which    \n");
    fprintf(f, "#gles2n64 will render to. Some games are GPU            \n");
    fprintf(f, "#bottlenecked (ie Banjo Kazooie) and will run           \n");
    fprintf(f, "#signficantly faster at 400x240 or 320x240. When        \n");
    fprintf(f, "#offscreen framebuffering is disabled the windows       \n");
    fprintf(f, "#dimensions are used. Note that if the window and       \n");
    fprintf(f, "#framebuffer dimensions are the same it is faster to    \n");
    fprintf(f, "#disable offscreen framebuffering due to the additional \n");
    fprintf(f, "#copy.                                                  \n\n");

    fprintf(f, "framebuffer enable=%i\n", OGL.fbEnable);
    fprintf(f, "framebuffer bilinear=%i\n", OGL.fbBilinearScale);
    fprintf(f, "framebuffer width=%i\n", OGL.fbWidth);
    fprintf(f, "framebuffer height=%i\n\n", OGL.fbHeight);

    fprintf(f, "#Frameskipping allows more CPU time be spent on other   \n");
    fprintf(f, "#tasks than GPU emulation, but at the cost of a lower   \n");
    fprintf(f, "#framerate. 1=No Frameskip, 2=One Frameskipped, etc     \n\n");

    fprintf(f, "frame skip=%i\n\n", OGL.frameSkip);

    fprintf(f, "#Vertical Sync Divider (0=No VSYNC, 1=60Hz, 2=30Hz, etc)\n");
    fprintf(f, "vertical sync=%i\n\n", OGL.vSync);

    fprintf(f, "#These options enable different rendering paths, they   \n");
    fprintf(f, "#can relieve pressure on the GPU / CPU.                 \n\n");

    fprintf(f, "enable fog=%i\n", OGL.enableFog);
    fprintf(f, "enable primitive z=%i\n", OGL.enablePrimZ);
    fprintf(f, "enable lighting=%i\n", OGL.enableLighting);
    fprintf(f, "enable alpha test=%i\n", OGL.enableAlphaTest);
    fprintf(f, "enable clipping=%i\n", OGL.enableClipping);
    fprintf(f, "enable face culling=%i\n", OGL.enableFaceCulling);

    fprintf(f, "#Texture Bit Depth (0=force 16bit, 1=either 16/32bit, 2=force 32bit) \n");
    fprintf(f, "texture depth=%i\n", OGL.textureBitDepth);
    fprintf(f, "texture mipmap=%i\n", OGL.textureMipmap);
    fprintf(f, "texture 2xSAI=%i\n", OGL.texture2xSaI);
    fprintf(f, "texture force bilinear=%i\n", OGL.textureForceBilinear);
    fprintf(f, "texture max anisotropy=%i\n\n", OGL.textureMaxAnisotropy);

    fprintf(f, "#RDP Clamping Mode (2=Fully Accurate, 1=Hack, 0=Default)\n");
    fprintf(f, "rdp clamp mode=%i\n\n", OGL.rdpClampMode);
    fprintf(f, "#Force Depthbuffer clear after Buffer swap \n");
    fprintf(f, "force depth clear=%i\n\n", OGL.forceDepthClear);

    fclose(f);
}

void Config_LoadConfig()
{
    static int loaded = 0;
    FILE *f;
    char line[4096];

    if (loaded)
        return;

    loaded = 1;

    if (pluginDir == 0)
        pluginDir = GetPluginDir();

    // default configuration
    OGL.winCentre = 1;
    OGL.fbYpos = OGL.fbXpos = OGL.winXpos = OGL.winYpos = 0;
    OGL.scrWidth = OGL.winWidth = 800;
    OGL.scrHeight = OGL.winHeight = 480;
    OGL.fbWidth = 400;
    OGL.fbHeight = 240;
    OGL.fbEnable = 0;
    OGL.fbBilinearScale = 0;
    OGL.frameSkip = 1;
    OGL.vSync = 0;
    OGL.enableLighting = 1;
    OGL.enableAlphaTest = 1;
    OGL.enableClipping = 1;
    OGL.enableFog = 0;
    OGL.enablePrimZ = 0;
    OGL.enableFaceCulling = 1;
    OGL.forceDepthClear = 0;
    OGL.rdpClampMode = 0;
    OGL.textureForceBilinear = 0;
    OGL.textureMaxAnisotropy = 0;
    OGL.textureBitDepth = 1;
    OGL.textureMipmap = 0;
    OGL.texture2xSaI = 0;
    OGL.logFrameRate = 1;
    cache.maxBytes = 32 * 1048576;

    // read configuration
    char filename[PATH_MAX];
    snprintf( filename, PATH_MAX, "%s/gles2n64.conf", pluginDir );
    f = fopen( filename, "r" );
    if (!f)
    {
        fprintf( stdout, "[gles2N64]: Couldn't open config file '%s' for reading: %s\n", filename, strerror( errno ) );
        fprintf( stdout, "[gles2N64]: Attempting to write new Config \n");
        Config_WriteConfig(filename);
    }
    else
    {
        printf("[gles2n64]: Loading Config from %s \n", filename);

        while (!feof( f ))
        {
            char *val;
            fgets( line, 4096, f );

            if (line[0] == '#' || line[0] == '\n')
                continue;

            val = strchr( line, '=' );
            if (!val) continue;

            *val++ = '\0';

            if (!strcasecmp( line, "window width" ))
            {
                OGL.winWidth = atoi( val );
            }
            else if (!strcasecmp( line, "window height" ))
            {
                OGL.winHeight = atoi( val );
            }
            else if (!strcasecmp( line, "screen width" ))
            {
                OGL.scrWidth = atoi( val );
            }
            else if (!strcasecmp( line, "screen height" ))
            {
                OGL.scrHeight = atoi( val );
            }
            else if (!strcasecmp( line, "framebuffer width" ))
            {
                OGL.fbWidth = atoi( val );
            }
            else if (!strcasecmp( line, "framebuffer height" ))
            {
                OGL.fbHeight = atoi( val );
            }
            else if (!strcasecmp( line, "window centre" ))
            {
                OGL.winCentre = atoi( val );
            }
            else if (!strcasecmp( line, "window xpos" ))
            {
                OGL.winXpos = atoi(val);
            }
            else if (!strcasecmp( line, "window ypos" ))
            {
                OGL.winYpos = atoi(val);
            }
            else if (!strcasecmp( line, "texture force bilinear" ))
            {
                OGL.textureForceBilinear = atoi( val );
            }
            else if (!strcasecmp( line, "texture 2xSAI" ))
            {
                OGL.texture2xSaI = atoi( val );
            }
            else if (!strcasecmp( line, "texture max anisotropy"))
            {
                OGL.textureMaxAnisotropy = atoi( val );
            }
            else if (!strcasecmp( line, "texture depth" ))
            {
                OGL.textureBitDepth = atoi( val );
            }
            else if (!strcasecmp( line, "texture mipmap" ))
            {
                OGL.textureMipmap = atoi( val );
            }
            else if (!strcasecmp( line, "enable fog" ))
            {
                OGL.enableFog = atoi( val );
            }
            else if (!strcasecmp( line, "enable primitive z" ))
            {
                OGL.enablePrimZ = atoi( val );
            }
            else if (!strcasecmp( line, "enable clipping" ))
            {
                OGL.enableClipping = atoi( val );
            }
            else if (!strcasecmp( line, "enable lighting" ))
            {
                OGL.enableLighting = atoi( val );
            }
            else if (!strcasecmp( line, "enable alpha test" ))
            {
                OGL.enableAlphaTest = atoi( val );
            }
            else if (!strcasecmp( line, "enable face culling" ))
            {
                OGL.enableFaceCulling = atoi( val );
            }
            else if (!strcasecmp( line, "cache size" ))
            {
                cache.maxBytes = atoi( val ) * 1048576;
            }
            else if (!strcasecmp( line, "frame skip" ))
            {
                OGL.frameSkip = atoi( val );
            }
            else if (!strcasecmp( line, "vertical sync" ))
            {
                OGL.vSync = atoi( val );
            }
            else if (!strcasecmp( line, "log fps" ))
            {
                OGL.logFrameRate = atoi( val );
            }
            else if (!strcasecmp( line, "rdp clamp mode" ))
            {
                OGL.rdpClampMode = atoi(val);
            }
            else if (!strcasecmp( line, "force depth clear" ))
            {
                OGL.forceDepthClear = atoi(val);
            }
            else if (!strcasecmp( line, "framebuffer enable" ))
            {
                OGL.fbEnable = atoi(val);
            }
            else if (!strcasecmp( line, "framebuffer bilinear" ))
            {
                OGL.fbBilinearScale = atoi(val);
            }
            else
            {
                printf( "[gles2n64]: Unsupported config option: %s\n", line );
            }
        }
    }

    if (OGL.winCentre)
    {
        OGL.winXpos = (OGL.scrWidth - OGL.winWidth) / 2;
        OGL.winYpos = (OGL.scrHeight - OGL.winHeight) / 2;
    }

    if (!OGL.fbEnable)
    {
        OGL.fbXpos = OGL.winXpos;
        OGL.fbYpos = OGL.winYpos;
        OGL.fbWidth = OGL.winWidth;
        OGL.fbHeight= OGL.winHeight;
    }

    if (f) fclose( f );
}

void Config_DoConfig(HWND)
{
    printf("Please edit the config file gles2n64.conf manually.\n");
}


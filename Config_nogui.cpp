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
//#include <dlfcn.h>
#include <unistd.h>

#include "Config.h"
#include "glN64.h"
#include "RSP.h"
#include "Textures.h"
#include "OpenGL.h"

#include "winlnxdefs.h"

#include "OpenGL.h"
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
#if 0
    else
    {
#ifdef __USE_GNU
                Dl_info info;
                void *addr = (void*)GetPluginDir;
                if(dladdr(addr, &info) != 0)
                {
                   strncpy(path, info.dli_fname, PATH_MAX);
                   *(strrchr(path, '/')) = '\0';
                }
                else
                {
                   LOG( "(WW) Couldn't get path of .so, trying to get emulator's path\n");
#endif // __USE_GNU
                   if(readlink("/proc/self/exe", path, PATH_MAX) == -1)
                   {
                           LOG( "(WW) readlink() /proc/self/exe failed: %s\n", strerror(errno));
                      path[0] = '.';
                      path[1] = '\0';
                   }
                   *(strrchr(path, '/')) = '\0';
                   strncat(path, "/plugins", PATH_MAX);
#ifdef __USE_GNU
        }
#endif
    }
#endif
        return path;
}

void Config_LoadConfig()
{
    static int loaded = 0;
    FILE *f;
    char line[2000];

    if (loaded)
        return;

    loaded = 1;

    if (pluginDir == 0)
        pluginDir = GetPluginDir();

    // default configuration
    OGL.fullscreenWidth = 800;
    OGL.fullscreenHeight = 480;
    OGL.windowedWidth = 800;
    OGL.windowedHeight = 480;

    OGL.frameSkip = 1;
    OGL.vSync = 0;
    OGL.logFrameRate = 1;
    OGL.textureHack = 0;
    OGL.alphaHack = 1;
    OGL.enableLighting = 1;
    OGL.enableAlphaTest = 1;

    OGL.combinerCompiler = 1;
    OGL.forceBilinear = 0;
    OGL.enable2xSaI = 0;
    OGL.enableAnisotropicFiltering = 0;
    OGL.fog = 1;
    OGL.textureBitDepth = 1; // normal (16 & 32 bits)
    cache.maxBytes = 32 * 1048576;

    // read configuration
    char filename[PATH_MAX];
    snprintf( filename, PATH_MAX, "%s/gles2n64.conf", pluginDir );
    f = fopen( filename, "r" );
    if (!f)
    {
        fprintf( stdout, "[glN64]: (WW) Couldn't open config file '%s' for reading: %s\n", filename, strerror( errno ) );
        return;
    }

    while (!feof( f ))
    {
        char *val;
        fgets( line, 2000, f );

        val = strchr( line, '=' );
        if (!val)
            continue;
        *val++ = '\0';

        if (!strcasecmp( line, "width" ))
        {
            int w = atoi( val );
            OGL.fullscreenWidth = OGL.windowedWidth = (w == 0) ? (800) : (w);
        }
        else if (!strcasecmp( line, "height" ))
        {
            int h = atoi( val );
            OGL.fullscreenHeight = OGL.windowedHeight = (h == 0) ? (480) : (h);
        }
        else if (!strcasecmp( line, "force bilinear" ))
        {
            OGL.forceBilinear = atoi( val );
        }
        else if (!strcasecmp( line, "enable 2xSAI" ))
        {
            OGL.enable2xSaI = atoi( val );
        }
        else if (!strcasecmp( line, "enable anisotropic"))
        {
            OGL.enableAnisotropicFiltering = atoi( val );
        }
        else if (!strcasecmp( line, "enable fog" ))
        {
            OGL.fog = atoi( val );
        }
        else if (!strcasecmp( line, "cache size" ))
        {
            cache.maxBytes = atoi( val ) * 1048576;
        }
        else if (!strcasecmp( line, "texture depth" ))
        {
            OGL.textureBitDepth = atoi( val );
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
        else if (!strcasecmp( line, "texture hack" ))
        {
            OGL.textureHack = atoi( val );
        }
        else if (!strcasecmp( line, "alpha hack" ))
        {
            OGL.alphaHack = atoi( val );
        }
        else if (!strcasecmp( line, "highp intermediates" ))
        {
            OGL.highpIntermediates = atoi( val );
        }
        else if (!strcasecmp( line, "enable lighting" ))
        {
            OGL.enableLighting = atoi( val );
        }
        else if (!strcasecmp( line, "enable alpha test" ))
        {
            OGL.enableAlphaTest = atoi( val );
        }
        else if (!strcasecmp( line, "combiner compiler" ))
        {
            OGL.combinerCompiler = atoi( val );
        }
        else
        {
            printf( "Unsupported config option: %s\n", line );
        }
    }
    OGL.xpos = (800 - OGL.fullscreenWidth) / 2;
    OGL.ypos = (480 - OGL.fullscreenHeight) / 2;

    fclose( f );
}

void Config_DoConfig(HWND /*hParent*/)
{
printf("GlN64 compiled with GUI=NONE. Please edit the config file\nin SHAREDIR/glN64.conf manually or recompile plugin with a GUI.\n");
}


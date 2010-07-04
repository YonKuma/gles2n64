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


struct Option
{
    const char* name;
    int*  data;
    const int   initial;
};

Option config_options[] =
{
    {"#gles2n64 Graphics Plugin for N64", NULL, 0},
    {"#by Orkin / glN64 developers and Adventus.", NULL, 0},

    {"#These values are the physical pixel dimensions of", NULL, 0},
    {"#your screen. They are only used for centering the", NULL, 0},
    {"#window.", NULL, 0},

    {"screen width", &OGL.screen.width, 800},
    {"screen height", &OGL.screen.height, 480},

    {"#The Window position and dimensions specify how and", NULL, 0},
    {"#where the games will appear on the screen. Enabling", NULL, 0},
    {"#Centre will ensure that the window is centered", NULL, 0},
    {"#within the screen (overriding xpos/ypos).", NULL, 0},

    {"window enable x11", &OGL.window.enablex11, 1},
    {"window fullscreen", &OGL.window.fullscreen, 1},
    {"window centre", &OGL.window.centre, 1},
    {"window xpos", &OGL.window.xpos, 0},
    {"window ypos", &OGL.window.ypos, 0},
    {"window width", &OGL.window.width, 800},
    {"window height", &OGL.window.height, 480},

    {"#Enabling offscreen frambuffering allows the resulting",NULL,0},
    {"#image to be upscaled to the window dimensions. The",NULL,0},
    {"#framebuffer dimensions specify the resolution which",NULL,0},
    {"#gles2n64 will render to.",NULL,0},

    {"framebuffer enable", &OGL.framebuffer.enable, 1},
    {"framebuffer bilinear", &OGL.framebuffer.bilinear, 0},
    {"framebuffer width", &OGL.framebuffer.width, 400},
    {"framebuffer height", &OGL.framebuffer.height, 240},

    {"#Frameskipping allows more CPU time be spent on other", NULL, 0},
    {"#tasks than GPU emulation, but at the cost of a lower", NULL, 0},
    {"#framerate.", NULL, 0},

    {"frame skip", &OGL.frameskip, 1},

    {"#Vertical Sync Divider (0=No VSYNC, 1=60Hz, 2=30Hz, etc)", NULL, 0},

    {"vertical sync", &OGL.vsync, 0},

    {"#These options enable different rendering paths, they", NULL, 0},
    {"#can relieve pressure on the GPU / CPU.", NULL, 0},

    {"enable fog", &OGL.enableFog, 0},
    {"enable primitive z", &OGL.enablePrimZ, 1},
    {"enable lighting", &OGL.enableLighting, 1},
    {"enable alpha test", &OGL.enableAlphaTest, 1},
    {"enable clipping", &OGL.enableClipping, 1},
    {"enable face culling", &OGL.enableFaceCulling, 1},

    {"#Texture Bit Depth (0=force 16bit, 1=either 16/32bit, 2=force 32bit)", NULL, 0},
    {"texture depth", &OGL.texture.bit_depth, 1},
    {"texture mipmap", &OGL.texture.mipmap, 0},
    {"texture 2xSAI", &OGL.texture.SaI2x, 0},
    {"texture force bilinear", &OGL.texture.force_bilinear, 0},
    {"texture max anisotropy", &OGL.texture.max_anisotropy, 0},

    {"#", NULL, 0},
    {"update mode", &OGL.updateMode, 1},
    {"ignore offscreen rendering", &OGL.ignoreOffscreenRendering, 0},
    {"force screen clear", &OGL.forceClear, 0},

};

const int config_options_size = sizeof(config_options) / sizeof(Option);


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

    for(int i=0; i<config_options_size; i++)
    {
        Option *o = &config_options[i];
        fprintf(f, o->name);
        if (o->data) fprintf(f,"=%i", *(o->data));
        fprintf(f, "\n");
    }

    fclose(f);
}

void Config_LoadConfig()
{
    FILE *f;
    char line[4096];

    const char *pluginDir = GetPluginDir();

    // default configuration
    for(int i=0; i < config_options_size; i++)
    {
        Option *o = &config_options[i];
        if (o->data) *(o->data) = o->initial;
    }

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

            for(int i=0; i< config_options_size; i++)
            {
                Option *o = &config_options[i];
                if (strcasecmp(line, o->name) == 0)
                {
                    if (o->data) *(o->data) = atoi(val);
                    break;
                }
            }

        }
    }
    if (f) fclose(f);
}

void Config_DoConfig(HWND)
{
    FILE *f = fopen("./config/gles2n64.conf", "r" );
    if (!f)
    {
        // default configuration
        for(int i=0; i < config_options_size; i++)
        {
            Option *o = &config_options[i];
            if (o->data) *(o->data) = o->initial;
        }
        Config_WriteConfig("./config/gles2n64.conf");
    }
    else
        fclose(f);

    system("mousepad ./config/gles2n64.conf");
}


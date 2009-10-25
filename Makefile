#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *   Mupen64plus - Makefile                                                *
# *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
# *   Copyright (C) 2007-2008 DarkJeztr Tillin9 Richard42                   *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# Makefile for glN64 in Mupen64plus.

ARCH = ARM
OS = LINUX

SO_EXTENSION = so
CXX = 	C:/CS2007q3/bin/arm-none-linux-gnueabi-g++
LD = 	C:/CS2007q3/bin/arm-none-linux-gnueabi-g++

CFLAGS = -I./
CFLAGS += -I./wes

ifeq ($(ARCH), X86)
CFLAGS += -IC:/MinGW/include
CFLAGS += -IC:/MinGW/include/SDL
CFLAGS += -IC:/MinGW/include/PVR
CFLAGS += -IC:/MinGW/include/libpng12

else

DRIVE 	= G
INCLUDE	= $(DRIVE):/Pandora/pnd_libs_081117/include
CFLAGS	+= -IC:/CS2007q3/include/PVR
CFLAGS 	+= -I$(INCLUDE)/libpng12
CFLAGS 	+= -I$(INCLUDE)/SDL
CFLAGS	+= -I$(INCLUDE)
CFLAGS 	+= -IC:/CS2007q3/arm-none-linux-gnueabi/libc/lib
#LDFLAGS += -LG:/Pandora/lib_rev2
#LDFLAGS += -LG:/Pandora/lib
endif

ifeq ($(OS), LINUX)
CFLAGS += -Wall -D__LINUX__ -fPIC -D__NEON_OPT 
LDFLAGS += G:/Pandora/lib_rev2/libEGL.so 
else
CFLAGS += -Wall
LDFLAGS +=  -LC:/MinGW/lib/PVR -lSDLmain -lSDL -lpng -lGLESv2
endif

CFLAGS  += -O3 -march=armv7-a -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ffast-math \
		  -fsingle-precision-constant -save-temps -fomit-frame-pointer -fno-tree-vectorize


OBJECTS = Config_nogui.o
OBJECTS += ./wes/wes_matrix.o ./wes/wes_begin.o ./wes/wes_fragment.o ./wes/wes_shader.o ./wes/wes_state.o ./wes/wes_texture.o ./wes/wes.o

# list of object files to generate
OBJECTS += glN64.o \
	OpenGL.o \
	N64.o \
	RSP.o \
	VI.o \
	Textures.o \
	FrameBuffer.o \
	Combiner.o \
	gDP.o \
	gSP.o \
	GBI.o \
	DepthBuffer.o \
	CRC.o \
	2xSAI.o \
	texture_env.o \
	texture_env_combine.o \
	RDP.o \
	F3D.o \
	F3DEX.o \
	F3DEX2.o \
	L3D.o \
	L3DEX.o \
	L3DEX2.o \
	S2DEX.o \
	S2DEX2.o \
	F3DPD.o \
	F3DDKR.o \
	F3DWRUS.o

# build targets
all: gles2n64.$(SO_EXTENSION)

clean:
	del -f *.o ".\wes\*.o" *.$(SO_EXTENSION) ui_gln64config.*

# build rules
.cpp.o:
	$(CXX) -o $@ $(CFLAGS) -c $<

.c.o:
	$(CXX) -o $@ $(CFLAGS) -c $<

ui_gln64config.h: gln64config.ui
	$(UIC) $< -o $@

gles2n64.$(SO_EXTENSION): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) $(ASM_LDFLAGS) $(PLUGIN_LDFLAGS) $(SDL_LIBS) $(LIBGL_LIBS) -shared -o $@

glN64.o: glN64.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -DMAINDEF -c -o $@ $<

Config_qt4.o: Config_qt4.cpp ui_gln64config.h
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<

Config_gtk2.o: Config_gtk2.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<

GBI.o: GBI.cpp
	$(CXX) $(CFLAGS) $(SDL_FLAGS) -c -o $@ $<


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

#SO_EXTENSION = dll
#CXX = 	C:/MinGW/bin/g++
#LD = 	C:/MinGW/bin/g++
#STRIP = C:/MinGW/bin/strip
SO_EXTENSION = so
CXX = 	C:/CS2009q1/bin/arm-none-linux-gnueabi-g++
LD = 	C:/CS2009q1/bin/arm-none-linux-gnueabi-g++
STRIP = C:/CS2009q1/bin/arm-none-linux-gnueabi-strip

CFLAGS = -I./
CFLAGS += -I./wes/
CFLAGS += -IC:/CS2009q1/arm-none-linux-gnueabi/include/c++/4.3.3
CFLAGS += -IC:/CS2009q1/arm-none-linux-gnueabi/include/c++/4.3.3/arm-none-linux-gnueabi/ 
CFLAGS += -IC:/CS2009q1/lib/gcc/arm-none-linux-gnueabi/4.3.3/include
CFLAGS += -IC:/CS2009q1/arm-none-linux-gnueabi/libc/lib
CFLAGS += -IC:/CS2009q1/include
CFLAGS += -IC:/CS2009q1/include/SDL
CFLAGS += -IC:/CS2009q1/include/libpng12


CFLAGS += -Wall -D__LINUX__
LDFLAGS += -LC:/CS2009q1/lib -lsrv_um -lGLESv2 -lIMGegl -lEGL -lSDL -lts -lpng12


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


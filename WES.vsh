/*
gl-wes-v2:  OpenGL 2.0 to OGLESv2.0 wrapper
Contact:    lachlan.ts@gmail.com
Copyright (C) 2009  Lachlan Tychsen - Smith aka Adventus

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define LIGHT_NUM						8
#define CLIPPLANE_NUM					6
#define MULTITEX_NUM					4
#define FACE_NUM						2

#define FACE_FRONT						0
#define FACE_BACK						1

#define COLORMAT_AMBIENT				0
#define COLORMAT_DIFFUSE				1
#define COLORMAT_AMBIENT_AND_DIFFUSE	2
#define COLORMAT_SPECULAR				3
#define COLORMAT_EMISSIVE				4

#define FUNC_NEVER                     	0
#define FUNC_LESS                     	1
#define FUNC_EQUAL                      2
#define FUNC_LEQUAL                     3
#define FUNC_GREATER                    4
#define FUNC_NOTEQUAL                   5
#define FUNC_GEQUAL                     6
#define FUNC_ALWAYS                     7

#define FOG_LINEAR						0
#define FOG_EXP							1
#define FOG_EXP2						2

#define GEN_OBJLINEAR					1
#define GEN_EYELINEAR					2
#define GEN_SPHEREMAP					3
#define GEN_REFLECTMAP					4

//Attributes:
attribute highp vec4 	aPosition;
attribute mediump vec4 	aTexCoord0;
attribute mediump vec4 	aTexCoord1;
attribute mediump vec4 	aTexCoord2;
attribute mediump vec4 	aTexCoord3;
attribute highp vec3 	aNormal;
attribute highp float 	aFogCoord;
attribute lowp vec4 	aColor;
attribute lowp vec4 	aColor2nd;

//Uniforms:
uniform highp mat4		uMVP;		//model-view-projection matrix
uniform highp mat4		uMV;		//model-view matrix
uniform highp mat3		uMVIT;		//model-view inverted & transformed matrix 

uniform	bool			uEnableFog;
uniform	bool			uEnableFogCoord;
uniform float			uFogStart, uFogEnd;

//Varyings:
varying lowp vec4 		vColor;
varying lowp vec2		vFactor;
varying mediump vec4 	vTexCoord[MULTITEX_NUM];


//local variables:
highp float				lEye;

void main(){
	gl_Position = uMVP * aPosition;
	
	/* Lighting 	*/
	vColor = aColor;
	
	/* Fog			*/
	if (uEnableFog){
		lEye = dot(vec4(uMV[0][2],uMV[1][2], uMV[2][2], uMV[3][2]), aPosition);
		if (uEnableFogCoord){
			vFactor.x = (uFogEnd - aFogCoord) * (1.0 / (uFogEnd - uFogStart));
		}else{
			vFactor.x = (uFogEnd + lEye) * (1.0 / (uFogEnd - uFogStart));
		}	
		vFactor.x = clamp(vFactor.x, 0.0, 1.0);
	} else {
		vFactor.x = 1.0;
	}

	/* Tex Coord Generators 	*/
	vTexCoord[0] = aTexCoord0;
	vTexCoord[1] = aTexCoord1;
	vTexCoord[2] = aTexCoord2;
	vTexCoord[3] = aTexCoord3;
}
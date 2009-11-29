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

//Attributes:
attribute highp vec4 	aPosition;
attribute lowp vec3 	aColor;
attribute lowp float 	aColorAlpha;
attribute mediump vec2 	aTexCoord0;
attribute mediump vec2 	aTexCoord1;

//
uniform	bool			uEnableFog;
uniform float			uFogMultiplier, uFogOffset;


uniform bool 			uEnablePrimitiveZ;
uniform bool 			uEnableTexGen;


uniform float 			uPrimitiveZ;

//texture transforms:
uniform mediump vec2 	uTexScale[2];
uniform mediump vec2 	uTexOffset[2];
uniform mediump vec2 	uCacheShiftScale[2];
uniform mediump vec2 	uCacheScale[2];
uniform mediump vec2 	uCacheOffset[2];

//Varyings:
varying lowp vec4 		vColor;
varying lowp float		vFactor;
varying mediump vec2 	vTexCoord[2];

//locals
highp vec4 	lPosition;
float 		lFogCoord;

void main()
{
	//Primitive Z:
	lPosition = aPosition;
	if (uEnablePrimitiveZ)
	{	 
		lPosition.z = uPrimitiveZ * aPosition.w;
	}
	
	//Fog:
	if (uEnableFog)
	{	
		lFogCoord = max(-1.0, aPosition.z / aPosition.w) * uFogMultiplier + uFogOffset;
		vFactor = 1.0 - lFogCoord * (1.0 / 255.0);
		vFactor = clamp(vFactor, 0.0, 1.0);
	} 
	else 
	{
		vFactor = 1.0;
	}

	//Texture Coord Transformation:
	if (uEnableTexGen)
	{
		vTexCoord[0] = (aTexCoord0 * (uTexScale[0] * uCacheShiftScale[0]) + (uCacheOffset[0] - uTexOffset[0])) * uCacheScale[0];
		vTexCoord[1] = (aTexCoord1 * (uTexScale[1] * uCacheShiftScale[1]) + (uCacheOffset[1] - uTexOffset[1])) * uCacheScale[1];
	}
	else
	{
		vTexCoord[0] = aTexCoord0;
		vTexCoord[1] = aTexCoord1;
	}
	
	gl_Position = lPosition;
	vColor.rgb = aColor;
	vColor.a = aColorAlpha;
}
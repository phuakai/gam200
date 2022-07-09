#version 450 core

/*
A per-fragment color attribute is no longer received from rasterizer.
Instead per-fragment color attribute is supplied by the application to
a uniform variable:
uniform vec3 uColor;
The uniform variable will retain the value for every invocation of the
fragment shader. That is why every fragment of the triangle primitive
rendered by an object has the same color
*/

layout (location=0) in vec3 vInterpColor;
layout (location=1) in vec2 vTexturecoord;
layout (location=0) out vec4 fFragColor;

uniform sampler2D ourTexture;

uniform int taskNum;

uniform float uTileSize;

uniform bool modul;

void main () {

	vec4 colour1 = vec4(1.0, 0.0, 1.0, 1.0);
	vec4 colour2 = vec4(0.0, 0.68, 0.94, 1.0);
	vec4 modd = vec4(vInterpColor, 1.0);
	switch (taskNum)
	{
		case 0:
			fFragColor = vec4(vInterpColor, 1.0);
			break;
		case 1:
	
			if ((mod(floor(gl_FragCoord.x/32) + floor(gl_FragCoord.y/32), 2.f)) == 0.0f)
			{
				fFragColor = colour1;
			}
			else
			{
				fFragColor = colour2;
			}
			break;
		case 2:
			if ((mod(floor(gl_FragCoord.x/uTileSize) + floor(gl_FragCoord.y/uTileSize), 2.f)) == 0.0f)
			{
				fFragColor = colour1;
			}
			else
			{
				fFragColor = colour2;
			}
			break;
		default:
			fFragColor = texture(ourTexture, vTexturecoord);
			break;
	}
	if (modul == true && taskNum != 0)
	{
		fFragColor = vec4(fFragColor.x * vInterpColor.x, fFragColor.y * vInterpColor.y, fFragColor.z * vInterpColor.z, 1.0) *  texture(ourTexture, vTexturecoord);
	}

}
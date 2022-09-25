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

//uniform vec3 uColor;

layout (location=0) in vec3 vInterpColor;
layout (location=1) in vec2 vTexturecoord;
layout (location=2) in float vTextureIndex;

layout (location=0) out vec4 fFragColor;

uniform sampler2D ourTexture[5];
uniform bool modulatebool;
uniform bool texturebool;
void main () {
	//fFragColor = vec4(uColor, 1.0);
	vec3 modulate = {1.f, 1.f, 1.f};
    if (modulatebool == true) 
	{
        modulate = vInterpColor;
	}
	else if (texturebool == true)
	{
		int index = int(vTextureIndex);
		if (index == 0)
		{
			fFragColor = vec4(vInterpColor, 1.0);
		}
		else
		{
			fFragColor = texture(ourTexture[index], vTexturecoord)  * vec4(modulate, 1.0);
		}
	}
	else
	{
		fFragColor = vec4(vInterpColor, 1.0);
	}
}
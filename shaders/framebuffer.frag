#version 450 core
layout (location=0) in vec3 vInterpColor;
layout (location=1) in vec2 vTexturecoord;
layout (location=2) in float vTextureIndex;

layout (location=0) out vec4 fFragColor;

uniform sampler2D screenTexture;


void main () 
{
	fFragColor = texture(screenTexture, vTexturecoord);
}
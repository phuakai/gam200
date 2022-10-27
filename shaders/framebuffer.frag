#version 450 core
layout (location=0) in vec2 vTexturecoord;

layout (location=0) out vec4 fFragColor;

uniform sampler2D screenTexture;


void main () 
{
	fFragColor = texture(screenTexture, vTexturecoord);
}
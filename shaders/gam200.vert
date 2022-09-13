#version 450 core
layout (location=0) in vec2 aVertexPosition;
//layout (location=1) in vec3 aVertexColor;
layout (location=1) in vec2 aTextureCoord;

layout(location=0) out vec2 vTextureCoord;

void main() 
{
	gl_Position = vec4(vec2(vec3(aVertexPosition, 1.0)), 0.0, 1.0);
	vTextureCoord = aTextureCoord;
}

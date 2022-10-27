#version 450 core
layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aTextureCoord;
layout (location=3) in float aTextureIndex;
layout (location=4) in mat3 aOffset;

layout (location=0) out vec3 vColor;
layout (location=1) out vec2 vTextureCoord;
layout (location=2) out float vTextureIndex;

void main() 
{
	gl_Position = vec4(aVertexPosition, 0.0, 1.0);
	vTextureCoord = aTextureCoord;
}

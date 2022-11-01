#version 450 core
layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aTextureCoord;
layout (location=3) in float aTextureIndex;
layout (location=4) in mat4 aOffset;

layout (location=0) out vec3 vColor;
layout (location=1) out vec2 vTextureCoord;
layout (location=2) out float vTextureIndex;

//uniform mat3 uModel_to_NDC;

void main() 
{
	//gl_Position = vec4(vec2(uModel_to_NDC * vec3(aVertexPosition, 1.0)), 0.0, 1.0);
	//gl_Position = vec4(aVertexPosition, 0.0, 1.0);
	vTextureIndex = aOffset[3][3];
	vColor = vec3(aOffset[0][3], aOffset[1][3], aOffset[2][3]);
	mat4 fakeOffset = aOffset;
	fakeOffset[0][3] = 0;
	fakeOffset[1][3] = 0;
	fakeOffset[2][3] = 0;
	fakeOffset[3][3] = 1;
	gl_Position = vec4(vec3(fakeOffset * vec4(aVertexPosition, 1.0, 1.0)), 1.0);

	vTextureCoord = aTextureCoord;
}

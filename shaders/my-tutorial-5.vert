#version 450 core

layout (location=0) in vec2 aVertexPosition;

layout (location=1) in vec3 aVertexColor;

layout (location=2) in vec2 aVertexTexture;

layout (location=0) out vec3 vColor;

layout (location=1) out vec2 vTexture;

uniform int myUniformTask;

void main(void) 
{
    gl_Position = vec4(aVertexPosition, 0.0, 1.0);
    vColor = aVertexColor;
    if (myUniformTask >= 4 && myUniformTask <= 6)
        vTexture = aVertexTexture * 4;
    else
        vTexture = aVertexTexture;

}
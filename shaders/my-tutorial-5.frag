#version 450 core

layout (location=0) in vec3 vInterpColor;
layout (location=1) in vec2 vTexture;

layout (location=0) out vec4 fFragColor;

uniform sampler2D uniTexture;
uniform int myUniformTask;
uniform bool myUniformModulate;
uniform float uTileSize;

void main () 
{
   //for modulating
   vec3 modulate = {1.f, 1.f, 1.f};
    if (myUniformModulate == true) 
        modulate = vInterpColor;


    switch (myUniformTask)
    {
        case 0:
            fFragColor = vec4(vInterpColor, 1.0);
            break;
        case 1:
            if ((mod(floor(gl_FragCoord.x / 32) + floor(gl_FragCoord.y / 32), 2)) == 0)
                fFragColor = vec4(1.0, 0.0, 1.0, 1.0) * vec4(modulate, 1.0);
            else
                fFragColor = vec4(0.0, 0.68, 0.94, 1.0) * vec4(modulate, 1.0);
            break;
        case 2:
            if ((mod(floor(gl_FragCoord.x / uTileSize) + floor(gl_FragCoord.y / uTileSize), 2)) == 0)
                fFragColor = vec4(1.0, 0.0, 1.0, 1.0)* vec4(modulate, 1.0);
            else
                fFragColor = vec4(0.0, 0.68, 0.94, 1.0) * vec4(modulate, 1.0);
            break;
        case 3:
            fFragColor = texture(uniTexture, vTexture) * vec4(modulate, 1.0);
            break;
        case 4:
            fFragColor = texture(uniTexture, vTexture) * vec4(modulate, 1.0);
            break;
        case 5:
            fFragColor = texture(uniTexture, vTexture) * vec4(modulate, 1.0);
            break;
        case 6:
            fFragColor = texture(uniTexture, vTexture) * vec4(modulate, 1.0);
            break;
        default:
            break;
    }
}
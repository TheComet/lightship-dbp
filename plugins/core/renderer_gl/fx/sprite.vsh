#version 330 core

/* ----------------------------------------------------------------------------
 * map vertex attributes
 * ------------------------------------------------------------------------- */

layout (location = 0) in vec2 vertexPosition_modelSpace;
layout (location = 1) in vec2 texCoord_vertex;

/* ----------------------------------------------------------------------------
 * uniforms
 * ------------------------------------------------------------------------- */

uniform vec2 spritePosition;
uniform vec2 spriteSize;

/* ----------------------------------------------------------------------------
 * fragment output data
 * ------------------------------------------------------------------------- */

out vec2 texCoord;

/* ----------------------------------------------------------------------------
 * vertex shader main
 * ------------------------------------------------------------------------- */

void main()
{
    /* forward texture coordinates */
    texCoord = texCoord_vertex;
    
    vec2 position = vertexPosition_modelSpace * spriteSize + spritePosition;
    gl_Position = vec4(position, 0.0, 1.0);
}

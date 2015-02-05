#version 330 core

/* ----------------------------------------------------------------------------
 * map vertex attributes
 * ------------------------------------------------------------------------- */

layout (location = 0) in vec2 vertexPosition_modelSpace;
layout (location = 1) in vec2 texCoord_vertex;

/* ----------------------------------------------------------------------------
 * fragment output data
 * ------------------------------------------------------------------------- */

out vec2 texCoord;

/* ----------------------------------------------------------------------------
 * vertex shader main
 * ------------------------------------------------------------------------- */

void main()
{
    texCoord = texCoord_vertex;
    gl_Position = vec4(vertexPosition_modelSpace, 0.0, 1.0);
}

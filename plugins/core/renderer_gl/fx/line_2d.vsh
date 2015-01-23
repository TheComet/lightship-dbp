#version 330 core

/* ----------------------------------------------------------------------------
 * map vertex attributes
 * ------------------------------------------------------------------------- */

layout (location = 0) in vec2 vertexPosition_modelSpace;
layout (location = 1) in vec4 vertexColour;

/* ----------------------------------------------------------------------------
 * fragment output data
 * ------------------------------------------------------------------------- */

out vec3 line_colour;

/* ----------------------------------------------------------------------------
 * vertex shader main
 * ------------------------------------------------------------------------- */

void main()
{
    line_colour = vertexColour.xyz;
    gl_Position = vec4(vertexPosition_modelSpace, 0.0, 1.0);
}

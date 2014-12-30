#version 330 core

uniform vec3 line_colour;
out vec3 colour;

void main()
{
    colour = line_colour;
}

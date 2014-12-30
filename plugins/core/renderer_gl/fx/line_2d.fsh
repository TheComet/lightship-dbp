#version 330 core
out vec3 colour;

in vec3 line_colour;

void main()
{
    colour = line_colour;
}

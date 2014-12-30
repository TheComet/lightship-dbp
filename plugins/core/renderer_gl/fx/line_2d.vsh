#version 330 core
layout (location = 0) in vec3 vertexPosition_modelSpace;

void main()
{
    gl_Position = vec4(vertexPosition_modelSpace, 1.0);
}

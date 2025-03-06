#version 330 core

layout (location = 0) in vec3 aPos; 

uniform vec3 position; 

void main()
{
    gl_Position = vec4(aPos + position, 1.0);
}

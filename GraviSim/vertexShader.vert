#version 330 core

layout(location = 0) in vec3 aPos; 

uniform vec2 position;

void main() {
    gl_Position = vec4(aPos.x + position.x, aPos.y + position.y, 0.0, 1.0);

}

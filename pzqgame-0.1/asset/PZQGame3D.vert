#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_Color;
uniform mat4 camera;
out vec2 ex_Color;

void main(void) {
    gl_Position = camera * vec4(in_Position,1.0);
    ex_Color = in_Color;
}
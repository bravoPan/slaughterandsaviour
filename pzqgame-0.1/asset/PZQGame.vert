#version 330 core

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_Color;
out float distToCamera;
out vec2 ex_Color;

void main(void) {
    gl_Position = vec4(in_Position,1.0,1.0);
    distToCamera = 0.0;
    ex_Color = in_Color;
}
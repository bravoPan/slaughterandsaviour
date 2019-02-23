#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_Color;
uniform mat4 camera;
uniform mat4 proj;
out vec2 ex_Color;
out float distToCamera;

void main(void) {
    vec4 t = camera * vec4(in_Position,1.0);
    distToCamera = sqrt(t.x * t.x + t.y * t.y + t.z * t.z);
    gl_Position = proj * t;
    ex_Color = in_Color;
}
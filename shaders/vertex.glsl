#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in int aFaceIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out int faceIndex;

void main() {
    faceIndex = aFaceIndex;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

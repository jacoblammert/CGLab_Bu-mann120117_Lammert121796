#version 150
#extension GL_ARB_explicit_attrib_location: require
layout (location=0) in vec3 pos;

out vec3 direction;

uniform mat4 ProjectionMatrix, ViewMatrix;

void main() {
    direction = pos;
    mat4 vm = mat4(mat3(ViewMatrix));
    gl_Position = (ProjectionMatrix * vm * vec4(pos, 1.0)).xyww;
}
#version 150
#extension GL_ARB_explicit_attrib_location: require
layout (location=0) in vec3 pos;

out vec3 direction;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main() {
    direction = pos; // position of the fragment (pixel) in 3d
    gl_Position = (ProjectionMatrix * mat4(mat3(ViewMatrix)) * vec4(pos, 1.0)).xyww;  // position of the triangles on the screen
}
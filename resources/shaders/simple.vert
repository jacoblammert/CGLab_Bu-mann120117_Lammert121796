#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;

layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_Normal;
out vec3 rotation_vector;
out vec4 pass_Position;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0); // position of each fragment
	pass_Position = ModelMatrix * vec4(in_Position, 1.0); // position in world space
    pass_Normal = normalize(vec3(pass_Position - ModelMatrix * vec4(0,0,0,1))); // Normal vector in world space

    rotation_vector = normalize(vec3(ModelMatrix * vec4(1,0,0,1) - ModelMatrix * vec4(0,0,0,1))); // single vector with the same rotation of the planet (in the horizontal axis)
}

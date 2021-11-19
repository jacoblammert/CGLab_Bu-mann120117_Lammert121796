#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;

layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;


in  vec3 color_ambient;
in  vec3 color_diffuse;
in  vec3 color_specular;

in  vec3 light_pos;

out vec3 light_pos_out;
out vec3 pass_Normal;
out vec3 light_pos_;
out vec4 pass_Position;

out vec3 color_ambient_;
out vec3 color_diffuse_;
out vec3 color_specular_;
out vec3 center;


void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	center = vec3((ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(0,0,0, 1.0));
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	pass_Position = gl_Position;

    light_pos_out = vec3((ProjectionMatrix  * ViewMatrix) * vec4(light_pos, 1.0));

    color_ambient_ = color_ambient;
    color_diffuse_ = color_diffuse;
    color_specular_ = color_specular;

}

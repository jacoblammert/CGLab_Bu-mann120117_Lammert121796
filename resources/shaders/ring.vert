#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
// glVertexAttribPointer mapped color  to second attribute 
layout(location = 1) in vec3 in_Color;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float dist;
uniform float angle;


out vec3 pass_Color;


void main(void)
{
    vec3 position = normalize(vec3(in_Position[0] * cos(angle) - in_Position[2] * sin(angle), 0, sin(angle) * in_Position[0] + cos(angle) * in_Position[2]));
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(position * dist, 1.0);
	// in_Position * dist scales the ring to the size of the orbit
	// the rest transforms the view
	pass_Color = in_Color;
}

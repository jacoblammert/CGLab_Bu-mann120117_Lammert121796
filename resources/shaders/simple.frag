#version 150

in  vec3 pass_Normal;
//in  vec4 gl_Position;
in  vec4 pass_Position;

in  vec3 light_pos_out;

uniform vec3 color_ambient_;
uniform vec3 color_diffuse_;
uniform vec3 color_specular_;

out vec4 out_Color;




void main() {

    // do light caculation

    float distance = 1;//length(vec3(pass_Position) - light_pos_out);

    out_Color = vec4(color_ambient_, 1.0);///(distance * distance)
}

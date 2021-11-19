#version 150

in  vec3 pass_Normal;
//in  vec4 gl_Position;
in  vec4 pass_Position;

in  vec3 light_pos_out;
in  vec3 center;

uniform vec3 color_ambient_;
uniform vec3 color_diffuse_;
uniform vec3 color_specular_;

out vec4 out_Color;




void main() {

    // do light caculation

    float distance = length(vec3(pass_Position) - vec3(light_pos_out));

    float angle = dot(normalize(-light_pos_out + vec3(pass_Position)), normalize(vec3(center-vec3(pass_Position))));

    //out_Color = ((vec4(color_ambient_, 1.0) * angle+1)/2) /distance;///(distance * distance)


    out_Color = vec4(color_ambient_, 1.0) * angle;

    if (out_Color[0]<0){
        out_Color[0] = 0;
    }
    if (out_Color[1]<0){
        out_Color[1] = 0;
    }
    if (out_Color[2]<0){
        out_Color[2] = 0;
    }

    out_Color *= 0.7f;
    out_Color += vec4(color_ambient_,1.0f) * 0.3f;

    out_Color *= 5;



        out_Color[0] = int(out_Color[0]) / 5.0f;
        out_Color[1] = int(out_Color[1]) / 5.0f;
        out_Color[2] = int(out_Color[2]) / 5.0f;
 }

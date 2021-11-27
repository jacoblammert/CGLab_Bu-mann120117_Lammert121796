#version 150

in vec3 pass_Normal;
//in vec4 gl_Position;
in vec4 pass_Position;

in vec3 light_pos_out;
//in vec3 center_planet;
in vec3 center_system;

uniform vec3 color_ambient_;
uniform vec3 color_diffuse_;
uniform vec3 color_specular_;
in vec3 camera_position;

out vec4 out_Color;




void main() {

    // do light caculation

    float distance = length(vec3(pass_Position) - vec3(light_pos_out));

    vec3 N = normalize(pass_Normal);
    vec3 L = normalize(center_system  - vec3(pass_Position));
    vec3 V = normalize(camera_position - vec3(pass_Position));

    vec3 H = normalize(L + V);

    float angle_specular = max(dot(H, N), 0.0);
    out_Color = vec4((color_ambient_ + color_diffuse_ * max(dot(N, L), 0)) + color_specular_ * pow(max(dot(N,H),0),30), 1.0);
    if (out_Color[0]<0){
        out_Color[0] = 0;
    }
    if (out_Color[1]<0){
        out_Color[1] = 0;
    }
    if (out_Color[2]<0){
        out_Color[2] = 0;
    }

    // if toonshading:
    /**/
    float stroke_thickness = 0.4;
    float normal_dir = dot(N,V);
    if (-stroke_thickness < normal_dir && normal_dir < stroke_thickness){
        out_Color = vec4(1.0f,1.0f,1.0f,1);
    }
    float layers = 5.0f;
    out_Color *= layers;
    out_Color[0] = int(out_Color[0]) / layers;
    out_Color[1] = int(out_Color[1]) / layers;
    out_Color[2] = int(out_Color[2]) / layers;/**/
 }

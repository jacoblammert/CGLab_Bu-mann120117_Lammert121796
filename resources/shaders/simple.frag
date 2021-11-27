#version 150

in vec3 pass_Normal;
//in vec4 gl_Position;
in vec4 pass_Position;

uniform vec3 light_pos;
uniform float light_intensity; // vec3(ambient, diffuse, specular) or float?
uniform vec3 camera_position;

uniform vec3 color_ambient_;
uniform vec3 color_diffuse_;
uniform vec3 color_specular_;
uniform bool toon_shading;

out vec4 out_Color;




void main() {

    // do light caculation

    //float distance = length(vec3(pass_Position) - vec3(light_pos)); // for brightness??

    vec3 N = normalize(pass_Normal);
    vec3 L = normalize(light_pos  - vec3(pass_Position));
    vec3 V = normalize(camera_position - vec3(pass_Position));

    vec3 H = normalize(L + V);

    float angle_specular = max(dot(H, N), 0.0);
    out_Color = vec4((color_ambient_ + light_intensity * color_diffuse_ * max(dot(N, L), 0)) + light_intensity * color_specular_ * pow(max(dot(N,H),0),30), 1.0);

    if (toon_shading){
        float stroke_thickness = 0.4;
        float normal_dir = dot(N,V);
        if (-stroke_thickness < normal_dir && normal_dir < stroke_thickness){
            out_Color = vec4(0.5f,0.5f,0.5f,1);
        }
        float layers = 5.0f;
        out_Color *= layers;
        out_Color[0] = int(out_Color[0]) / layers;
        out_Color[1] = int(out_Color[1]) / layers;
        out_Color[2] = int(out_Color[2]) / layers;
    }
 }

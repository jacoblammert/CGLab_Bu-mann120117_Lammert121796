#version 150

in vec3 pass_Normal;
in vec4 pass_Position;

uniform vec3 light_pos;
uniform float light_intensity; // vec3(ambient, diffuse, specular) or float?
uniform vec3 camera_position;

uniform vec3 color_diffuse_; // color of the planet
uniform vec3 color_ambient_; // color of the light
uniform vec3 color_specular_;// specular part of the light
uniform bool toon_shading;

out vec4 out_Color;


void main() {
    // do light caculation

    //float distance = length(vec3(pass_Position) - vec3(light_pos)); // for brightness??

    vec3 L = normalize(light_pos  - vec3(pass_Position)); // vector from point to light
    vec3 V = normalize(camera_position - vec3(pass_Position)); // vector from point to camera

    vec3 H = normalize(L + V); // vector between point to camera and point to light

    float angle_specular = max(dot(H, pass_Normal), 0.0f); // specular angle
    float angle_diffuse = max(dot(pass_Normal, L), 0.0f); // diffuse angle
    // light calculations:
    out_Color = vec4((color_ambient_ + light_intensity * color_diffuse_ * angle_diffuse) + light_intensity * color_specular_ * pow(angle_specular,30), 1.0);

    if (toon_shading){ // toon shading boolean
        float stroke_thickness = 0.4; // angle of grey area
        float normal_dir = dot(pass_Normal,V); // angle camera to point and normal
        if (normal_dir < stroke_thickness){ // if we have a point on the edge, we paint it grey
            out_Color = vec4(0.5f,0.5f,0.5f,1); // outline color
        }

        float layers = 5.0f; // total number of colors between 0 and 1
        out_Color *= layers; // scale from 0 1 to value from 0 to total nr of colors
        out_Color[0] = int(out_Color[0]) / layers; // converion to int and then division by total nr of colors for value between 0 and 1
        out_Color[1] = int(out_Color[1]) / layers; // converion to int and then division by total nr of colors for value between 0 and 1
        out_Color[2] = int(out_Color[2]) / layers; // converion to int and then division by total nr of colors for value between 0 and 1
    }
 }

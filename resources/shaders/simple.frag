#version 150

in vec3 pass_Normal;
in vec4 pass_Position;
in vec3 rotation_vector;

uniform vec3 light_pos;
uniform float light_intensity; // vec3(ambient, diffuse, specular) or float?
uniform vec3 camera_position;

uniform vec3 color_diffuse_; // color of the planet
uniform vec3 color_specular_;// specular part of the light
uniform bool toon_shading;
uniform bool sun;

out vec4 out_Color;
uniform sampler2D texture_;
uniform sampler2D texture_normal;

void main() {
    // do light caculation

    //float distance = length(vec3(pass_Position) - vec3(light_pos)); // for brightness??


    if (dot(normalize(camera_position - vec3(pass_Position)),pass_Normal) > 0){

        vec2 horizontal = normalize(vec2(pass_Normal[0],pass_Normal[2]));

        vec2 rotation = normalize(vec2(rotation_vector[0],rotation_vector[2]));

        float hor = atan(dot(horizontal,rotation),horizontal[0] * rotation[1] - rotation[0] * horizontal[1])/(3.142f * 2) + 0.5f;
        // value between 0 and 1 for our horizontal pixel position
        float ver = asin(pass_Normal.y)/3.142f + 0.5f;
        // value between 0 and 1 for our vertical pixel position

        vec4 color = texture(texture_,vec2(hor,ver));
        vec3 color_normal = 2 * (vec3(texture(texture_normal,vec2(hor,ver))) - vec3(0.5f,0.5f,0.5f));

        vec3 right = normalize(cross(pass_Normal,vec3(0,1,0)));
        vec3 up = normalize(cross(pass_Normal,right));

        vec3 new_normal = normalize(pass_Normal * color_normal[2] - right * color_normal[0] - up * color_normal[1]);





        vec3 L = normalize(light_pos  - vec3(pass_Position)); // vector from point to light
        vec3 V = normalize(camera_position - vec3(pass_Position)); // vector from point to camera

        vec3 H = normalize(L + V); // vector between point to camera and point to light

        float angle_specular = max(dot(H, new_normal), 0.0f); // specular angle
        float angle_diffuse = max(dot(pass_Normal, L), 0.0f); // diffuse angle
        // light calculations:


        out_Color = vec4((color.rgb * 0.1f + light_intensity * color.rgb * angle_diffuse) + max(dot(pass_Normal, L) + 0.25f, 0.0f) * light_intensity * color_specular_ * pow(angle_specular,30), 1.0);

        if (sun){
            out_Color = color;
        }

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
 }

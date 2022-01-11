#version 150

in vec2 pass_TexCoord;

out vec4 out_Color;

uniform sampler2D screenTexture;

uniform vec2 textureSize;

uniform bool luminance_preserving_greyscale;
uniform bool horizontal_mirroring;
uniform bool vertical_mirroring;
uniform bool gaussian_blur;

vec2 tex_coords = pass_TexCoord;

void main() {

    // changing the 2d texture coordinate first
    if (horizontal_mirroring){
        tex_coords = vec2(1 - tex_coords.x, tex_coords.y);
    }
    if (vertical_mirroring){
        tex_coords = vec2(tex_coords.x, 1 - tex_coords.y);
    }
	out_Color = texture(screenTexture, tex_coords);

    if (gaussian_blur){
        float pixel_size_x = 1.0f / textureSize.x;
        float pixel_size_y = 1.0f / textureSize.x;
        out_Color *= 4;
        out_Color += texture(screenTexture, tex_coords + vec2(pixel_size_x,0)) * 2;     // pixel right
        out_Color += texture(screenTexture, tex_coords - vec2(pixel_size_x,0)) * 2;     // pixel left
        out_Color += texture(screenTexture, tex_coords + vec2(0,pixel_size_y)) * 2;     // pixel above
        out_Color += texture(screenTexture, tex_coords - vec2(0,pixel_size_y)) * 2;     // pixel below

        out_Color += texture(screenTexture, tex_coords + vec2(-pixel_size_x,-pixel_size_y)); // pixel above left
        out_Color += texture(screenTexture, tex_coords + vec2(pixel_size_x,-pixel_size_y));  // pixel below right
        out_Color += texture(screenTexture, tex_coords + vec2(-pixel_size_x,pixel_size_y));  // pixel above left
        out_Color += texture(screenTexture, tex_coords + vec2(pixel_size_x,pixel_size_y));   // pixel above right
        out_Color /= 16;
    }

	if(luminance_preserving_greyscale) {
	// rgb multiplier values from: https://stackoverflow.com/questions/17615963/standard-rgb-to-grayscale-conversion
		float luminance = (0.2126 * out_Color.r + 0.7152 * out_Color.g + 0.0722 * out_Color.b);
		out_Color = vec4(luminance,luminance,luminance,1.0);
	}
}
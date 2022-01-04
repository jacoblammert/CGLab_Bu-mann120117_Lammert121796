#version 150

in vec2 pass_TexCoord;

out vec4 out_Color;

uniform sampler2D screenTexture;

uniform vec2 textureSize;

uniform bool horizontalMirroring;
uniform bool verticalMirroring;
uniform bool greyscale;
uniform bool blur;

vec2 tex_coords = pass_TexCoord;

void main() {

	out_Color = texture(screenTexture, tex_coords);

	if(greyscale) {
		float luminance = (0.2126*out_Color.r+0.7152*out_Color.g+0.0722*out_Color.b);
		out_Color = vec4(luminance,luminance,luminance,1.0);
	}
}
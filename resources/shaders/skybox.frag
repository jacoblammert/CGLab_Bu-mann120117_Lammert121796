#version 150

in vec3 direction;
out vec4 color;
uniform samplerCube skybox_texture;

void main() {
    color = texture(skybox_texture, direction); // texture sampling
}

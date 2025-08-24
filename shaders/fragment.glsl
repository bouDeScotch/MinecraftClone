#version 330 core

in vec2 fragUV;

uniform sampler2D textureAtlas;

out vec4 FragColor;

void main() {
    FragColor = texture(textureAtlas, fragUV);
}


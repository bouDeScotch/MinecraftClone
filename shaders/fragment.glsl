#version 330 core
in vec2 fragUV;
in vec3 fragNormal;

uniform sampler2D textureAtlas;
uniform vec3 lightDir; // direction du soleil

out vec4 FragColor;

// petit bruit pour effet subtil
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec3 normal = normalize(fragNormal); 
    float ambient = 0.3;
    float lightFactor = max(dot(normal, lightDir), 0.0);
    vec3 color = texture(textureAtlas, fragUV).rgb * (ambient + (1.0 - ambient) * lightFactor);
    color += vec3(rand(fragUV) * 0.05); // ajout de bruit subtil
    FragColor = vec4(color, 1.0);
}

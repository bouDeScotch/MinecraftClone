#version 330 core

in vec2 fragUV;
in vec3 fragNormal;

uniform sampler2D textureAtlas;

uniform vec3 lightDir;

out vec4 FragColor;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 computeLighting(vec3 normal, vec3 lightDir) {
    normal = normalize(normal);
    lightDir = normalize(lightDir);

    // composante ambiante faible
    float ambient = 0.2;

    // diffuse Lambert
    float diffuse = max(dot(normal, lightDir), 0.0);

    // petit effet spéculaire "fake" pour adoucir les arêtes
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0)); // on suppose caméra fixe
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0); // exponent faible pour un éclat subtil
    spec *= 0.1; // réduire l'intensité

    return vec3(ambient + diffuse + spec);
}


void main() {
    vec3 lightFactor = computeLighting(fragNormal, lightDir);
    float noise = rand(fragUV * 100.0) * 0.1; // bruit subtil
    lightFactor += noise;
    vec3 color = texture(textureAtlas, fragUV).rgb * lightFactor;
    FragColor = vec4(color, 1.0);
}


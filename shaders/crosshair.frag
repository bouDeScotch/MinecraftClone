#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform vec2 u_ScreenSize; // résolution de l'écran (width, height)
uniform float size;      // taille du crosshair en pixels

void main()
{
    // Coordonnées normalisées [-1, 1]
    vec2 uv = gl_FragCoord.xy / u_ScreenSize;
    vec2 center = vec2(0.5, 0.5);

    // Ajuster ratio pour garder carré
    float aspect = u_ScreenSize.x / u_ScreenSize.y;
    vec2 offset = uv - center;
    offset.x *= aspect;

    // Taille relative
    float halfSize = size / u_ScreenSize.y; // taille basée sur hauteur

    // Test si on est dans la croix (un petit carré)
    if(abs(offset.x) < halfSize && abs(offset.y) < halfSize) {
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0); // transparent
    }
}

#version 330 core
out vec4 FragColor;

in vec2 fragUV; // il faudra passer des UV 0-1 dans le quad

uniform vec3 sunColor; // couleur centrale du soleil

void main()
{
    // Convertir UV en coordonnées centrées (-0.5 à +0.5)
    vec2 centered = fragUV - vec2(0.5);
    float dist = length(centered); // distance au centre
    float alpha = smoothstep(0.5, 0.3, dist); // dégradé, 0.45 = bord intérieur, 0.5 = bord extérieur
    FragColor = vec4(sunColor, alpha);
}

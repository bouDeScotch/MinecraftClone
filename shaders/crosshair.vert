#version 330 core

layout(location = 0) in vec2 aPos; // position en 2D (-1 à 1)

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0); // position en 3D (x, y, z, w)
}


#version 330 core

flat in int faceID;

out vec4 FragColor;

void main() {
    switch(faceID) {
        case 0:
            FragColor = vec4(0.1, 0.8, 0.1, 1.0);
            break;
        case 1:
            FragColor = vec4(0.8, 0.1, 0.1, 1.0);
            break;
        case 2:
            FragColor = vec4(0.1, 0.1, 0.8, 1.0);
            break;
        default:
            FragColor = vec4(0.8, 0.8, 0.1, 1.0);
            break;
    }
}


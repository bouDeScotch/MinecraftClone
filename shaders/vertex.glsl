#version 430 core
// layout(location=0) in vec3 aPos;
// layout(location=1) in vec2 aUV;
// layout(location=2) in vec3 aNormal;
// layout(location=3) in int  aFaceTexID;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in int aFaceID;
out vec2 fragUV;
out vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int atlasSize;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    int actualFaceID = aFaceID;

    int faceX = actualFaceID % atlasSize;
    int faceY = actualFaceID / atlasSize;
    fragUV = vec2(aUV.x, 1.0 - aUV.y) / float(atlasSize) + vec2(faceX, faceY) / float(atlasSize);
    fragNormal = aNormal;
}

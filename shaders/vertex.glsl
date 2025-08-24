#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in int aFaceID;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 iOffset;
layout(location = 5) in int iTopFaceID;
layout(location = 6) in int iSideFaceID;
layout(location = 7) in int iBottomFaceID;

out vec2 fragUV;
out vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int atlasSize;

void main() {
    gl_Position = projection * view * model * vec4(aPos + iOffset, 1.0);

    int actualFaceID;
    switch(aFaceID) {
        case 0: actualFaceID = iTopFaceID; break;
        case 1: actualFaceID = iSideFaceID; break;
        case 2: actualFaceID = iBottomFaceID; break;
        default: actualFaceID = 0; break;
    }

    int faceX = actualFaceID % atlasSize;
    int faceY = actualFaceID / atlasSize;
    fragUV = vec2(aUV.x, 1.0 - aUV.y) / float(atlasSize) + vec2(faceX, faceY) / float(atlasSize);
    fragNormal = aNormal;
}

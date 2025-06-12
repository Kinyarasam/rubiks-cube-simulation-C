#version 330 core
out vec4 FragColor;

// Receive face index from vertex shader
flat in int faceIndex;

// Cubelet face colors (set as uniform)
uniform vec3 faceColors[6];

void main() {
    if (faceIndex == -1) discard; // Don't render internal faces
    
    FragColor = vec4(faceColors[faceIndex], 1.0);
}

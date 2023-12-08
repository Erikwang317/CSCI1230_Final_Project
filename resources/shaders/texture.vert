#version 330 core

layout(location = 0) in vec4 centerPosition;
layout(location = 1) in vec2 texOffset; // Assuming you have a per-vertex texture offset

uniform mat4 u_ViewProjection;
uniform float u_Size;

out vec2 TexCoords;

void main() {
    vec2 texCoords[4] = vec2[](
        vec2(0.0, 1.0), // Top-left
        vec2(1.0, 1.0), // Top-right
        vec2(0.0, 0.0), // Bottom-left
        vec2(1.0, 0.0)  // Bottom-right
    );

    // Pass the texture coordinates to the fragment shader
    TexCoords = texCoords[gl_VertexID] + texOffset;

    // ... rest of your vertex shader code ...
}


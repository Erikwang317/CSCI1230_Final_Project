#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float life;

uniform mat4 u_ViewProjection; // ProjectionMatrix * ViewMatrix * ModelMatrix(=mat4(1.0f))
const vec3 u_cameraUp = vec3(0.f, 1.f, 0.f);
uniform vec3 u_cameraRight;
uniform float u_aspectRatio;
uniform float u_max_life;

out float out_life;
out vec4 out_position;
out vec2 out_texCoords;

void main() {
    out_life = life;

    vec2 offsets[4] = vec2[](
        vec2(-0.5, 0.5),  // Top-left
        vec2(0.5, 0.5),   // Top-right
        vec2(-0.5, -0.5), // Bottom-left
        vec2(0.5, -0.5)   // Bottom-right
    );

    const vec2 texCoords[] = vec2[](
        vec2(0.0, 1.0), // Top-left
        vec2(1.0, 1.0), // Top-right
        vec2(0.0, 0.0), // Bottom-left
        vec2(1.0, 0.0)  // Bottom-right
    );
    out_texCoords = texCoords[gl_VertexID % 4];
    // Calculate the world position for each vertex of the quad

    float size = 0.1f * (out_life/u_max_life) * u_aspectRatio;
    vec3 worldPosition = position.xyz
                         + u_cameraRight * offsets[gl_VertexID % 4].x * size * 0.175f
                         + u_cameraUp * offsets[gl_VertexID % 4].y * size;

    out_position = vec4(worldPosition, 1.f);
    gl_Position = u_ViewProjection * vec4(worldPosition, 1.f);
}


#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in float life;

uniform mat4 u_ViewProjection; // ProjectionMatrix * ViewMatrix * ModelMatrix(=mat4(1.0f))

out float out_life;
out vec4 out_position;

void main()
{
    out_life = life;
    out_position = position;

    gl_Position = u_ViewProjection * position;
}

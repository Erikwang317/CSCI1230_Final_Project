#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in float life;

uniform mat4 u_ViewProjection; // ProjectionMatrix * ViewMatrix * ModelMatrix(=mat4(1.0f))

out float out_life;

void main()
{
    gl_Position = u_ViewProjection * vec4(vec3(position), 1.0f);

    if (life < 0.0f){
        life = 0.0f;
    }
    out_life = life;
}

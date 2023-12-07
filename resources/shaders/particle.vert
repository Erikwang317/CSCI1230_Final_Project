#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in float life;

uniform mat4 u_ViewProjection; // ProjectionMatrix * ViewMatrix * ModelMatrix(=mat4(1.0f))
const vec3 u_cameraUp = vec3(0.f, 1.f, 0.f);
uniform vec3 u_cameraRight;
uniform float u_max_life;

out float out_life;
out vec4 out_position;
//out vec2 out_texCoords; // If we're using a texture

//void main()
//{
//    out_life = life;
//    out_position = position;

//    gl_Position = u_ViewProjection * position;
//}

void main()
{
    out_life = life;

    vec3 worldPosition = position.xyz;
    float minSize = 0.01;
    float maxSize = 0.1;
    float size = mix(minSize, maxSize, life / u_max_life);
    vec3 vertexPos = worldPosition + (u_cameraRight * vec3(position.xy,0.f) + u_cameraUp * vec3(position.zw,0.f)) * size;
    out_position = vec4(vertexPos, 1.f);

    //out_texCoords = position.xy * 0.5 + 0.5; //  If we're using a texture
    gl_Position = u_ViewProjection * vec4(vertexPos, 1.f);

}

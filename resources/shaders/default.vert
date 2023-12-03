#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

out vec3 outPosition;
out vec3 outNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {

    vec4 worldPosition = ModelMatrix*vec4(inPosition, 1.0);
    vec3 worldNormal = normalize( transpose(inverse(mat3(ModelMatrix)))*inNormal.xyz );

    outPosition = worldPosition.xyz;
    outNormal = worldNormal;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);

    //gl_Position = vec4(inPosition, 1.0);
}

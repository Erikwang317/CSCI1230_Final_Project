#version 330

in float out_life;
in vec4 out_position;

uniform vec4 u_bornColor;
uniform vec4 u_deadColor;
uniform float u_max_life;

out vec4 fragColor;


void main()
{
    fragColor = u_bornColor*(out_life/u_max_life) + 0.5*u_deadColor*(1.0f-out_life/u_max_life);
    float r = clamp(out_position.x,0.0f,1.0f);
    float g = clamp(out_position.z,0.0f,1.0f);
    float b = clamp(out_position.y,0.0f,1.0f);

    fragColor = fragColor + 0.5*vec4(r,g,b,1.0f);

    fragColor = u_bornColor;
}


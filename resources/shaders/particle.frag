#version 330

in float out_life;

uniform vec4 u_bornColor;
uniform vec4 u_deadColor;

out vec4 fragColor;


void main()
{
    fragColor = u_bornColor*out_life + u_deadColor*(1.0f-out_life);
}

#version 330

in float out_life;
in vec4 out_position;
in vec2 out_texCoords;

uniform vec4 u_bornColor;
uniform vec4 u_deadColor;
uniform float u_max_life;
uniform sampler2D u_texture; // If we're using a texture
uniform sampler2D u_beckground_texture;
uniform bool isBackground;


out vec4 fragColor;


void main()
{
    if (isBackground) {
        fragColor = texture(u_beckground_texture, out_texCoords);

    } else {
        if(out_life == -1.0f){
            discard;
        }
        vec4 particleColor = u_bornColor*(out_life/u_max_life) + 1.0*u_deadColor*(1.0f-out_life/u_max_life);

        float r = clamp(out_position.x, 0.f, 1.f);
        float g = clamp(out_position.z, 0.f, 1.f);
        float b = clamp(out_position.y, 0.f, 1.f);

        particleColor = particleColor + 0.5*vec4(r,g,b,1.f);


    //    vec4 particleColor = positionColor*(out_life/u_max_life) + 1.0*vec4(1.0f,1.0f,1.0f,1.0f)*(1.0f-out_life/u_max_life);


        vec4 textureColor = texture(u_texture, out_texCoords);
        if (textureColor.a < 0.1)
            discard;

        float blendFactor = 0.5f;
        fragColor = mix(particleColor, textureColor, blendFactor);

    }
}






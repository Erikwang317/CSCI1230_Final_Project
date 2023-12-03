#version 330 core

in vec2 out_uv_coord;
//in vec2 neighbors[25];

uniform sampler2D s2D;
uniform bool isPerPixelFilter;
uniform bool isKernelBasedFilter;
uniform int blurRadius;   // Radius of the blur operation
uniform vec2 texelSize;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.0);
    int kernelsize = (2*blurRadius+1)*(2*blurRadius+1);

    if (isKernelBasedFilter){
        for (float x = -blurRadius; x <= blurRadius; x++) {
            for (float y = -blurRadius; y <= blurRadius; y++) {
                fragColor += (1.0f/kernelsize)*texture(s2D, out_uv_coord+vec2(x,y)*texelSize);
            }
        }
//        for (int i=0; i<kernelsize; i++){
//            fragColor += (0.5f/kernelsize)*texture(s2D, neighbors[i]);
//        }
    }
    else{
        fragColor = texture(s2D, out_uv_coord);
    }

    if (isPerPixelFilter){
        fragColor = 1-fragColor;
    }
}


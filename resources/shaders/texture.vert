#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;

//uniform bool isKernelBasedFilter;
uniform int blurRadius;

out vec2 out_uv_coord;
//out vec2[25] neighbors;

void main() {
    out_uv_coord = uv_coord;
    gl_Position = vec4(position, 1.0);

//    if (isKernelBasedFilter){
//        int i=0;
//        for (float x = -blurRadius; x <= blurRadius; x++) {
//            for (float y = -blurRadius; y <= blurRadius; y++) {
//                neighbors[i] = uv_coord + vec2(x,y);
//                i++;
//            }
//        }
//    }

}

__kernel void particle(__global float4* position_list,
                       __global float4* velocity_list,
                       __global float4* randveloffset_list,
                       __global float* life_list,
                       float dt
                       )
{
    int i = get_global_id(0);
    // float3 is an alias for float4, why are you such a dick OpenCL?

    if (life_list[i] > 0.0f){
        velocity_list[i] -= 10.0f*velocity_list[i] * dt;
        if (i>0){
            velocity_list[i-1] += 2.0f*velocity_list[i] * dt;
        }
        velocity_list[i].x += 0.5*position_list[i].y+0.5*position_list[i].z;
        velocity_list[i].y += 0.5*position_list[i].x+0.5*position_list[i].z;
        velocity_list[i].z += 0.5*position_list[i].y+0.5*position_list[i].x;

        position_list[i] = position_list[i] + velocity_list[i] * dt;
        // life_list[i] = life_list[i] - 0.5*dt;


    }
}

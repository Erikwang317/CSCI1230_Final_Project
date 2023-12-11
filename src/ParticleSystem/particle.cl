__kernel void particle(__global float4* position_list,
                       __global float4* velocity_list,
                       __global float4* randveloffset_list,
                       __global float* life_list,
                       float4 init_vel,
                       float4 cursor_field,
                       float4 emission_pos,
                       float dt
                       )
{
    int i = get_global_id(0);
    // float3 is an alias for float4, why are you such a dick OpenCL?
    // hahaha

    if (life_list[i] > 0.0f){
        if (cursor_field.w == -1.0f){
//            velocity_list[i].y = velocity_list[i].y - 1.0f * dt;
            position_list[i] = position_list[i] + velocity_list[i] * dt;
        }
        else{
//            velocity_list[i].y = velocity_list[i].y - 0.001f * dt;
            velocity_list[i] += cursor_field * dt * 0.01f;
            position_list[i] = position_list[i] + velocity_list[i] * dt;
        }

        life_list[i] = life_list[i] - 1*dt;
    }
    else{
        life_list[i] = 1.2f;
        position_list[i].x = emission_pos.x;
        position_list[i].y = emission_pos.y;
        position_list[i].z = emission_pos.z;
        position_list[i].w = 1.0f;
//        velocity_list[i].x = 0.0f+randveloffset_list[i].x;
//        velocity_list[i].y = 0.9f+randveloffset_list[i].y;
//        velocity_list[i].z = 0.0f+randveloffset_list[i].z;
//        velocity_list[i].w = 0.0f;
        velocity_list[i].x = init_vel.x+randveloffset_list[i].x;
        velocity_list[i].y = init_vel.y+randveloffset_list[i].y;
        velocity_list[i].z = init_vel.z+randveloffset_list[i].z;
        velocity_list[i].w = init_vel.w;
    }
}

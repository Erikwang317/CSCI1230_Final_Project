__kernel void particle(__global float4* position_list,
                       __global float4* velocity_list,
                       __global float4* randveloffset_list,
                       __global float* life_list,
                       float dt
                       )
{
//    int i = get_global_id(0);
//    // float3 is an alias for float4, why are you such a dick OpenCL?

//    if (life_list[i] > 0.0f){
//        velocity_list[i] -= velocity_list[i] * 1.0f * dt;
//        position_list[i] = position_list[i] + velocity_list[i] * dt;
//    }

    int i = get_global_id(0);
    int global_size = (int)get_global_size(0);
    float4 acceleration = (float4)(0.0f, 0.0f, 0.0f, 0.0f);  // Initialize acceleration to zero
    float4 target_position1 = (float4)(0.0f, 0.4f, 0.0f, 1.0f);

    float attraction_strength = 1.0f;
    float cohesion_strength = 0.8f;


    if (life_list[i] > 0.0f) {
        // Calculate attraction force towards the target position
        float4 attraction_force1 = (float4)(target_position1.x - position_list[i].x,
                                           target_position1.y - position_list[i].y,
                                           target_position1.z - position_list[i].z,
                                           0.0f);


        // Cohesion: Move towards the center of mass of nearby particles
        float4 cohesion_force = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for (int j = 0; j < 30; ++j) {
            int k = (i+j+1)%global_size;
            if (life_list[k] > 0.0f) {
                cohesion_force += (float4)(position_list[k].x - position_list[k].x,
                                            position_list[k].y - position_list[k].y,
                                            position_list[k].z - position_list[k].z, 0.0f);
            }
        }


        float4 total_force = attraction_strength*(attraction_force1) +
                             cohesion_strength*cohesion_force;
        // Normalize the force
        float distance = length(total_force);
        total_force /= distance;

        // Apply the force with attraction strength
        acceleration += total_force;

        // Update velocity and position using the acceleration
        velocity_list[i] += acceleration * dt;
        position_list[i] += velocity_list[i] * dt;
    }


}

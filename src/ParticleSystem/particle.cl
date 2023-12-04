__kernel void particle(__global float3* position,
                       __global float3* velocity,
                       __global float3* randveloffset,
                       __global float* life,
                       float dt
                       )
{
    // operate only on the particle that we're supposed to
    int workSize = get_local_size(0);	// number of kernel instances in this work group
    int i = (get_group_id(0) * workSize) + get_local_id(0);
    // Alternatively, do:
    // size_t i = get_global_id(0);

    // retrieve particle position
    float3 *posPtr = &position[i];
    // retrieve particle velocity
    float3 *velPtr = &velocity[i];
    // retrieve particle life
    float *lifePtr = &life[i];

    // (life==-1.0f) means particle not created yet
    if (*lifePtr != -1.0f){

        // reset from emit point if life ends or position.y fall below -5.0f
        if (*lifePtr < 0.0f || posPtr[2] < -5.0f){
            // reset pos/vel/life
            *posPtr = float3(0.0f,0.0f,0.0f);

            float3 randoffset = randveloffset[i];
            *velPtr[0] = 0.0f+randoffset[0];
            *velPtr[1] = 10.0f+randoffset[1];
            *velPtr[2] = 0.0f+randoffset[2];

            *lifePtr = 1.0f;
        }
        else {
            // update(dereference) velocity's value
            *velPtr += -9.8 * dt;
            // update(dereference) position's value
            *posPtr += vel * dt;
            // update(dereference) life's value
            *lifePtr -= dt;
        }
    }

}

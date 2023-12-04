__kernel void particle(__global float3* particles, float dt, int count)
{
    // operate only on the particle that we're supposed to
    int workSize = get_local_size(0);	// number of kernel instances in this work group
    size_t i = (get_group_id(0) * workSize) + get_local_id(0);
    // Alternatively, do:
    // size_t i = get_global_id(0);

    float3 pos;
    float3 vel;
    // retrieve current work group particle position
    __global float3 *posPtr = &particles[i];
    // retrieve its velocity
    __global float3 *velPtr = posPtr + count;

    // update(dereference) velocity's value
    *(velPtr) += -9.8 * dt;
    // update(dereference) position's value
    *(posPtr) += vel * dt;
}

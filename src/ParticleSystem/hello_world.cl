__kernel void square(global int* input, global int* output){
    size_t i = get_global_id(0);
    output[i] = input[i] * input[i];
}

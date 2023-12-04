#include "hello_world.h"
#include <iostream>
#include <filesystem>

#define MAX_SOURCE_SIZE (0x100000)


void print_info(){
    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);

    if (platformCount == 0) {
        std::cerr << "No OpenCL platforms found." << std::endl;
        exit(1);
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    cl_uint deviceCount;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);

    if (deviceCount == 0) {
        std::cerr << "No OpenCL devices found." << std::endl;
        exit(1);
    }

    std::cout << "Number of OpenCL devices: " << deviceCount << std::endl;

    cl_device_id* devices = new cl_device_id[deviceCount];
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, deviceCount, devices, nullptr);

    for (cl_uint i = 0; i < deviceCount; ++i) {
        char deviceName[1024];
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);

        cl_uint maxWorkItemSizes[3];
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSizes), maxWorkItemSizes, nullptr);

        std::cout << "Device " << i << ": " << deviceName;
        std::cout << " (Max Work Group Sizes: [" << maxWorkItemSizes[0] << ", " << maxWorkItemSizes[1] << ", " << maxWorkItemSizes[2] << "])";
        std::cout << std::endl;
    }

    delete[] devices;


}


void hello_world(){

    std::cout << "Hello World OpenCL" << std::endl;

    print_info();

    int i;
    const int LIST_SIZE = 10;
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    std::filesystem::path relativePath = "src/ParticleSystem/hello_world.cl";
    std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);

    fp = fopen(absolutePath.c_str(), "r");
    if (!fp) {
        fprintf(stderr, "Fail load kernel.\n");
        exit(1);
    }
    else{
        fprintf(stdout, "Sucess load kernel.\n");
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id  platform_id = NULL;
    cl_device_id    device_id = NULL;
    cl_uint         ret_num_devices;
    cl_int          ret;

    // Setup OpenCL
    ret = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    std::cout << "ret code: " << ret << std::endl;
    cl_device_id* devices = (cl_device_id*)malloc(sizeof(cl_device_id) * ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "square", &ret);

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                      LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                      LIST_SIZE * sizeof(int), NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
                               LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
                               LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(a_mem_obj), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(b_mem_obj), (void *)&b_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    // size_t local_item_size = 2; // Divide work items into groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                 &global_item_size, NULL, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    ret = clEnqueueReadBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
                              LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    clFinish(command_queue);

    // Display the result to the screen
    for(i = 0; i < LIST_SIZE; i++)
        printf("%d * %d = %d\t", A[i], A[i], B[i]);
    printf("\n");

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);

    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(A);
    free(B);
}

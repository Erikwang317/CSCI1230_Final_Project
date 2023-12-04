#pragma once


#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <OpenCL/opencl.h>
#include <string>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define RAND ((float) rand()) / (float) RAND_MAX

/* May be useful for quad particle system later
*/
//struct ParticleAttributes{
//    glm::vec3 position  = glm::vec3(0,0,0);
//    glm::vec3 velocity  = glm::vec3(0,0,0);
//    glm::vec3 accel     = glm::vec3(0,0,0);
//    glm::vec3 rot_axis  = glm::vec3(0,1,0); // quad
//    float rot_angle     = 0.0f; // degrees //quad

//    float life = 0.0f;
//    float mass = 0.0f; // quad

//    float dist_from_camera = 0.0f;
//    bool operator < (const particleAttributes & p) const
//    {
//        return dist_from_camera < p.dist_from_camera;
//    }
//    bool operator > (const particleAttributes& p) const
//    {
//        return dist_from_camera > p.dist_from_camera;
//    }
//};



/* Basic fountain particle effect
 * Emits from origin, shoots up with random initial velocity, drop off the screen, then respawn
 * This is a point particle system. Implement quad particle system later
*/
class ParticleManager{

public:
    int m_active_particles = 0;
    int m_num_of_particles;

    /* Particle attributes, maintained via OpenCL */
    std::vector<float> m_particle_life;

    std::vector<glm::vec3> m_particle_velocity;
    std::vector<glm::vec3> m_particle_randVelOffset;
    // std::vector<int> m_particle_indices; // quad
    // std::vector<glm::mat4> m_translations; // quad
    // std::vector<glm::mat4> m_rotations; // quad
    // std::vector<float> m_scales; // quad

    /* ParticleManager member functions for init + render + update */
    ParticleManager(int number);
    ~ParticleManager();
    void changeNumParticles(int new_number);
    void render(const glm::mat4 &ViewProjection);
    void update(float time, float dt);
    void create(int id);
    // glm::vec4 calculateBillboardRotationMatrix(glm::vec3 particle_pos, glm::vec3 camera_pos); // quad

private:
    glm::vec3 m_emit_pos;

    GLuint m_VAO;
    Gluint m_posVBO;
    Gluint m_lifeVBO;
    Gluint m_shader;
    // Gluint m_indicesVBO; // quad
    // GLuint m_transformationsVBO; // quad
    // GLuint m_rotationsVBO; // quad
    // GLuint m_scalesVBO; // quad

    cl_mem              m_clpositionBuffer, m_clvelocityBuffer, m_clrandVelOffsetBuffer, m_cllifeBuffer;
    cl_command_queue    m_clcommandQueue;   // CL command queue for sending instructions to the GPU
    cl_program          m_clprogram;        // CL program we run
    cl_kernel           m_clkernel;
    cl_platform_id      *m_clplatformIDs;
    cl_device_id        *m_cldeviceIDs;
    cl_uint             m_cldeviceIDCount, m_clplatformIDCount;
    cl_context          m_clcontext;

    void configureVAO();
    void configureShader();
    void bindAndUpdateBuffers();

    void initializeCL();
    void setupCL();
    void setupKernel();
    void setupBuffer();
};


/* Helper function for reading OpenCL kernel source code -> array source string */
void loadKernelSource(const char *path, char* source_str, size_t* source_size){
    fp = fopen(path, "r");
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
}


/* Helper function for OpenCL error checking */
void checkError(const std::string &source, cl_int error){
    if (error != CL_SUCCESS){
        std::cerr << description << std::endl;
        std::cerr << "OpenCL call failed with error: " << getErrorString(error) << std::endl;
        exit(1);
    }
}


/* Helper function for OpenCL error checking */
std::string getErrorString(cl_int error)
{
    switch(error)
    {
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

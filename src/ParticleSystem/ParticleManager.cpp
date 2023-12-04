#include "ParticleManager.h"
#include "utils/shaderloader.h"
#include <iostream>
#include <algorithm>

#define MAX_SOURCE_SIZE (0x100000)


ParticleManager::ParticleManager(int number) {
    m_emit_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_num_of_particles = number;

    m_particle_position.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_velocity.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_life.resize(m_num_of_particles, -1.0f);
    m_particle_randVelOffset(m_num_of_particles);
    for (int i = 0; i < m_num_of_particles; i++) {
        m_particle_randVelOffset[i].x = RAND;
        m_particle_randVelOffset[i].y = RAND;
        m_particle_randVelOffset[i].z = RAND;
    }

    configureVAO();
}


/* Change particle number, reset particles attributes
 * TODO: clean the screen
*/
void ParticleManager::changeParticleNumber(int new_number) {
    if(new_number == m_num_of_particles) return;
    m_num_of_particles = new_number;

    m_particle_position.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_velocity.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_life.resize(m_num_of_particles, -1.0f);
    m_particle_randVelOffset(m_num_of_particles);
    for (int i = 0; i < m_num_of_particles; i++) {
        m_particle_randVelOffset[i].x = RAND;
        m_particle_randVelOffset[i].y = RAND;
        m_particle_randVelOffset[i].z = RAND;
    }

    configureShader();
    configureVAO();
}


void ParticleManager::configureShader(){
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert", ":/resources/shaders/particle.frag");
}


/* Bind VAO to VBO
 * m_posVBO -> pariticle's vertex
 * m_lifeVBO -> particle's color
*/
void ParticleManager::configureVAO(){
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_lifeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


/* Bind VAO, update VBO
*/
void ParticleManager::bindAndUpdateBuffers(){
    // Bind the VAO
    glBindVertexArray(m_VAO);

    // Send (updated) pos data to the GPU, pos => vertex
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles*sizeof(glm::vec3), &m_particle_position[0]);

    // Send (updated) life data to the GPU, life => color
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles*sizeof(float), &m_particle_life[0]);
}


/* Initialize all CL members:
    cl_uint m_cldeviceIDCount, m_clplatformIDCount;
    cl_platform_id *m_clplatformIDs;
    cl_device_id *m_cldeviceIDs;
    cl_context m_clcontext;
    cl_command_queue m_clcommandQueue;
    cl_program m_clprogram;
    cl_kernel m_clkernel;
    cl_mem m_clpositionBuffer, m_clvelocityBuffer, m_clrandVelOffsetBuffer, m_cllifeBuffer;
 */
void ParticleManager::initializeCL(){
    setupCL();
    setupKernel();
    setupBuffer();
}


void ParticleManager::create(int id){
    m_particle_life[id] = 1.0f;
    m_particle_position[id] = m_emit_pos;
    m_particle_velocity[id] = glm::vec3(0.0f,10.0f,0.0f) + m_particle_randVelOffset[id];
}


void ParticleManager::render(const glm::mat4 &ViewProjection){
    if (m_num_of_particles == 0) return;

    // bind VAO and update local vector data to VBO
    glUseProgram(m_shader);
    bindAndUpdateBuffers();

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "u_ViewProjection"), 1, GL_FALSE, &ViewProjection[0][0]);
    glDrawArrays(GL_POINTS, 0, m_num_of_particles);

    // Unbind VAO
    glBindVertexArray(0);
    glUseProgram(0);
}


void ParticleManager::update(float dt) {

    // slowly increase the number of its particles to the max amount instead of shooting all at once
    if (m_active_particles < m_num_of_particles) {
        int batch = 14;
        int limit = std::min(m_num_of_particles - m_active_particles, batch);
        for (int i = 0; i < limit; i++) {
            create(i);
            m_active_particles++;
        }
    }
    else {
        m_active_particles = m_num_of_particles; // in case resized to a smaller particle number
    }

    // update by OpenCL kernel function
    cl_int error;

    // define the arguments to our kernel
    clSetKernelArg(m_clkernel, 0, sizeof(cl_mem), &m_clpositionBuffer);
    clSetKernelArg(m_clkernel, 1, sizeof(cl_mem), &m_clvelocityBuffer);
    clSetKernelArg(m_clkernel, 2, sizeof(cl_mem), &m_clrandVelOffsetBuffer);
    clSetKernelArg(m_clkernel, 3, sizeof(cl_mem), &m_cllifeBuffer);
    clSetKernelArg(m_clkernel, 4, sizeof(float), &dt);

    size_t workSize[1] = { (size_t)(m_num_of_particles) };
    error = clEnqueueNDRangeKernel(m_clcommandQueue,					// our command queue
                                   m_clkernel,							// our kernel
                                   1,                                   // dimensions of work
                                   NULL,                                // global work offset (docs specify that this must currently always be NULL)
                                   workSize,                            // global work size (array dimensions should match 3rd param)
                                   NULL,                                // local work size: NULL means OpenCL figures this out
                                   0,                                   // no events in wait list
                                   NULL,                                // no event list to wait for before executing this command
                                   NULL                                 // no event is associated with this command
                                   );
    checkError("clEnqueueNDRangeKernel()", error);

    // read back from cl buffers to local vector data
    error = clEnqueueReadBuffer(m_clcommandQueue,
                                m_clparticleBuffer,                     // CL buffer where we're getting the data from
                                CL_TRUE,
                                0,
                                m_num_of_particles*sizeof(glm::vec3),   // how much to read, in bytes
                                m_particle_position.data(),                    // where to read to
                                0,
                                NULL,
                                NULL
                                );
    checkError("clEnqueueReadBuffer()", error);
    error = clEnqueueReadBuffer(m_clcommandQueue,
                                m_clvelocityBuffer,                     // CL buffer where we're getting the data from
                                CL_TRUE,
                                0,
                                m_num_of_particles*sizeof(glm::vec3),   // how much to read, in bytes
                                m_particle_velocity.data(),                    // where to read to
                                0,
                                NULL,
                                NULL
                                );
    checkError("clEnqueueReadBuffer()", error);
    error = clEnqueueReadBuffer(m_clcommandQueue,
                                m_cllifeBuffer,                     // CL buffer where we're getting the data from
                                CL_TRUE,
                                0,
                                m_num_of_particles*sizeof(float),   // how much to read, in bytes
                                m_particle_life.data(),                    // where to read to
                                0,
                                NULL,
                                NULL
                                );
    checkError("clEnqueueReadBuffer()", error);

    clFinish(commandQueue);
}


void ParticleManager::setupCL(){
    cl_int error;

    // determine how many OpenCL platforms we have
    error = clGetPlatformIDs(0, NULL, &m_clplatformIDCount);
    checkError("clGetPlatformIDs()", error);
    if(m_clplatformIDCount == 0){
        std::cout << "No platforms found" << std::endl;
        exit(1);
    }
    else{
        std::cout << "-- Found " << m_clplatformIDCount << " OpenCL platform(s)" << std::endl;
    }

    // get the IDs of those platforms
    m_clplatformIDs = new cl_platform_id[m_clplatformIDCount];
    error = clGetPlatformIDs(m_clplatformIDCount, m_clplatformIDs, NULL);
    checkError("clGetPlatformIDs()", error);

    // determine how many OpenCL devices we have
    error = clGetDeviceIDs(m_clplatformIDs[0], CL_DEVICE_TYPE_GPU, 0, NULL, &m_cldeviceIDCount);
    checkError("clGetDeviceIDs()", error);
    if(m_cldeviceIDCount == 0){
        std::cout << "No devices found" << std::endl;
        exit(1);
    }
    else{
        std::cout << "-- Found " << m_cldeviceIDCount << " OpenCL device(s)" << std::endl;
    }

    // get the IDs of those devices
    m_cldeviceIDs = new cl_device_id[m_cldeviceIDCount];
    error = clGetDeviceIDs(m_clplatformIDs[0], CL_DEVICE_TYPE_GPU, m_cldeviceIDCount, m_cldeviceIDs, NULL);
    checkError("clGetDeviceIDs()", error);

    // create an OpenCL context
    m_clcontext = clCreateContext(NULL, m_cldeviceIDCount, m_cldeviceIDs, NULL, NULL, &error);
    checkError("clCreateContext()" , error);

    // set command queue
    m_clcommandQueue = clCreateCommandQueue(m_clcontext, m_cldeviceIDs[0], 0, &error); // scale up to multiple device ids?
    checkError("clCreateCommandQueue()", error);
}


void ParticleManager::setupKernel(){
    // create kernel
    char *build_log;						// error log if compilation failed
    size_t ret_val_size;					// length of build log in bytes
    cl_build_status build_status;			// build status of our attempted program compilation
    cl_int error;

    // load up our program source code and attempt to compile it
    std::filesystem::path relativePath = "src/ParticleSystem/particle.cl";
    std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);
    size_t source_size;
    char* source_str;
    loadKernelSource(absolutePath.c_str(), source_str, source_size);
    m_clprogram = clCreateProgramWithSource(m_clcontext, 1, (const char **)&source_str, (const size_t *)&source_size, &error);
    checkError("clCreateProgramWithSource()", error);

    // did it compile successfully?
    clBuildProgram(m_clprogram, m_cldeviceIDCount, m_cldeviceIDs, NULL, NULL, NULL);
    error = clGetProgramBuildInfo(m_clprogram, m_cldeviceIDs[0], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
    checkError("clGetProgramBuildInfo()", error);

    // error checking of kernel program
    if (build_status != CL_SUCCESS)
    {
        // how long the build log is, in bytes
        error = clGetProgramBuildInfo(program, deviceIDs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
        checkError("clGetProgramBuildInfo()", error);

        // now that we have the length, get the actual build log
        build_log = new char[ret_val_size+1];
        error = clGetProgramBuildInfo(program, deviceIDs[0], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
        checkError("clGetProgramBuildInfo()", error);

        // to be careful, terminate with NULL char---there's no information in the reference whether the string is 0 terminated or not
        build_log[ret_val_size] = '\0';
        std::cout << "BUILD LOG: '" << sourceFile << "'" << std::endl;
        cout << build_log << endl;
        exit(1);
    }

    // everything was successful at this point so far, so create our kernel using the compiled program
    m_clkernel = clCreateKernel(m_clprogram, "particle", &error);
    checkError("clCreateKernel()", error);
}


void ParticleManager::setupBuffer(){
    // Create memory buffers on the device for each vector
    cl_int error;

    m_clpositionBuffer = clCreateBuffer(m_clcontext,
                                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                        m_num_of_particles*sizeof(glm::vec3),
                                        m_particle_position.data(),
                                        &error
                                        );
    checkError("clCreateBuffer(): m_clpositionBuffer", error);

    m_clvelocityBuffer = clCreateBuffer(m_clcontext,
                                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                        m_num_of_particles*sizeof(glm::vec3),
                                        m_particle_velocity.data(),
                                        &error
                                        );
    checkError("clCreateBuffer(): m_clvelocityBuffer", error);

    m_clrandVelOffsetBuffer = clCreateBuffer(m_clcontext,
                                             CL_MEM_READ | CL_MEM_COPY_HOST_PTR,
                                             m_num_of_particles*sizeof(glm::vec3),
                                             m_particle_randVelOffset.data(),
                                             &error
                                             );
    checkError("clCreateBuffer(): m_clrandVelOffsetBuffer", error);

    m_cllifeBuffer = clCreateBuffer(m_clcontext,
                                    CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                    m_num_of_particles*sizeof(float),
                                    m_particle_life.data(),
                                    &error
                                    );
    checkError("clCreateBuffer(): m_cllifeBuffer", error);
}


ParticleManager::~ParticleManager() {
    // Release OpenCL resources
    if (m_clcommandQueue) {
        clReleaseCommandQueue(m_clcommandQueue);
        m_clcommandQueue = nullptr;
    }

    if (m_clkernel) {
        clReleaseKernel(m_clkernel);
        m_clkernel = nullptr;
    }

    if (m_clprogram) {
        clReleaseProgram(m_clprogram);
        m_clprogram = nullptr;
    }

    if (m_clpositionBuffer) {
        clReleaseMemObject(m_clpositionBuffer);
        m_clpositionBuffer = nullptr;
    }

    if (m_clvelocityBuffer) {
        clReleaseMemObject(m_clvelocityBuffer);
        m_clvelocityBuffer = nullptr;
    }

    if (m_clrandVelOffsetBuffer) {
        clReleaseMemObject(m_clrandVelOffsetBuffer);
        m_clrandVelOffsetBuffer = nullptr;
    }

    if (m_cllifeBuffer) {
        clReleaseMemObject(m_cllifeBuffer);
        m_cllifeBuffer = nullptr;
    }

    if (m_clcontext) {
        clReleaseContext(m_clcontext);
        m_clcontext = nullptr;
    }

    if (m_clplatformIDs) {
        delete[] m_clplatformIDs;
        m_clplatformIDs = nullptr;
    }

    if (m_cldeviceIDs) {
        delete[] m_cldeviceIDs;
        m_cldeviceIDs = nullptr;
    }

    // Release OpenGL resources
    glDeleteBuffers(1, &m_posVBO);
    glDeleteBuffers(1, &m_lifeVBO);
    glDeleteVertexArrays(1, &m_VAO);
}

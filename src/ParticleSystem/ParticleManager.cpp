#include "ParticleManager.h"
#include <iostream>
#include <algorithm>

#define MAX_SOURCE_SIZE (0x100000)

// Load the kernel source code into the array source_str
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


ParticleManager::ParticleManager(int number) {
    m_emit_pos = glm::vec3(0.0f, 0.0f, 0.0f);

    m_num_of_particles = number;
    m_particle_attributes.resize(m_num_of_particles, ParticleAttributes());
    m_particle_vertices.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_life.resize(m_num_of_particles, 0.0f);

    configureVAO();
}


void ParticleManager::changeParticleNumber(int new_number) {
    if(new_number == m_num_of_particles) return;

    m_num_of_particles = new_number;
    m_particle_attributes.resize(m_num_of_particles, ParticleAttributes());
    m_particle_vertices.resize(m_num_of_particles, glm::vec3(0.0f));
    m_particle_life.resize(m_num_of_particles, 0.0f);
}


void ParticleManager::configureVAO()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_lifeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void ParticleManager::bindAndUpdateBuffers(){
    if (m_use_sorting) {
        std::sort(m_particle_attributes.begin(), m_particle_attributes.end());
        std::reverse(m_particle_attributes.begin(), m_particle_attributes.end());
    }

    // Bind the VAO
    glBindVertexArray(m_VAO);

    // Send (updated) vertex data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles*sizeof(glm::mat4), &m_particle_vertices[0]);

    // Send (updated) life data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles*sizeof(float), &m_particle_life[0]);
}


void ParticleManager::initializeCL(){
    /*
    cl_mem m_clparticleBuffer;
    cl_command_queue m_clcommandQueue; // CL command queue for sending instructions to the GPU
    cl_program m_clprogram; // CL program we run
    cl_kernel m_clkernel;

    cl_platform_id *m_clplatformIDs;
    cl_device_id *m_cldeviceIDs;
    cl_uint m_cldeviceIDCount, m_clplatformIDCount;
    cl_context m_clcontext;
    */
    setupCL();
    setupKernel();
    setupBuffer();
}


void ParticleManager::create(int id){
    ParticleAttributes& particle = m_particle_attributes[id];

    particle.position = m_emit_pos + glm::vec3(1 - RAND*2, RAND, 1 - RAND*2);
    particle.velocity = glm::vec3( 3 - RAND*6,10,3 - RAND*6)*4.0f;
    particle.accel = glm::vec3(0.0f, -9.8f, 0.0f);
    particle.rot_axis = glm::vec3(0.0f); // quad
    particle.rot_angle = RAND*360; // quad
    particle.life = 100.0f;
    particle.mass = RAND + 0.5f; // quad
}


void ParticleManager::render(const glm::mat4 &ViewProjection){
    if (m_num_of_particles == 0) return;
    bindAndUpdateBuffers();

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ViewProjection"), 1, GL_FALSE, &ViewProjection[0][0]);
    glDrawArrays(GL_POINTS, 0, m_num_of_particles);

    // Unbind VAO
    glBindVertexArray(0);
}


void ParticleManager::update(float time, float dt) {

    // slowly increase the number of its particles to the max amount instead of shooting all at once
    if (m_active_particles < m_num_of_particles) {
        int batch = 14;
        int limit = std::min(m_num_of_particles - m_active_particles, batch);
        for (int i = 0; i < limit; i++) {
            createNewParticle(m_active_particles);
            m_active_particles++;
        }
    }
    else {
        m_active_particles = m_num_of_particles; // in case resized to a smaller particle number
    }


//    for(int i = 0; i < active_particles; i++){
//        particleAttributes & particle = p_attributes[i];
//        if(particle.life == 0.0f){
//            createNewParticle(i);
//        }
//        particle.position = particle.position + particle.velocity*dt + particle.accel*(dt*dt)*0.5f;
//        particle.velocity = particle.velocity + particle.accel*dt;
//    }


    // update the above using openGL logic
    cl_int error;

    // define the arguments to our kernel
    // clSetKernelArg(kernel, 0, sizeof(cl_mem), &particleBuffer);
    // clSetKernelArg(kernel, 1, sizeof(int), &numPoints);
    // clSetKernelArg(kernel, 4, sizeof(int), &numDivisions);
    // clSetKernelArg(kernel, 2, sizeof(int), &start);
    // clSetKernelArg(kernel, 3, sizeof(int), &end);

    size_t workSize[1] = {(size_t)(m_num_of_particles)};
    error = clEnqueueNDRangeKernel(m_clcommandQueue,					// our command queue
                                   m_clkernel,							// our kernel
                                   1,                                   // dimensions of work
                                   NULL,                                // global work offset (docs specify that this must currently always be NULL)
                                   workSize,                            // global work size (array dimensions should match 3rd param)
                                   NULL,                                // local work size: NULL means OpenCL figures this out
                                   0,                                   // no events in wait list
                                   NULL,                                // no event list to wait for before executing this command
                                   NULL);                               // no event is associated with this command
    checkError("clEnqueueNDRangeKernel()", error);

    error = clEnqueueReadBuffer(m_clcommandQueue,
                                m_clparticleBuffer,                     // CL buffer where we're getting the data from
                                CL_TRUE,
                                0,
                                m_num_of_particles*sizeof(glm::vec3),   // how much to read, in bytes
                                m_particle_vertices,                    // where to read to
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
    kernel = clCreateKernel(program, kernelName.c_str(), &error);
    checkError("clCreateKernel()", error);
}


void ParticleManager::setupBuffer(){
    // Create memory buffers on the device for each vector
    cl_int error;
    m_clparticleBuffer = clCreateBuffer(m_clcontext,
                                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                        m_num_of_particles*sizeof(glm::vec3),
                                        m_particle_vertices,
                                        &error
                                        );
    checkError("clCreateBuffer(): m_clparticleBuffer", error);
}

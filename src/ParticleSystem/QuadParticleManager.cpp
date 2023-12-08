#include "QuadParticleManager.h"
#include <cstdlib>
#include <fstream>

double generateGaussianNoise() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(0.0, 1.0);
    return distribution(gen);
}

QuadParticleManager::QuadParticleManager(int number): m_num_of_particles(number) {
    if (number > 0){
        m_particle_position.resize(m_num_of_particles, glm::vec4(0.0f));// modified to draw triangles
        m_particle_velocity.resize(m_num_of_particles, glm::vec4(0.0f));
        m_particle_life.resize(m_num_of_particles, -1.0f);
        m_particle_randVelOffset.resize(m_num_of_particles, glm::vec4(0.0f));
    }
    m_emit_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

/* Change particle number, reset particles attributes
*/
void QuadParticleManager::changeNumParticles(int new_number) {
    if(!m_init){
        configureShaderProgram();
        m_init = true;
    }

    if(new_number == m_num_of_particles) return;

    m_num_of_particles = new_number;

    m_particle_position.resize(m_num_of_particles, glm::vec4(0.0f));// modified to draw triangles
    m_particle_velocity.resize(m_num_of_particles, glm::vec4(0.0f));
    m_particle_life.resize(m_num_of_particles, -1.0f);
    m_particle_randVelOffset.resize(m_num_of_particles, glm::vec4(0.0f));

    configureVAO();
    configureTexture();//
    initializeCL();
}

void QuadParticleManager::configureShaderProgram(){
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert", ":/resources/shaders/particle.frag");
}

/* Bind VAO to VBO
 * m_posVBO -> pariticle's vertex
 * m_lifeVBO -> particle's color
*/
void QuadParticleManager::configureVAO(){
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles *sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);

    glGenBuffers(1, &m_lifeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles* sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); //??

//    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    glEnableVertexAttribArray(0);


//    glBindBuffer(GL_ARRAY_BUFFER, instanceSizeVBO);
//    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    glEnableVertexAttribArray(2);
//    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);
}


/* Bind VAO, update VBO
*/
void QuadParticleManager::bindAndUpdateBuffers(){
    // Bind the VAO
    glBindVertexArray(m_VAO);

    // Send (updated) pos data to the GPU, pos => vertex
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles * sizeof(glm::vec4), &m_particle_position[0]);

    // Send (updated) life data to the GPU, life => color
    glBindBuffer(GL_ARRAY_BUFFER, m_lifeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_num_of_particles*sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_of_particles*sizeof(float), &m_particle_life[0]);
}

void QuadParticleManager::configureTexture() {
    //QString kitten_filepath = QString("../lab11-textures-FBOs-Erikwang317/resources/images/kitten.png");
    QString kitten_filepath = QString(":/resources/images/snowflakes.png");
    if (!m_image.load(kitten_filepath)) {
        std::cerr << "Failed to load texture from: " << kitten_filepath.toStdString() << std::endl;
        return;
    }

    m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());

    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    int width, height, nrChannels;
//    unsigned char *data = stbi_load("path/to/texture.jpg", &width, &height, &nrChannels, 0);
//    if (data) {
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    } else {
//        std::cout << "Failed to load texture" << std::endl;
//    }
//    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void QuadParticleManager::initializeCL(){
    setupCL();
    setupKernel();
}

void QuadParticleManager::create(int id){

    // Define the two triangles in CCW order
    glm::vec3 center = glm::vec3(m_emit_pos);

    m_particle_position[id] = glm::vec4(center, 1.f);

    m_particle_randVelOffset[id] = glm::vec4(0.5 * generateGaussianNoise(), // RAND
                                             0.5 * generateGaussianNoise(), // RAND
                                             0.5 * generateGaussianNoise(), // RAND
                                             0.0f);
    m_particle_life[id] = m_max_life;
    m_particle_velocity[id] = glm::vec4(0.0f, 7.0f, 0.0f, 0.0f) + m_particle_randVelOffset[id];
}


void QuadParticleManager::render(const glm::mat4 &ViewProjection, const glm::vec3 &cameraRight){
    if (m_num_of_particles == 0) return;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(glGetUniformLocation(m_shader, "u_texture"), 0);

    // bind VAO and update local vector data to VBO
    glUseProgram(m_shader);
    bindAndUpdateBuffers();

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "u_ViewProjection"), 1, GL_FALSE, &ViewProjection[0][0]);

    //glm::vec3 worldUp(0.f, 1.f, 0.f);

    glUniform3fv(glGetUniformLocation(m_shader, "u_cameraRight"), 1, &cameraRight[0]);

    glUniform4fv(glGetUniformLocation(m_shader, "u_bornColor"), 1, &m_bornColor[0]);
    glUniform4fv(glGetUniformLocation(m_shader, "u_deadColor"), 1, &m_deadColor[0]);
    glUniform1f(glGetUniformLocation(m_shader, "u_max_life"), m_max_life);
    //glDrawArrays(GL_POINTS, 0, m_num_of_particles);

    //glDrawArrays(GL_TRIANGLES, 0, m_num_of_particles); //draw triagnles

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, m_num_of_particles);

    // Unbind VAO
    glBindVertexArray(0);
    glUseProgram(0);
}


void QuadParticleManager::update(float dt) {
    // slowly increase the number of its particles to the max amount instead of shooting all at once
    if (m_active_particles < m_num_of_particles) {
        int batch = 10;
        int limit = std::min(m_num_of_particles - m_active_particles, batch);

        for (int i = 0; i < limit; i++) {
            create(m_active_particles);
            m_active_particles++;
        }
    }
    else {
        m_active_particles = m_num_of_particles; // in case resized to a smaller particle number
    }
    setupBuffer();

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
                                m_clpositionBuffer,                     // CL buffer where we're getting the data from
                                CL_TRUE,
                                0,
                                m_num_of_particles*sizeof(glm::vec4),   // how much to read, in bytes
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
                                m_num_of_particles*sizeof(glm::vec4),   // how much to read, in bytes
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
    clFinish(m_clcommandQueue);
}


void QuadParticleManager::setupCL(){
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
    m_cldeviceIDs = new cl_device_id[1];
    error = clGetDeviceIDs(m_clplatformIDs[0], CL_DEVICE_TYPE_GPU, m_cldeviceIDCount, m_cldeviceIDs, NULL);
    checkError("clGetDeviceIDs()", error);

    // create an OpenCL context
    m_clcontext = clCreateContext(NULL, 1, m_cldeviceIDs, NULL, NULL, &error);
    checkError("clCreateContext()" , error);

    // set command queue
    m_clcommandQueue = clCreateCommandQueue(m_clcontext, m_cldeviceIDs[0], 0, &error); // scale up to multiple device ids?
    checkError("clCreateCommandQueue()", error);
}


void QuadParticleManager::setupKernel(){
    setenv("CL_LOG_ERRORS", "stdout", 1);
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
//    std::cout << "absolutePath==============" << absolutePath << std::endl;
//    std::cout << "RelativePath==============" << relativePath << std::endl;

    FILE *fp;
    fp = fopen(absolutePath.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "Fail load kernel.\n");
        exit(1);
    }
    else{
        fprintf(stdout, "Success load kernel.\n");
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
    std::cout << source_str << std::endl;

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
        error = clGetProgramBuildInfo(m_clprogram, m_cldeviceIDs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
        checkError("clGetProgramBuildInfo()", error);

        // now that we have the length, get the actual build log
        build_log = new char[ret_val_size+1];
        error = clGetProgramBuildInfo(m_clprogram, m_cldeviceIDs[0], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
        checkError("clGetProgramBuildInfo()", error);

        // to be careful, terminate with NULL char---there's no information in the reference whether the string is 0 terminated or not
        build_log[ret_val_size] = '\0';
        std::cout << "BUILD LOG: '" << absolutePath.c_str() << "'" << std::endl;
        std::cout << build_log << std::endl;
        exit(1);
    }

    // everything was successful at this point so far, so create our kernel using the compiled program
    m_clkernel = clCreateKernel(m_clprogram, "particle", &error);
    checkError("clCreateKernel()", error);
}


void QuadParticleManager::setupBuffer(){
    // Create memory buffers on the device for each vector
    cl_int error;

    m_clpositionBuffer = clCreateBuffer(m_clcontext, CL_MEM_READ_WRITE, m_num_of_particles*sizeof(glm::vec4), NULL, &error);
    checkError("clCreateBuffer(): m_clpositionBuffer", error);
    error = clEnqueueWriteBuffer(m_clcommandQueue, m_clpositionBuffer, CL_TRUE, 0, m_num_of_particles*sizeof(glm::vec4),
                                 m_particle_position.data(), 0, NULL, NULL);
    checkError("clEnqueueWriteBuffer(): m_clpositionBuffer", error);

    m_clvelocityBuffer = clCreateBuffer(m_clcontext, CL_MEM_READ_WRITE, m_num_of_particles*sizeof(glm::vec4), NULL, &error);
    checkError("clCreateBuffer(): m_clvelocityBuffer", error);
    error = clEnqueueWriteBuffer(m_clcommandQueue, m_clvelocityBuffer, CL_TRUE, 0, m_num_of_particles*sizeof(glm::vec4),
                                 m_particle_velocity.data(), 0, NULL, NULL);
    checkError("clEnqueueWriteBuffer(): m_clvelocityBuffer", error);

    m_clrandVelOffsetBuffer = clCreateBuffer(m_clcontext, CL_MEM_READ_ONLY, m_num_of_particles*sizeof(glm::vec4), NULL, &error);
    checkError("clCreateBuffer(): m_clrandVelOffsetBuffer", error);
    error = clEnqueueWriteBuffer(m_clcommandQueue, m_clrandVelOffsetBuffer, CL_TRUE, 0, m_num_of_particles*sizeof(glm::vec4),
                                 m_particle_randVelOffset.data(), 0, NULL, NULL);
    checkError("clEnqueueWriteBuffer(): m_clrandVelOffsetBuffer", error);

    m_cllifeBuffer = clCreateBuffer(m_clcontext, CL_MEM_READ_WRITE, m_num_of_particles*sizeof(float), NULL, &error);
    checkError("clCreateBuffer(): m_cllifeBuffer", error);
    error = clEnqueueWriteBuffer(m_clcommandQueue, m_cllifeBuffer, CL_TRUE, 0, m_num_of_particles*sizeof(float),
                                 m_particle_life.data(), 0, NULL, NULL);
    checkError("clEnqueueWriteBuffer(): m_cllifeBuffer", error);
}


QuadParticleManager::~QuadParticleManager() {
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


/* Helper function for reading OpenCL kernel source code -> array source string */
void loadKernelSource(const char *path, char* source_str, size_t* source_size){
    FILE *fp;
    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Fail load kernel.\n");
        exit(1);
    }
    else{
        fprintf(stdout, "Success load kernel.\n");
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    *source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
}


/* Helper function for OpenCL error checking */
void checkError(const std::string &description, cl_int error){
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


#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <OpenCL/opencl.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include "utils/shaderloader.h"
#include <QImage>
#include <QOpenGLWidget>
#include "settings.h"

//#include "utils/shaderloader.h"

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define MAX_SOURCE_SIZE (0x100000)
#define RAND ((float) arc4random()) / (float) UINT32_MAX - 0.5f

//// standard normal distribution


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

//struct Particle{
//    glm::vec3 pos, speed;
//    unsigned char r,g,b,a;
//    float size, angle, weight;
//    float life;
//    float cameradistance;
//    bool operator<(Particle& that){
//        return this->cameradistance > that.cameradistance;
//    }

//};


/* Basic fountain particle effect
 * Emits from origin, shoots up with random initial velocity, drop off the screen, then respawn
 * This is a point particle system. Implement quad particle system later
*/
class QuadParticleManager: public QOpenGLWidget {

public:
    int m_active_particles = 0;
    int m_num_of_particles;
    float m_max_life = 1.2f;
    std::string textureFilePath;
    bool m_texture_initalized = false;
//    int MaxParticles = 100000;
//    Particle ParticlesContainer[MaxParticles];

    /* Particle attributes, maintained via OpenCL */
    std::vector<float> m_particle_life;

    std::vector<glm::vec4> m_particle_position;
    std::vector<glm::vec4> m_particle_velocity;
    std::vector<glm::vec4> m_particle_randVelOffset;
    // std::vector<int> m_particle_indices; // quad
    // std::vector<glm::mat4> m_translations; // quad
    // std::vector<glm::mat4> m_rotations; // quad
    // std::vector<float> m_scales; // quad

    /* ParticleManager member functions for init + render + update */
    QuadParticleManager(int number=0);
    ~QuadParticleManager();
    void changeNumParticles(int new_number);
    void render(const glm::mat4 &ViewProjection, const glm::vec3 &right, float aspectRatio);
    void updateParticles(float dt);
    void create(int id);
    void updateTexture();
    // glm::vec4 calculateBillboardRotationMatrix(glm::vec3 particle_pos, glm::vec3 camera_pos); // quad

private:
    bool m_init = false;
    glm::vec4 m_emit_pos;
    glm::vec4 m_bornColor = glm::vec4(0.0f, 0.1f, 0.1f, 1.0f);
    glm::vec4 m_deadColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint m_VAO;
    GLuint m_posVBO;
    GLuint m_lifeVBO;
    GLuint m_shader;
    GLuint m_texture;
    QImage m_image;
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
    void configureShaderProgram();
    void bindAndUpdateBuffers();
    void configureTexture();
    void enableGLBlend();

    void initializeCL();
    void setupCL();
    void setupKernel();
    void setupBuffer();
};


/* Helper function for reading OpenCL kernel source code -> array source string */
void loadKernelSource(const char *path, char* source_str, size_t* source_size);


/* Helper function for OpenCL error checking */
void checkError(const std::string &description, cl_int error);


/* Helper function for OpenCL error checking */
std::string getErrorString(cl_int error);

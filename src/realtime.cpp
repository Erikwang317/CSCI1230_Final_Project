#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "ParticleSystem/IntParticleEmitter.h"


// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteBuffers(1,&m_cube_vbo);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1,&m_cylinder_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vao);
    glDeleteBuffers(1,&m_cone_vbo);
    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteBuffers(1,&m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);

    this->doneCurrent();
}

void Realtime::initializeGL() {

    m_blurRadius = 2;
    m_defaultFBO = 2;
    m_devicePixelRatio = this->devicePixelRatio();
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    m_texelSize = glm::vec2(1.0f/m_screen_width, 1.0f/m_screen_height);

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // TODO: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    Sphere sphere;
    sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_sphere_data_size = sphere.getDataSize();
    setVboAndBindVao(m_sphere_vao, m_sphere_vbo, sphere.generateShape());

    Cube cube;
    cube.updateParams(settings.shapeParameter1);
    m_cube_data_size = cube.getDataSize();
    setVboAndBindVao(m_cube_vao, m_cube_vbo, cube.generateShape());

    Cylinder cylinder;
    cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cylinder_data_size = cylinder.getDataSize();
    setVboAndBindVao(m_cylinder_vao, m_cylinder_vbo, cylinder.generateShape());

    Cone cone;
    cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cone_data_size = cone.getDataSize();
    setVboAndBindVao(m_cone_vao, m_cone_vbo, cone.generateShape());

    m_is_init = true;


//    /* init m_texture_shader + m_fullscreen_vbo + m_fullscreen_vbo + m_fbo */
//    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

//    // bind uniform sampler2D -> 0
//    glUseProgram(m_texture_shader);
//    glUniform1i(glGetUniformLocation(m_texture_shader, "s2D"), 0);
//    glUseProgram(0);

//    // vector data
//    std::vector<GLfloat> fullscreen_quad_data =
//        { //     POSITIONS + UV   //
//            -1.f,  1.f, 0.0f,
//            0.f,  1.f,
//            -1.f, -1.f, 0.0f,
//            0.f,  0.f,
//            1.f, -1.f, 0.0f,
//            1.f,  0.f,
//            1.f,  1.f, 0.0f,
//            1.f,  1.f,
//            -1.f,  1.f, 0.0f,
//            0.f,  1.f,
//            1.f, -1.f, 0.0f,
//            1.f,  0.f
//        };

//    // m_fullscreen_vbo + m_fullscreen_vao
//    glGenBuffers(1, &m_fullscreen_vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
//    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
//    glGenVertexArrays(1, &m_fullscreen_vao);
//    glBindVertexArray(m_fullscreen_vao);

//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

//    // m_fbo
//    makeFBO();
}


void Realtime::paintGL() {
    // TODO: anything requiring OpenGL calls every frame should be done here

//    // bind m_fbo, and draw everything there
//    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//    glViewport(0, 0, m_fbo_width, m_fbo_height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    paintExampleGeometry();

//    // bind the default framebuffer, draw to the default screen (defaultFBO) m_fbo_texture as a texture
//    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
//    glViewport(0, 0, m_screen_width, m_screen_height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // (fbo's color attachment texture)
//    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter);

    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintExampleGeometry();
}

void Realtime::resizeGL(int w, int h) {
    std::cout << "resizeGL(" << w << "," << h << ")" << std::endl;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
}

void Realtime::sceneChanged() {
    // set m_renderData
    bool success = SceneParser::parse(settings.sceneFilePath, m_renderData);
    if (!success){
        std::cout << "Error: parse JSON fail" << std::endl;
        return;
    }
    else{
        std::cout << "DEBUG: Load scnene file success" << std::endl;
    }
    const auto &cameraData = m_renderData.cameraData;
    m_camera = Camera(cameraData.pos, cameraData.look, cameraData.up, (float)size().width()/size().height(),
                      cameraData.heightAngle, settings.farPlane, settings.nearPlane
                      );
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    if (m_is_init){
        Sphere sphere;
        sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        m_sphere_data_size = sphere.getDataSize();
        setVboAndBindVao(m_sphere_vao, m_sphere_vbo, sphere.generateShape());

        Cube cube;
        cube.updateParams(settings.shapeParameter1);
        m_cube_data_size = cube.getDataSize();
        setVboAndBindVao(m_cube_vao, m_cube_vbo, cube.generateShape());

        Cylinder cylinder;
        cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        m_cylinder_data_size = cylinder.getDataSize();
        setVboAndBindVao(m_cylinder_vao, m_cylinder_vbo, cylinder.generateShape());

        Cone cone;
        cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        m_cone_data_size = cone.getDataSize();
        setVboAndBindVao(m_cone_vao, m_cone_vbo, cone.generateShape());

    }
    const auto &cameraData = m_renderData.cameraData;
    m_camera = Camera(cameraData.pos, cameraData.look, cameraData.up, (float)size().width()/size().height(),
                      cameraData.heightAngle, settings.farPlane, settings.nearPlane
                      );

    update(); // asks for a PaintGL() call to occur
}



void Realtime::setVboAndBindVao(GLuint& vao, GLuint& vbo, const std::vector<float>& vertex_data){
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size()*sizeof(GLfloat), vertex_data.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}


void Realtime::makeFBO(){
    // generate m_fbo_texture, bind, set minmax filter, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // generate m_fbo_renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // generate and bind an fbo, add our texture as a color attachment,
    // and our renderbuffer as a depth+stencil attachment to our fbo,
    // then unbind to default fbo
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    //std::cout << "keyPressEvent() detected" << std::endl;
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    //std::cout << "keyReleaseEvent() detected" << std::endl;
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    //std::cout << "mousePressEvent() detected" << std::endl;
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    //std::cout << "mouseReleaseEvent() detected" << std::endl;
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera.moveCameraMouseX(deltaX*0.005f);
        m_camera.moveCameraMouseY(deltaY*0.005f);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W]){
        m_camera.moveCameraW(deltaTime);
    }
    if (m_keyMap[Qt::Key_S]){
        m_camera.moveCameraS(deltaTime);
    }
    if (m_keyMap[Qt::Key_A]){
        m_camera.moveCameraA(deltaTime);
    }
    if (m_keyMap[Qt::Key_D]){
        m_camera.moveCameraD(deltaTime);
    }
    if (m_keyMap[Qt::Key_Space]){
        m_camera.moveCameraSpace(deltaTime);
    }
    if (m_keyMap[Qt::Key_Control]){
        m_camera.moveCameraCtrl(deltaTime);
    }

    update(); // asks for a PaintGL() call to occur
}


void Realtime::paintExampleGeometry(){
    // set camera
    glUseProgram(m_shader);

    // (camera) view matrix + projection matrix
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ViewMatrix"), 1, GL_FALSE, &m_camera.getViewMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "ProjectionMatrix"), 1, GL_FALSE, &m_camera.getProjectionMatrix()[0][0]);
    glUniform4fv(glGetUniformLocation(m_shader, "campos"), 1, &m_camera.getPosWorldSpace()[0]);

    // (lights) ka ks kd num_lights
    glUniform1f(glGetUniformLocation(m_shader, "ka"), m_renderData.globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "kd"), m_renderData.globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "ks"), m_renderData.globalData.ks);
    glUniform1i(glGetUniformLocation(m_shader, "num_lights"), m_renderData.lights.size());
    /*
    vec4 color;
    vec4 pos; // Position with CTM applied (Not applicable to directional lights)
    vec4 dir; // Direction with CTM applied (Not applicable to point lights)
    vec3 function;
    int type;
    float penumbra; // Only applicable to spot lights, in RADIANS
    float angle;

    enum class LightType {
        LIGHT_POINT, -> 0
        LIGHT_DIRECTIONAL, -> 1
        LIGHT_SPOT, -> 2
    };
    */
    for (int i=0; i<m_renderData.lights.size(); i++){
        GLint loc_color = glGetUniformLocation(m_shader, ("lights_array["+std::to_string(i)+"].color").c_str());
        glUniform4fv(loc_color, 1, &m_renderData.lights[i].color[0]);
        GLint loc_pos = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].pos").c_str());
        glUniform4fv(loc_pos, 1, &m_renderData.lights[i].pos[0]);
        GLint loc_dir = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].dir").c_str());
        glUniform4fv(loc_dir, 1, &m_renderData.lights[i].dir[0]);
        GLint loc_func = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].function").c_str());
        glUniform3fv(loc_func, 1, &m_renderData.lights[i].function[0]);
        GLint loc_type = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].type").c_str());
        glUniform1i(loc_type, (int)m_renderData.lights[i].type);
        GLint loc_penumbra = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].penumbra").c_str());
        glUniform1f(loc_penumbra, m_renderData.lights[i].penumbra);
        GLint loc_angle = glGetUniformLocation(m_shader, ("lights_array[" + std::to_string(i) + "].angle").c_str());
        glUniform1f(loc_angle, m_renderData.lights[i].angle);
    }

    // (shapes)
    for (auto& shapedata: m_renderData.shapes){

        int num_triangles_to_draw = 0;
        switch(shapedata.primitive.type){
        case(PrimitiveType::PRIMITIVE_CONE):
            glBindVertexArray(m_cone_vao);
            num_triangles_to_draw = m_cone_data_size / 6;
            break;

        case(PrimitiveType::PRIMITIVE_CUBE):
            glBindVertexArray(m_cube_vao);
            num_triangles_to_draw = m_cube_data_size / 6;
            break;

        case(PrimitiveType::PRIMITIVE_CYLINDER):
            glBindVertexArray(m_cylinder_vao);
            num_triangles_to_draw = m_cylinder_data_size / 6;
            break;

        case(PrimitiveType::PRIMITIVE_SPHERE):
            glBindVertexArray(m_sphere_vao);
            num_triangles_to_draw = m_sphere_data_size / 6;
            break;
        default:
            break;
        }
        // (model matrix)
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "ModelMatrix"), 1, GL_FALSE, &shapedata.ctm[0][0]);

        // (material)
        /*
        struct SceneMaterial {
        SceneColor cAmbient;  // Ambient term
        SceneColor cDiffuse;  // Diffuse term
        SceneColor cSpecular; // Specular term
        float shininess;      // Specular exponent

        SceneColor cReflective; // Used to weight contribution of reflected ray lighting (via multiplication)
        */
        SceneMaterial material = shapedata.primitive.material;
        glUniform4fv(glGetUniformLocation(m_shader, "cAmbient"), 1, &material.cAmbient[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "cDiffuse"), 1, &material.cDiffuse[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "cSpecular"), 1, &material.cSpecular[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "cReflective"), 1, &material.cReflective[0]);
        glUniform1f(glGetUniformLocation(m_shader, "shininess"), material.shininess);


        // call draw
        glDrawArrays(GL_TRIANGLES, 0, num_triangles_to_draw);
        glBindVertexArray(0);

    }
    glUseProgram(0);
}


void Realtime::paintTexture(GLuint texture, bool isPerPixelFilter, bool isKernelBasedFilter){
    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isPerPixelFilter"), isPerPixelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isKernelBasedFilter"), isKernelBasedFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "blurRadius"), m_blurRadius);
    glUniform2fv(glGetUniformLocation(m_texture_shader, "texelSize"), 1, &m_texelSize[0]);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

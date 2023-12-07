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
    this->doneCurrent();
}

void Realtime::initializeGL() {

    m_devicePixelRatio = this->devicePixelRatio();
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;

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
    glViewport(0, 0, m_screen_width, m_screen_height);
}


void Realtime::paintGL() {
    // TODO: anything requiring OpenGL calls every frame should be done here

    glViewport(0, 0, size().width()*m_devicePixelRatio, size().height()*m_devicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 camViewProjection = m_camera.getProjectionMatrix()*m_camera.getViewMatrix();
    glm::vec3 right = glm::cross(m_camera.getLook(), m_camera.getUp());

    m_ParticleManager.render(camViewProjection, right);
}

void Realtime::resizeGL(int w, int h) {
    std::cout << "resizeGL(" << w << "," << h << ")" << std::endl;
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width()*m_devicePixelRatio, size().height()*m_devicePixelRatio);
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

    // Reset ParticleManager
    m_is_init = true;

    std::cout << "void Realtime::sceneChanged()" << std::endl;
    std::cout << "m_ParticleManager.changeNumParticles(m_num_of_particles);" << std::endl;
    m_ParticleManager.changeNumParticles(m_num_of_particles);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    update(); // asks for a PaintGL() call to occur
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
    float deltaTime = elapsedms * 0.1f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W]){
        m_camera.moveCameraW(deltaTime*0.005);
    }
    if (m_keyMap[Qt::Key_S]){
        m_camera.moveCameraS(deltaTime*0.005);
    }
    if (m_keyMap[Qt::Key_A]){
        m_camera.moveCameraA(deltaTime*0.005);
    }
    if (m_keyMap[Qt::Key_D]){
        m_camera.moveCameraD(deltaTime*0.005);
    }
    if (m_keyMap[Qt::Key_Space]){
        m_camera.moveCameraSpace(deltaTime*0.005);
    }
    if (m_keyMap[Qt::Key_Control]){
        m_camera.moveCameraCtrl(deltaTime*0.005);
    }

    if (m_is_init){
        m_ParticleManager.update(0.01*deltaTime);
    }

    update(); // asks for a PaintGL() call to occur
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

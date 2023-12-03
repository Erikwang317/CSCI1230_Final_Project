#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
#include "camera.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    glm::vec2 m_texelSize;
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not


    // Device Correction Variables
    int m_devicePixelRatio;


    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;
    int m_blurRadius;

    bool m_is_init = false;

    GLuint m_texture_shader;
    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint m_shader; // Stores id of shader program
    GLuint m_cube_vbo;
    GLuint m_cylinder_vbo;
    GLuint m_cone_vbo;
    GLuint m_sphere_vbo;
    GLuint m_cube_vao;
    GLuint m_cylinder_vao;
    GLuint m_cone_vao;
    GLuint m_sphere_vao;

    int m_cube_data_size;
    int m_cylinder_data_size;
    int m_cone_data_size;
    int m_sphere_data_size;

    RenderData m_renderData;
    Camera m_camera;

    void setVboAndBindVao(GLuint& vao, GLuint& vbo, const std::vector<float>& vertex_data);
    void makeFBO();
    void paintExampleGeometry();
    void paintTexture(GLuint texture, bool isPerPixelFilter, bool isKernelBasedFilter);

};

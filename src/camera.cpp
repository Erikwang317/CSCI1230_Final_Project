#include <stdexcept>
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(){}

Camera::Camera(glm::vec4 pos, glm::vec4 look, glm::vec4 up, float aspect_ratio,
               float height_angle, float far, float near) :
    m_pos_world_space(pos), m_look(look), m_up(up), m_aspect_ratio(aspect_ratio),
    m_height_angle(height_angle), m_far(far), m_near(near)
{
    _setViewMatrix();
    _setProjectionMatrix();
}


Camera& Camera::operator=(const Camera& other) {
    if (this != &other) {  // Avoid self-assignment
        m_pos_world_space = other.m_pos_world_space;
        m_look = other.m_look;
        m_up = other.m_up;
        m_near = other.m_near;
        m_far = other.m_far;
        m_aspect_ratio = other.m_aspect_ratio;
        m_height_angle = other.m_height_angle;

        // Recompute the view and projection matrices
        _setViewMatrix();
        _setProjectionMatrix();
    }
    return *this;
}


void Camera::_setViewMatrix(){
    glm::vec3 eye = glm::vec3(m_pos_world_space);
    glm::vec3 center = glm::vec3(m_pos_world_space + m_look);
    m_view = glm::lookAt(eye, center, glm::vec3(m_up));
    m_inv_view = glm::inverse(m_view);
}

void Camera::_setProjectionMatrix(){
    glm::mat4 proj;
    proj[0][0] = 1.0f/(m_far*glm::tan(m_height_angle/2)*m_aspect_ratio);
    proj[1][1] = 1.0f/(m_far*glm::tan(m_height_angle/2));
    proj[2][2] = -(m_far + m_near) / (m_far*(m_far - m_near));
    proj[3][2] = -2.0f * m_near / (m_far - m_near);
    proj[2][3] = -1.0f / m_far;

    m_proj = proj;
}


glm::mat4 Camera::getViewMatrix() const {
    return m_view;
}
glm::mat4 Camera::getInvViewMatrix() const {
    return m_inv_view;
}
glm::mat4 Camera::getProjectionMatrix() const {
    return m_proj;
}
glm::vec4 Camera::getPosWorldSpace() const {
    return m_pos_world_space;
}

float Camera::getHeightAngle() const {
    return m_height_angle;
}

glm::vec3 Camera::getLook() {
    return glm::vec3(m_look);
}

glm::vec3 Camera::getUp() {
    return glm::vec3(m_up);
}

void Camera::moveCameraW(float deltaTime) {
    m_pos_world_space = m_pos_world_space + 5*deltaTime*glm::normalize(m_look);
    _setViewMatrix();
}
void Camera::moveCameraS(float deltaTime) {
    m_pos_world_space = m_pos_world_space - 5*deltaTime*glm::normalize(m_look);
    _setViewMatrix();
}
void Camera::moveCameraA(float deltaTime) {
    glm::vec4 left = glm::vec4(glm::normalize(glm::cross(glm::vec3(m_up), glm::vec3(m_look))), 0);
    m_pos_world_space = m_pos_world_space + 5*deltaTime*left;
    _setViewMatrix();
}
void Camera::moveCameraD(float deltaTime) {
    glm::vec4 right = glm::vec4(glm::normalize(glm::cross(glm::vec3(m_look), glm::vec3(m_up))), 0);
    m_pos_world_space = m_pos_world_space + 5*deltaTime*right;
    _setViewMatrix();
}
void Camera::moveCameraSpace(float deltaTime) {
    m_pos_world_space = m_pos_world_space + 5*deltaTime*glm::vec4(0, 1, 0, 0);
    _setViewMatrix();
}
void Camera::moveCameraCtrl(float deltaTime) {
    m_pos_world_space = m_pos_world_space - 5*deltaTime*glm::vec4(0, 1, 0, 0);
    _setViewMatrix();
}
void Camera::moveCameraMouseX(float deltaX) {

    // rotate about (0,1,0)
    glm::vec3 r = glm::normalize(glm::vec3(0,-1,0));
    glm::mat3 rotation =
    {
        glm::cos(deltaX)+r.x*r.x*(1-glm::cos(deltaX)), r.x*r.y*(1-glm::cos(deltaX))+r.z*glm::sin(deltaX), r.x*r.z*(1-glm::cos(deltaX))-r.y*glm::sin(deltaX),
        r.x*r.y*(1-glm::cos(deltaX))-r.z*glm::sin(deltaX),glm::cos(deltaX)+r.y*r.y*(1-glm::cos(deltaX)), r.y*r.z*(1-glm::cos(deltaX))+r.x*glm::sin(deltaX),
        r.x*r.z*(1-glm::cos(deltaX))+r.y*glm::sin(deltaX), r.y*r.z*(1-glm::cos(deltaX))-r.x*glm::sin(deltaX), glm::cos(deltaX)+r.z*r.z*(1-glm::cos(deltaX))
    };

    //m_pos_world_space = glm::vec4(rotation*glm::vec3(m_pos_world_space),1);
    m_look = glm::vec4(rotation*glm::vec3(m_look),1);
    _setViewMatrix();
}
void Camera::moveCameraMouseY(float deltaY) {

    // rotate about cross(m_up, m_look)
    glm::vec3 r = glm::normalize(glm::cross(glm::vec3(m_up),glm::vec3(m_look)));
    glm::mat3 rotation =
    {
        glm::cos(deltaY)+r.x*r.x*(1-glm::cos(deltaY)), r.x*r.y*(1-glm::cos(deltaY))+r.z*glm::sin(deltaY), r.x*r.z*(1-glm::cos(deltaY))-r.y*glm::sin(deltaY),
        r.x*r.y*(1-glm::cos(deltaY))-r.z*glm::sin(deltaY),glm::cos(deltaY)+r.y*r.y*(1-glm::cos(deltaY)), r.y*r.z*(1-glm::cos(deltaY))+r.x*glm::sin(deltaY),
        r.x*r.z*(1-glm::cos(deltaY))+r.y*glm::sin(deltaY), r.y*r.z*(1-glm::cos(deltaY))-r.x*glm::sin(deltaY), glm::cos(deltaY)+r.z*r.z*(1-glm::cos(deltaY))
    };

    //m_pos_world_space = glm::vec4(rotation*glm::vec3(m_pos_world_space),1);
    m_look = glm::vec4(rotation*glm::vec3(m_look),1);
    _setViewMatrix();
}




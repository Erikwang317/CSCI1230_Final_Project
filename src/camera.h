#pragma once

#include <glm/glm.hpp>

class Camera {
    float m_aspect_ratio;
    float m_height_angle;
    float m_near;
    float m_far;

    glm::vec4 m_pos_world_space;
    glm::vec4 m_look;
    glm::vec4 m_up;

    glm::mat4 m_view;
    glm::mat4 m_inv_view;
    glm::mat4 m_proj;

    void _setViewMatrix();
    void _setProjectionMatrix();

public:
    Camera();
    Camera(glm::vec4, glm::vec4, glm::vec4, float, float, float, float);
    Camera& operator=(const Camera& other);

    // Adjusts m_pos
    void moveCameraW(float );
    void moveCameraS(float );
    void moveCameraA(float );
    void moveCameraD(float );
    void moveCameraSpace(float );
    void moveCameraCtrl(float );
    void moveCameraMouseX(float );
    void moveCameraMouseY(float );

    // Returns the view matrix for the current camera settings.
    glm::mat4 getViewMatrix() const;
    glm::mat4 getInvViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec4 getPosWorldSpace() const;

    glm::vec3 getLook() const;
    glm::vec3 getUp() const;

    float getAspectRatio() const;
    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

};

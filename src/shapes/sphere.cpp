#include "Sphere.h"
#include <math.h>

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
    m_data_size = m_vertexData.size();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    glm::vec3 n1 = glm::normalize(glm::vec3(2*topLeft.x, 2*topLeft.y, 2*topLeft.z));
    glm::vec3 n2 = glm::normalize(glm::vec3(2*bottomRight.x, 2*bottomRight.y, 2*bottomRight.z));
    glm::vec3 n3 = glm::normalize(glm::vec3(2*topRight.x, 2*topRight.y, 2*topRight.z));
    glm::vec3 n4 = glm::normalize(glm::vec3(2*bottomLeft.x, 2*bottomLeft.y, 2*bottomLeft.z));

    std::vector<glm::vec3> order = {topLeft, n1, bottomRight, n2, topRight, n3, topLeft, n1, bottomLeft, n4, bottomRight, n2};
    for (auto x: order){
        for (int i=0; i<3; i++){
            m_vertexData.push_back(x[i]);
        }
    }
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    for (int i=0; i<m_param1; i++){
        float phi1 = i*glm::radians(180.0f)/m_param1;
        float phi2 = (i+1)*glm::radians(180.0f)/m_param1;
        glm::vec3 tl(m_radius*glm::sin(phi1)*glm::sin(currentTheta),
                     m_radius*glm::cos(phi1),
                     m_radius*glm::sin(phi1)*glm::cos(currentTheta));
        glm::vec3 tr(m_radius*glm::sin(phi1)*glm::sin(nextTheta),
                     m_radius*glm::cos(phi1),
                     m_radius*glm::sin(phi1)*glm::cos(nextTheta));
        glm::vec3 bl(m_radius*glm::sin(phi2)*glm::sin(currentTheta),
                     m_radius*glm::cos(phi2),
                     m_radius*glm::sin(phi2)*glm::cos(currentTheta));
        glm::vec3 br(m_radius*glm::sin(phi2)*glm::sin(nextTheta),
                     m_radius*glm::cos(phi2),
                     m_radius*glm::sin(phi2)*glm::cos(nextTheta));
        makeTile(tl,tr,bl,br);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i=0; i<m_param2; i++){
        float currrentTheta = i*thetaStep;
        float nextTheta = (i+1)*thetaStep;
        makeWedge(currrentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    //     float thetaStep = glm::radians(360.f / m_param2);
    //     float currentTheta = 0 * thetaStep;
    //     float nextTheta = 1 * thetaStep;
    //     makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

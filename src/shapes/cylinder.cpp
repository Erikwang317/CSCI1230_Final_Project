#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
    m_data_size = m_vertexData.size();
}


void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {

    glm::vec3 n1 = glm::normalize(glm::vec3(2*topLeft.x, 0.0f, 2*topLeft.z));
    glm::vec3 n2 = glm::normalize(glm::vec3(2*bottomRight.x, 0.0f, 2*bottomRight.z));
    glm::vec3 n3 = glm::normalize(glm::vec3(2*topRight.x, 0.0f, 2*topRight.z));
    glm::vec3 n4 = glm::normalize(glm::vec3(2*bottomLeft.x, 0.0f, 2*bottomLeft.z));

    std::vector<glm::vec3> order = {topLeft, n1, bottomRight, n2, topRight, n3, topLeft, n1, bottomLeft, n4, bottomRight, n2};
    for (auto x: order){
        for (int i=0; i<3; i++){
            m_vertexData.push_back(x[i]);
        }
    }
}

void Cylinder::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {
    float normal_y;
    if (topLeft.y == 0.5f){ normal_y=1.0f; } else { normal_y=-1.0f; }

    glm::vec3 n = glm::vec3(0.0f, normal_y, 0.0f);
    std::vector<glm::vec3> order = {topLeft, n, bottomRight, n, topRight, n, topLeft, n, bottomLeft, n, bottomRight, n};
    for (auto x: order){
        for (int i=0; i<3; i++){
            m_vertexData.push_back(x[i]);
        }
    }
}


void Cylinder::makeSideFace(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {

    for (int j=0; j<m_param1; j++){
        float b1 = (float)j/m_param1;
        float b2 = (float)(j+1)/m_param1;
        glm::vec3 tl = (1-b1)*topLeft+b1*bottomLeft;
        glm::vec3 tr = (1-b1)*topRight+b1*bottomRight;
        glm::vec3 bl = (1-b2)*topLeft+b2*bottomLeft;
        glm::vec3 br = (1-b2)*topRight+b2*bottomRight;

        makeSideTile(tl,tr,bl,br);
    }
}


void Cylinder::makeSliceCap(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight){
    for (int j=0; j<m_param1; j++){
        float b1 = (float)j/m_param1;
        float b2 = (float)(j+1)/m_param1;

        glm::vec3 tl = (1-b1)*center+b1*bottomLeft;
        glm::vec3 tr = (1-b1)*center+b1*bottomRight;
        glm::vec3 bl = (1-b2)*center+b2*bottomLeft;
        glm::vec3 br = (1-b2)*center+b2*bottomRight;

        makeCapTile(tl,tr,bl,br);
    }
}


void Cylinder::circleAround(){
    glm::vec3 top_cap_center = glm::vec3(0.0f,0.5f,0.0f);
    glm::vec3 bottom_cap_center = glm::vec3(0.0f,-0.5f,0.0f);
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i=0; i<m_param2; i++){
        float currrentTheta = i*thetaStep;
        float nextTheta = (i+1)*thetaStep;

        glm::vec3 tl = glm::vec3(m_radius*glm::sin(currrentTheta), 0.5f, m_radius*glm::cos(currrentTheta));
        glm::vec3 tr = glm::vec3(m_radius*glm::sin(nextTheta), 0.5f, m_radius*glm::cos(nextTheta));
        glm::vec3 bl = glm::vec3(m_radius*glm::sin(currrentTheta), -0.5f, m_radius*glm::cos(currrentTheta));
        glm::vec3 br = glm::vec3(m_radius*glm::sin(nextTheta), -0.5f, m_radius*glm::cos(nextTheta));
        makeSideFace(tl, tr, bl, br);

        glm::vec3 top_cap_bl = tl;
        glm::vec3 top_cap_br = tr;
        makeSliceCap(top_cap_center, top_cap_bl, top_cap_br);

        glm::vec3 bottom_cap_bl = br;
        glm::vec3 bottom_cap_br = bl;
        makeSliceCap(bottom_cap_center, bottom_cap_bl, bottom_cap_br);
    }
}


void Cylinder::setVertexData() {
    circleAround();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

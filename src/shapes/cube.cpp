#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
    m_data_size = m_vertexData.size();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 v1 = glm::normalize(topRight - bottomRight);
    glm::vec3 v2 = glm::normalize(topLeft - bottomRight);
    glm::vec3 n1 = glm::cross(v1,v2);

    glm::vec3 v3 = glm::normalize(bottomLeft - topLeft);
    glm::vec3 v4 = glm::normalize(bottomRight - topLeft);
    glm::vec3 n2 = glm::cross(v3,v4);

    std::vector<glm::vec3> order = {topLeft, n1, bottomRight, n1, topRight, n1, topLeft, n2, bottomLeft, n2, bottomRight, n2};
    for (auto x: order){
        for (int i=0; i<3; i++){
            m_vertexData.push_back(x[i]);
        }
    }

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube

    for (int i=0; i<m_param1; i++){
        float a1 = (float)i/m_param1;
        float a2 = (float)(i+1)/m_param1;
        for (int j=0; j<m_param1; j++){
            float b1 = (float)j/m_param1;
            float b2 = (float)(j+1)/m_param1;
            glm::vec3 tl = (1-b1)*((1-a1)*topLeft+a1*topRight)+b1*((1-a1)*bottomLeft+a1*bottomRight);
            glm::vec3 tr = (1-b1)*((1-a2)*topLeft+a2*topRight)+b1*((1-a2)*bottomLeft+a2*bottomRight);
            glm::vec3 bl = (1-b2)*((1-a1)*topLeft+a1*topRight)+b2*((1-a1)*bottomLeft+a1*bottomRight);
            glm::vec3 br = (1-b2)*((1-a2)*topLeft+a2*topRight)+b2*((1-a2)*bottomLeft+a2*bottomRight);

            makeTile(tl,tr,bl,br);
        }
    }

}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

    //    makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //             glm::vec3( 0.5f,  0.5f, 0.5f),
    //             glm::vec3(-0.5f, -0.5f, 0.5f),
    //             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
    makeFace(glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f,-0.5f));

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f));
    makeFace(glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f,-0.5f),
             glm::vec3(0.5f,  0.5f,-0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }
    int getDataSize() const { return m_data_size; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();


    void makeSideTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight);
    void makeCapTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight);
    void makeSideFace(glm::vec3 center,glm::vec3 bottomLeft,glm::vec3 bottomRight);
    void makeSliceCap(glm::vec3 center, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void circleAround();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    int m_data_size = 0;
};

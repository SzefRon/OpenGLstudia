#pragma once

#include "Shader.h"
#include "GraphNode.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Cone : public GraphNode
{
private:
    int precision;
    float diameter, height;
    float *vertices;
    unsigned int *indices;
    unsigned int VBO, VAO, EBO;
public:
    Cone(int precision, float diameter, float height);
    ~Cone() = default;

    void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};
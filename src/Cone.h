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
    unsigned int VBO, VAO, EBO;
    glm::vec4 color;

    void generate();
public:
    Cone(int precision, float diameter, float height, glm::vec4 color);
    ~Cone() = default;

    void changePrecision(int precision);

    void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};
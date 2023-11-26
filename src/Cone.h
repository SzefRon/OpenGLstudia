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

    void generate();
public:
    Cone(int precision, float diameter, float height);
    ~Cone() = default;

    void changePrecision(int precision);

    void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};
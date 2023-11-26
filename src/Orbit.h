#pragma once

#include "Shader.h"
#include "GraphNode.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Orbit : public GraphNode
{
private:
    int precision;
    float radius;
    unsigned int VBO, VAO;

    void generate();
public:
    Orbit(int precision, float radius);
    ~Orbit() = default;

    void changePrecision(int precision);

    void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};
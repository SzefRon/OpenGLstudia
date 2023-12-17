#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "vModel.h"
#include "Shader.h"

class PointLight : public GraphNode
{
private:
    int index;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;
public:
    PointLight(int index, float constant, float linear, float quadratic, glm::vec3 color);
    void useLight(Shader &shader);
};
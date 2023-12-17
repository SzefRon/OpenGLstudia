#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "vModel.h"
#include "Shader.h"

class PointLight : public GraphNode
{
public:
    int enabled = 1;
    int index;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;

    PointLight(int index, float constant, float linear, float quadratic, glm::vec3 color);
    void useLight(Shader &shader);
};
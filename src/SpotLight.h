#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"

class SpotLight : public GraphNode
{
private:
    int index;
    float cutoff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;
public:
    SpotLight(int index, float cutoff, float outerCutoff, float constant, float linear, float quadratic, glm::vec3 color);
    void useLight(Shader &shader);
};
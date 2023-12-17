#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "Shader.h"

class DirectionalLight : public GraphNode
{
public:
    int enabled = 1;
    glm::vec3 color;

    DirectionalLight(glm::vec3 color);
    void useLight(Shader &shader);
};
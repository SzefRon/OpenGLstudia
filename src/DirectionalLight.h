#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "Shader.h"

class DirectionalLight : public GraphNode
{
private:
    glm::vec3 color;
public:
    DirectionalLight(glm::vec3 color);
    void useLight(Shader &shader);
};
#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "vModel.h"
#include "Instancer.h"

class InstanceNode : public GraphNode
{
protected:
    Instancer *instancer;
    unsigned int index;
public:
    InstanceNode(glm::mat4 model, unsigned int index, Instancer *instancer);
    void addChild(GraphNode *object);
    void updateSelfChildren(float deltaTime);
    void draw(Shader &shader) {}
};
#pragma once

#include <glm/glm.hpp>

#include "GraphNode.h"
#include "vModel.h"
#include "Instancer.h"

class InstanceNode : public GraphNode
{
private:
    Instancer *instancer;
    int noInstances;
    std::deque<InstanceNode *> objects;
    vModel *model;
public:
    glm::mat4 *modelMatrices;
    std::deque<int> dirtyIndexes;
    InstanceNode(vModel *model, glm::mat4 *modelMatrices, int noInstances);
    void addChild(GraphNode *object);
    void updateSelfChildren(float deltaTime);
    void draw(Shader &shader);
};
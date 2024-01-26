#pragma once

#include "InstanceNode.h"
#include "Instancer.h"

class DoorNode : public InstanceNode
{
private:
    float offset = 0.0f;
public:
    float rotateOffset = 0.0f;
    DoorNode(glm::mat4 model, unsigned int index, Instancer *instancer, float offset);
    void updateSelfChildren(float deltaTime) override;
};
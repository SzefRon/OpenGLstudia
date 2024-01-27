#pragma once

#include "InstanceNode.h"
#include "Instancer.h"

class DoorNode : public InstanceNode
{
private:
    float offset = 0.0f;
    bool direction;
public:
    float rotateOffset = 0.0f;
    DoorNode(glm::mat4 model, unsigned int index, Instancer *instancer, float offset, bool direction);
    void updateSelfChildren(float deltaTime) override;
    static DoorNode *create(int amount, glm::mat4 *modelMatrices, unsigned int startingIndex, Instancer *instancer, float offset, bool direction);
};

inline DoorNode *DoorNode::create(int amount, glm::mat4 *modelMatrices, unsigned int startingIndex, Instancer *instancer, float offset, bool direction)
{
    DoorNode *mainNode = new DoorNode(modelMatrices[startingIndex], startingIndex, instancer, offset, direction);
    DoorNode *currentNode = mainNode;
    for (int i = 0; i < amount - 1; i++) {
        currentNode = new DoorNode(modelMatrices[startingIndex + i + 1], startingIndex + i + 1, instancer, offset, direction);
        currentNode->addChild(mainNode);
        currentNode->makeDirty();
        mainNode = currentNode;
    }
    return mainNode;
}
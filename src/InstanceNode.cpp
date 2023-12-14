#include "InstanceNode.h"

InstanceNode::InstanceNode(vModel *model, glm::mat4 *modelMatrices, int noInstances)
    : noInstances(noInstances), modelMatrices(modelMatrices), model(model)
{
    instancer = new Instancer(model, modelMatrices, noInstances);
}

void InstanceNode::addChild(GraphNode *object)
{
    InstanceNode *instanceNode = dynamic_cast<InstanceNode *>(object);
    objects.push_back(instanceNode);
    instanceNode->parent = this;
}

void InstanceNode::updateSelfChildren(float deltaTime)
{
    InstanceNode *instanceParent = dynamic_cast<InstanceNode *>(parent);
    if (!dirtyIndexes.empty()) {
        if (instanceParent) {
            glm::mat4 *parentMatrices = instanceParent->modelMatrices;
            for (auto &index : dirtyIndexes) {
                modelMatrices[index] = parentMatrices[index] * modelMatrices[index];
            }
            instancer->updateInstances(modelMatrices);
        } else {
            instancer->updateInstances(modelMatrices);
        }
        dirtyIndexes.clear();
    }

    for (auto &object : objects) {
        object->updateSelfChildren(deltaTime);
    }
}

void InstanceNode::draw(Shader &shader)
{
    instancer->drawInstances(shader);
}

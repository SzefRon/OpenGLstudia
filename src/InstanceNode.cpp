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
    if (instanceParent) {
        glm::mat4 *updatedMatrices;
        glm::mat4 *parentMatrices = instanceParent->modelMatrices;
        updatedMatrices = new glm::mat4[noInstances];
        for (int i = 0; i < noInstances; i++) {
            updatedMatrices[i] = parentMatrices[i] * modelMatrices[i];
        }
        instancer->updateInstances(updatedMatrices);
        delete[] updatedMatrices;
    } else {
        instancer->updateInstances(modelMatrices);
    }

    for (auto &object : objects) {
        object->updateSelfChildren(deltaTime);
    }
}

void InstanceNode::draw(Shader &shader)
{
    instancer->drawInstances(shader);
}

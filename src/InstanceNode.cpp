#include "InstanceNode.h"

InstanceNode::InstanceNode(glm::mat4 model, unsigned int index, Instancer *instancer)
    : index(index), instancer(instancer)
{
    this->model = model;
}

void InstanceNode::addChild(GraphNode *child)
{
    children.push_back(child);
    child->parent = this;
}

void InstanceNode::updateSelfChildren(float deltaTime)
{
    model = parent->model;
    
    model = glm::translate(model, translation);

    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    instancer->updateInstance(model, index);
    isDirty = false;
    for (auto &dirtyChild : dirtyChildren) {
        dirtyChild->updateSelfChildren(deltaTime);
    }
    dirtyChildren.clear();
}

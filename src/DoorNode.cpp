#include "DoorNode.h"

DoorNode::DoorNode(glm::mat4 model, unsigned int index, Instancer *instancer, float offset, bool direction)
    : InstanceNode(model, index, instancer), offset(offset), direction(direction)
{}

void DoorNode::updateSelfChildren(float deltaTime)
{
    DoorNode *parentDoor = dynamic_cast<DoorNode *>(parent);
    
    model = parent->model;
    
    model = glm::translate(model, translation);

    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    if (parentDoor != nullptr) {
        float parentRotateOffset = parentDoor->rotateOffset;
        rotateOffset = -parentRotateOffset;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, offset));
        model = glm::rotate(model, rotateOffset * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        rotateOffset = fabs(rotateOffset) * (direction ? 1.0f : -1.0f);
        model = glm::rotate(model, rotateOffset, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    model = glm::scale(model, scale);

    instancer->updateInstance(model, index);
    isDirty = false;
    for (auto &dirtyChild : dirtyChildren) {
        dirtyChild->updateSelfChildren(deltaTime);
    }
    dirtyChildren.clear();
}
#include "GraphNode.h"

GraphNode::GraphNode(float distance, float speed, float offset)
    : distance(distance), speed(speed), offset(offset)
{
    model = glm::mat4(1);
}

void GraphNode::addChild(GraphNode *object)
{
    objects.push_back(object);
    object->parent = this;
}

void GraphNode::updateSelfChildren(float deltaTime)
{
    if (parent) {
        model = parent->model;
    } else {
        model = glm::mat4(1);
    }
    rotation += speed * deltaTime;
    model = glm::rotate(model, offset, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(distance, 0.0f, 0.0f));

    for (auto &object : objects) {
        object->updateSelfChildren(deltaTime);
    }
}

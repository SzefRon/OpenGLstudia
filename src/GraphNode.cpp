#include "GraphNode.h"

GraphNode::GraphNode()
{
    model = glm::mat4(1);
}

void GraphNode::addChild(GraphNode *object)
{
    children.push_back(object);
    object->parent = this;
}

void GraphNode::updateSelfChildren(float deltaTime)
{
    if (parent) {
        model = parent->model;
    } else {
        model = glm::mat4(1);
    }

    model = glm::translate(model, translation);

    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    isDirty = false;

    for (auto &dirtyChild : dirtyChildren) {
        dirtyChild->updateSelfChildren(deltaTime);
    }
    dirtyChildren.clear();
}

void GraphNode::makeDirty()
{
    updateParentNodes();
    makeChildrenDirty();
}

void GraphNode::updateParentNodes()
{
    isDirty = true;
    if (parent) {
        parent->dirtyChildren.push_back(this);
        if (!(parent->isDirty))
            parent->updateParentNodes();
    }
}

void GraphNode::makeChildrenDirty()
{
    isDirty = true;
    for (auto &child : children) {
        dirtyChildren.push_back(child);
        child->makeChildrenDirty();
    }
}

void GraphNode::setTranslation(glm::vec3 translation)
{
    this->translation = translation;
    makeDirty();
}

void GraphNode::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
    makeDirty();
}

void GraphNode::setScale(glm::vec3 scale)
{
    this->scale = scale;
    makeDirty();
}

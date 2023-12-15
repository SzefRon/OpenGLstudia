#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class GraphNode
{
protected:
    std::deque<GraphNode *> children;
    std::deque<GraphNode *> dirtyChildren;
    bool isDirty = false;
public:
    glm::vec3 translation = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    GraphNode *parent = nullptr;

    GraphNode();
    ~GraphNode() = default;

    virtual void addChild(GraphNode *object);
    virtual void updateSelfChildren(float deltaTime);

    void makeDirty();
    void updateParentNodes();
    void makeChildrenDirty();

    void setTranslation(glm::vec3 translation);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);

    virtual void draw(Shader &shader) {}
    
    glm::mat4 model;
};
#pragma once

#include "Drawable.h"

#include <deque>
#include <glm/glm.hpp>

class GraphNode
{
private:
    std::deque<GraphNode *> objects;

    float distance, speed, offset;
    float rotation = 0.0f;
protected:
    GraphNode *parent = nullptr;
public:
    GraphNode() = default;
    GraphNode(float distance, float speed, float offset);
    ~GraphNode() = default;

    void addChild(GraphNode *object);
    virtual void updateSelfChildren(float deltaTime);
    
    glm::mat4 model;
};
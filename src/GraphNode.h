#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class GraphNode
{
private:
    std::deque<GraphNode *> objects;

    float distance, speed, offset, pitch;
    float rotation = 0.0f;
protected:
    GraphNode *parent = nullptr;
public:
    GraphNode() = default;
    GraphNode(float distance, float speed, float offset, float pitch);
    ~GraphNode() = default;

    void addChild(GraphNode *object);
    virtual void updateSelfChildren(float deltaTime);
    virtual void draw(Shader &shader) {}
    
    glm::mat4 model;
};
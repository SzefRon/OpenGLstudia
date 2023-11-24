#pragma once

#include "Shader.h"

#include <glm/glm.hpp>

class Drawable
{
protected:
    Drawable() = default;
    glm::mat4 model;
public:
    virtual ~Drawable() = default;
    virtual void draw(Shader &shader) = 0;
    virtual void updateSelf(float deltaTime) = 0;
};
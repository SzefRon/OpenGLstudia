#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

#include "vModel.h"

class Instancer
{
private:
    unsigned int bufferID;
    unsigned int noInstances;
    vModel *model;
public:
    Instancer(vModel *model, glm::mat4 *modelMatrices, unsigned int noInstances);
    void updateInstances(glm::mat4 *modelMatrices);
    void drawInstances(Shader &shader);
};
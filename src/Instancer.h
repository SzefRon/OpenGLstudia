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
    glm::mat4 *modelMatrices;

    Instancer(vModel *model, unsigned int noInstances, glm::mat4 *modelMatrices);
    void updateInstance(glm::mat4 modelMatrix, unsigned int index);
    void updateInstances();
    void drawInstances(Shader &shader);
};
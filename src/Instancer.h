#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

#include "vModel.h"

class Instancer
{
public:
    static void createInstances(vModel *model, glm::mat4 *modelMatrices, unsigned int noInstances);
    static void drawInstances(vModel *model, int amount, Shader &shader);
};
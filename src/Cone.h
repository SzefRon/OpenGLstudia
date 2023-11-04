#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Cone
{
private:
    int precision;
    float diameter, height;
    float *vertices;
    unsigned int *indices;
    glm::mat4 mat;
    unsigned int VBO, VAO, EBO;
public:
    Cone(float x, float y, float z, int precision, float diameter, float height);
    void draw(Shader &shader);
};
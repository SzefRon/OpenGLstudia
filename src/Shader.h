#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader
{
private:
    unsigned int programID;
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    void use();
    const unsigned int &getProgramID();
    void setUniformMat4(const char *name, glm::mat4 &val);
    void setUniform4fv(const char *name, glm::vec4 &val);
};
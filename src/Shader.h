#pragma once

#include <glad/glad.h>

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
};
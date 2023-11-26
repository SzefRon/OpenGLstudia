#include "Shader.h"

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
    std::ifstream vertexFile, fragmentFile;
    std::string vertexCode, fragmentCode;

    vertexFile.open(vertexPath);
    fragmentFile.open(fragmentPath);

    if (vertexFile.is_open() && fragmentFile.is_open()) {
        std::stringstream vertexSStream, fragmentSStream;

        vertexFile >> vertexSStream.rdbuf();
        fragmentFile >> fragmentSStream.rdbuf();

        vertexCode = vertexSStream.str();
        fragmentCode = fragmentSStream.str();

        vertexFile.close();
        fragmentFile.close();
    }

    const char *cVertexCode = vertexCode.c_str();
    const char *cFragmentCode = fragmentCode.c_str();

    int success;
    char infoLog[512];
    unsigned int vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &cVertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &cFragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use()
{
    glUseProgram(programID);
}

const unsigned int &Shader::getProgramID()
{
    return programID;
}

void Shader::setUniformMat4(const char *name, glm::mat4 &val)
{
    unsigned location = glGetUniformLocation(programID, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setUniform4fv(const char * name, glm::vec4 &val)
{
    unsigned location = glGetUniformLocation(programID, name);
    glUniform4fv(location, 1, glm::value_ptr(val));
}

void Shader::setUInt(const char *name, unsigned int &val)
{
    unsigned location = glGetUniformLocation(programID, name);
    glUniform1ui(location, val);
}

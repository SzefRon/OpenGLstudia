#include "PointLight.h"

PointLight::PointLight(int index, float constant, float linear, float quadratic, glm::vec3 color)
    : index(index), constant(constant), linear(linear), quadratic(quadratic), color(color)
{}

void PointLight::useLight(Shader &shader)
{
    std::string indexStr = std::to_string(index);

    shader.setUniform3fv(("pointLights[" + indexStr + "].position").c_str(), translation);
    shader.setUniform3fv(("pointLights[" + indexStr + "].color").c_str(), color);
    shader.setFloat(("pointLights[" + indexStr + "].constant").c_str(), constant);
    shader.setFloat(("pointLights[" + indexStr + "].linear").c_str(), linear);
    shader.setFloat(("pointLights[" + indexStr + "].quadratic").c_str(), quadratic);
    shader.setInt(("pointLights[" + indexStr + "].enabled").c_str(), enabled);
}

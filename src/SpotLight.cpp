#include "SpotLight.h"

SpotLight::SpotLight(int index, float cutoff, float outerCutoff, float constant, float linear, float quadratic, glm::vec3 color)
    : index(index), cutoff(cutoff), outerCutoff(outerCutoff), constant(constant), linear(linear), quadratic(quadratic), color(color)
{}

void SpotLight::useLight(Shader &shader)
{
    std::string indexStr = std::to_string(index);
    glm::vec4 direction(0.0f, 1.0f, 0.0f, 1.0f);
    glm::mat4 rotMat(1.0f);

    rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    direction = rotMat * direction;

    glm::vec3 direction3D(direction.x, direction.y, direction.z);

    shader.setUniform3fv(("spotLights[" + indexStr + "].direction").c_str(), direction3D);
    shader.setUniform3fv(("spotLights[" + indexStr + "].position").c_str(), translation);
    shader.setUniform3fv(("spotLights[" + indexStr + "].color").c_str(), color);
    shader.setFloat(("spotLights[" + indexStr + "].constant").c_str(), constant);
    shader.setFloat(("spotLights[" + indexStr + "].linear").c_str(), linear);
    shader.setFloat(("spotLights[" + indexStr + "].quadratic").c_str(), quadratic);
    shader.setFloat(("spotLights[" + indexStr + "].cutOff").c_str(), cutoff);
    shader.setFloat(("spotLights[" + indexStr + "].outerCutOff").c_str(), outerCutoff);
    shader.setInt(("spotLights[" + indexStr + "].enabled").c_str(), enabled);
}

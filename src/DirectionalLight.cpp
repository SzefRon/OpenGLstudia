#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 color)
    : color(color)
{}

void DirectionalLight::useLight(Shader &shader)
{
    glm::vec4 direction(0.0f, 1.0f, 0.0f, 0.0f);
    glm::mat4 rotMat(1.0f);

    rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    direction = rotMat * direction;

    glm::vec3 direction3D(direction.x, direction.y, direction.z);

    shader.setUniform3fv("dirLight.direction", direction3D);
    shader.setUniform3fv("dirLight.color", color);
}

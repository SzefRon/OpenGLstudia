#include "Instancer.h"

Instancer::Instancer(vModel *model, unsigned int noInstances, glm::mat4 *modelMatrices)
    : model(model), noInstances(noInstances), modelMatrices(modelMatrices)
{
    this->modelMatrices = modelMatrices;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, noInstances * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    std::vector<vMesh> meshes = model->meshes;

    for (auto &mesh : meshes) {
        unsigned int VAO = mesh.VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    updateInstances();
}

void Instancer::updateInstance(glm::mat4 modelMatrix, unsigned int index)
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glm::mat4 *mappedBuffer = (glm::mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    mappedBuffer[index] = modelMatrix;
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Instancer::updateInstances()
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glm::mat4 *mappedBuffer = (glm::mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    for (int i = 0; i < noInstances; i++) {
        mappedBuffer[i] = modelMatrices[i];
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Instancer::drawInstances(Shader &shader)
{
    int val = 0;
    shader.setInt("texture_diffuse1", val);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures_loaded[0].id);

    std::vector<vMesh> meshes = model->meshes;
    for (auto mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, (mesh.indices.size()), GL_UNSIGNED_INT, 0, noInstances);
        glBindVertexArray(0);
    }
}

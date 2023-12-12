#include "Instancer.h"

void Instancer::drawInstances(vModel *model, int amount, Shader &shader)
{
    int val = 0;
    shader.setInt("texture_diffuse1", val);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures_loaded[0].id);

    std::vector<vMesh> meshes = model->meshes;
    for (auto mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, (mesh.indices.size()), GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
    }
}

void Instancer::createInstances(vModel *model, glm::mat4 *modelMatrices, unsigned int noInstances)
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
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
}
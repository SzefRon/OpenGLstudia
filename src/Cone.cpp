#include "Cone.h"

Cone::Cone(float x, float y, float z, int precision, float radius, float height)
    : precision(precision)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    vertices = new float[3 * (precision + 2)];

    vertices[3 * precision] = 0.0f;
    vertices[3 * precision + 1] = 0.0f;
    vertices[3 * precision + 2] = 0.5f;

    vertices[3 * (precision + 1)] = 0.0f;
    vertices[3 * (precision + 1) + 1] = 0.0f;
    vertices[3 * (precision + 1) + 2] = 0.0f;

    float pi = glm::pi<float>();
    float recip = 1.0f / precision;

    for (int i = 0; i < precision; i++) {
        float x = 0.5f * glm::sin(2.0f * pi * i * recip);
        float y = 0.5f * glm::cos(2.0f * pi * i * recip);
        vertices[3 * i] = x;
        vertices[3 * i + 1] = y;
        vertices[3 * i + 2] = 0.0f;
    }

    indices = new unsigned int[6 * precision];

    for (int i = 0; i < precision; i++) {
        // Bottom
        indices[3 * i] = i;
        indices[3 * i + 1] = (i + 1) % precision;
        indices[3 * i + 2] = precision + 1;

        // Sides
        indices[3 * (i + precision)] = i;
        indices[3 * (i + precision) + 1] = (i + 1) % precision;
        indices[3 * (i + precision) + 2] = precision;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (precision + 1) * 3, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * precision * 6, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    mat = glm::mat4(1);
    //mat = glm::scale(mat, glm::vec3(radius, radius, height));
    mat = glm::translate(mat, glm::vec3(x, y, z));

    delete vertices, indices;
}

void Cone::draw(Shader &shader)
{
    shader.setUniformMat4("model", mat);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * precision, GL_UNSIGNED_INT, 0);
}

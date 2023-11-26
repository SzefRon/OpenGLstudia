#include "Cone.h"

void Cone::generate()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    float *vertices = new float[3 * (precision + 2)];

    vertices[3 * precision + 0] = 0.0f;
    vertices[3 * precision + 1] = 0.5f;
    vertices[3 * precision + 2] = 0.0f;

    vertices[3 * (precision + 1) + 0] = 0.0f;
    vertices[3 * (precision + 1) + 1] = -0.5f;
    vertices[3 * (precision + 1) + 2] = 0.0f;

    float pi = glm::pi<float>();
    float recip = 1.0f / precision;

    for (int i = 0; i < precision; i++) {
        float x = 0.5f * glm::sin(2.0f * pi * i * recip);
        float y = 0.5f * glm::cos(2.0f * pi * i * recip);
        vertices[3 * i] = x;
        vertices[3 * i + 1] = -0.5f;
        vertices[3 * i + 2] = y;
    }

    unsigned int *indices = new unsigned int[6 * precision];

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (precision + 2) * 3, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * precision * 6, indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    model = glm::mat4(1);

    delete[] vertices;
    delete[] indices;
}

Cone::Cone(int precision, float diameter, float height, glm::vec4 color)
    : precision(precision), diameter(diameter), height(height), color(color)
{
    generate();
}

void Cone::changePrecision(int precision)
{
    this->precision = precision;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    generate();
}

void Cone::draw(Shader &shader)
{
    shader.setUniformMat4("model", model);
    shader.setUniform4fv("color", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * precision, GL_UNSIGNED_INT, 0);
}

void Cone::updateSelfChildren(float deltaTime)
{
    model = parent->model;
    model = glm::scale(model, glm::vec3(diameter, height, diameter));
}

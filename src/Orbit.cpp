#include "Orbit.h"

void Orbit::generate()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    float *vertices = new float[3 * precision];

    float pi = glm::pi<float>();
    float recip = 1.0f / precision;

    for (int i = 0; i < precision; i++) {
        float x = glm::sin(2.0f * pi * i * recip);
        float y = glm::cos(2.0f * pi * i * recip);
        vertices[3 * i] = x;
        vertices[3 * i + 1] = 0.0f;
        vertices[3 * i + 2] = y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * precision * 3, vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    model = glm::mat4(1);
}

Orbit::Orbit(int precision, float radius)
    : precision(precision), radius(radius)
{
    generate();
}

void Orbit::changePrecision(int precision)
{
    this->precision = precision;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    generate();
}

void Orbit::draw(Shader &shader)
{
    shader.setUniformMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, precision);
}

void Orbit::updateSelfChildren(float deltaTime)
{
    model = parent->model;
    model = glm::scale(model, glm::vec3(radius));
}

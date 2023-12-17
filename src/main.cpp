// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "Shader.h"
#include "Cube.h"
#include "Cone.h"
#include "GraphNode.h"
#include "vMesh.h"
#include "vModel.h"
#include "Orbit.h"
#include "Instancer.h"
#include "InstanceNode.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <deque>
#include <array>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX, lastY, deltaTime;
bool firstMouse = true;
bool editMode = false;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!editMode)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_RIGHT_SHIFT) {
            if (editMode)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouse = true;
            }
            editMode ^= true;
        }
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.MovementSpeed += 5.0f * deltaTime;
    else
        camera.MovementSpeed = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 4.3 + GLSL 430
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
#endif
    if (err)
    {
        spdlog::error("Failed to initialize OpenGL loader!");
        return 1;
    }
    spdlog::info("Successfully initialized OpenGL loader!");

    Shader textureShader("..\\res\\shaders\\texture.vert", "..\\res\\shaders\\texture.frag");
    Shader instanceShader("..\\res\\shaders\\instances.vert", "..\\res\\shaders\\texture.frag");
    Shader lightlessShader("..\\res\\shaders\\texture.vert", "..\\res\\shaders\\lightless.frag");

    stbi_set_flip_vertically_on_load(true);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Loop variables

    float speed = 1.0f;
    int indexDomy = 0, indexDachy = 0;
    bool polygonMode = false, vsyncMode = true;
    double lastFrame = 0.0, currFrame = 0.0f;
    vModel *model;

    std::vector<GraphNode *> textureModels;
    std::vector<Cone *> cones;

    // Scene preparation

    const int noInstances = 40000;
    const int noInstancesSqrd = (int)std::ceilf(std::sqrtf((float)noInstances));
    int it = 0;
    glm::mat4 *houseMatrices = new glm::mat4[noInstances];
    glm::mat4 *roofMatrices = new glm::mat4[noInstances];
    glm::mat4 *updatedRoofMatrices = new glm::mat4[noInstances];
    for (int i = 0; i < noInstancesSqrd; i++) {
        for (int j = 0; j < noInstancesSqrd; j++) {
            if (it >= noInstances) break;
            houseMatrices[it] = glm::mat4(1);
            houseMatrices[it] = glm::translate(houseMatrices[it], glm::vec3(i * 3.0f, 0.0f, j * 3.0f));
            roofMatrices[it] = glm::mat4(1);
            roofMatrices[it] = glm::translate(roofMatrices[it], glm::vec3(0.0f, 2.0f, 0.0f));
            updatedRoofMatrices[it] = roofMatrices[it] * houseMatrices[it];
            it++;
        }
    }

    std::deque<InstanceNode *> houseNodes;
    std::deque<InstanceNode *> roofNodes;

    GraphNode *mainNode = new GraphNode();
        GraphNode *housesNode = new GraphNode();
        mainNode->addChild(housesNode);
            vModel *houseModel = new vModel("..\\res\\models\\house\\house.obj", 1.0f);
            vModel *roofModel = new vModel("..\\res\\models\\roof\\roof.obj", 1.0f);
            Instancer *housesInstancer = new Instancer(houseModel, noInstances, houseMatrices);
            Instancer *roofsInstancer = new Instancer(roofModel, noInstances, updatedRoofMatrices);
            for (int i = 0; i < noInstances; i++) {
                InstanceNode *houseNode = new InstanceNode(houseMatrices[i], i, housesInstancer);
                houseNode->translation = houseMatrices[i][3];
                houseNodes.push_back(houseNode);
                housesNode->addChild(houseNode);
                    InstanceNode *roofNode = new InstanceNode(roofMatrices[i], i, roofsInstancer);
                    roofNode->translation = roofMatrices[i][3];
                    roofNodes.push_back(roofNode);
                    houseNode->addChild(roofNode);
            }
        vModel *grassFloor = new vModel("..\\res\\models\\grass-floor\\grass-floor.obj", 1.0f);
        mainNode->addChild(grassFloor); 
        grassFloor->setScale(glm::vec3(noInstancesSqrd * 1.5f + 4.0f));
        grassFloor->setTranslation(glm::vec3(noInstancesSqrd * 1.5f + 2.0f, -1.0f, noInstancesSqrd * 1.5f + 2.0f));
        
        PointLight *pointLight = new PointLight(0, 1.0f, 0.01f, 0.001f, glm::vec3(0.0f, 1.0f, 1.0f));
        mainNode->addChild(pointLight);
            vModel *pointLightModel = new vModel("..\\res\\models\\sun\\sun.obj", 1.0f);
            pointLight->addChild(pointLightModel);
            pointLightModel->setScale(glm::vec3(0.5f));
        pointLight->setTranslation(glm::vec3(0.0f, 10.0f, 0.0f));

        DirectionalLight *directionalLight = new DirectionalLight(glm::vec3(1.0f, 1.0f, 0.5f));
        mainNode->addChild(directionalLight);
            vModel *directionalLightModel = new vModel("..\\res\\models\\arrow\\arrow.obj", 1.0f);
            directionalLight->addChild(directionalLightModel);
        directionalLight->setTranslation(glm::vec3(0.0f, 10.0f, 0.0f));
        directionalLight->setRotation(glm::vec3(2.0f, 0.0f, 0.0f));

        SpotLight *spotLight = new SpotLight(0, 0.9f, 0.8f, 1.0f, 0.01f, 0.001f, glm::vec3(1.0f, 1.0f, 1.0f));
        mainNode->addChild(spotLight);
            vModel *spotLightModel = new vModel("..\\res\\models\\cone\\cone.obj", 1.0f);
            spotLight->addChild(spotLightModel);
        spotLight->setTranslation(glm::vec3(20.0f, 10.0f, 10.0f));
        spotLight->setRotation(glm::vec3(0.0f, 0.0f, -2.0f));

    mainNode->updateSelfChildren(deltaTime);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        //std::cout << deltaTime << ' ';
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        if (editMode) {
            ImGui::SetWindowSize(ImVec2(300.0f, 100.0f));
            ImGui::Begin("Hello darkness my old friend!");

            if (ImGui::CollapsingHeader("Domki")) {
                ImGui::Indent(20.0f);
                if (ImGui::InputInt("Domki Index", &indexDomy)) {
                    if (indexDomy > noInstances - 1) indexDomy = noInstances - 1;
                    if (indexDomy < 0) indexDomy = 0;
                }
                if (ImGui::CollapsingHeader("Domki Translation")) {
                    glm::vec3 &translation = houseNodes.at(indexDomy)->translation;
                    if (ImGui::InputFloat("Domki Pos X", &translation.x)) {
                        houseNodes.at(indexDomy)->setTranslation(translation);
                    }
                    if (ImGui::InputFloat("Domki Pos Y", &translation.y)) {
                        houseNodes.at(indexDomy)->setTranslation(translation);
                    }
                    if (ImGui::InputFloat("Domki Pos Z", &translation.z)) {
                        houseNodes.at(indexDomy)->setTranslation(translation);
                    }
                }
                if (ImGui::CollapsingHeader("Domki Rotation")) {
                    glm::vec3 &rotation = houseNodes.at(indexDomy)->rotation;
                    if (ImGui::InputFloat("Domki Rot X", &rotation.x)) {
                        houseNodes.at(indexDomy)->setRotation(rotation);
                    }
                    if (ImGui::InputFloat("Domki Rot Y", &rotation.y)) {
                        houseNodes.at(indexDomy)->setRotation(rotation);
                    }
                    if (ImGui::InputFloat("Domki Rot Z", &rotation.z)) {
                        houseNodes.at(indexDomy)->setRotation(rotation);
                    }
                }
                if (ImGui::CollapsingHeader("Domki Scale")) {
                    glm::vec3 &scale = houseNodes.at(indexDomy)->scale;
                    if (ImGui::InputFloat("Domki Scale X", &scale.x)) {
                        houseNodes.at(indexDomy)->setScale(scale);
                    }
                    if (ImGui::InputFloat("Domki Scale Y", &scale.y)) {
                        houseNodes.at(indexDomy)->setScale(scale);
                    }
                    if (ImGui::InputFloat("Domki Scale Z", &scale.z)) {
                        houseNodes.at(indexDomy)->setScale(scale);
                    }
                }
                ImGui::Indent(-20.0f);
            }

            if (ImGui::CollapsingHeader("Dachy")) {
                ImGui::Indent(20.0f);
                if (ImGui::InputInt("Dachy Index", &indexDachy)) {
                    if (indexDachy > noInstances - 1) indexDachy = noInstances - 1;
                    if (indexDachy < 0) indexDachy = 0;
                }
                if (ImGui::CollapsingHeader("Dachy Translation")) {
                    glm::vec3 &translation = roofNodes.at(indexDachy)->translation;
                    if (ImGui::InputFloat("Dachy Pos X", &translation.x)) {
                        roofNodes.at(indexDachy)->setTranslation(translation);
                    }
                    if (ImGui::InputFloat("Dachy Pos Y", &translation.y)) {
                        roofNodes.at(indexDachy)->setTranslation(translation);
                    }
                    if (ImGui::InputFloat("Dachy Pos Z", &translation.z)) {
                        roofNodes.at(indexDachy)->setTranslation(translation);
                    }
                }
                if (ImGui::CollapsingHeader("Dachy Rotation")) {
                    glm::vec3 &rotation = roofNodes.at(indexDachy)->rotation;
                    if (ImGui::InputFloat("Dachy Rot X", &rotation.x)) {
                        roofNodes.at(indexDachy)->setRotation(rotation);
                    }
                    if (ImGui::InputFloat("Dachy Rot Y", &rotation.y)) {
                        roofNodes.at(indexDachy)->setRotation(rotation);
                    }
                    if (ImGui::InputFloat("Dachy Rot Z", &rotation.z)) {
                        roofNodes.at(indexDachy)->setRotation(rotation);
                    }
                }
                if (ImGui::CollapsingHeader("Dachy Scale")) {
                    glm::vec3 &scale = roofNodes.at(indexDachy)->scale;
                    if (ImGui::InputFloat("Dachy Scale X", &scale.x)) {
                        roofNodes.at(indexDachy)->setScale(scale);
                    }
                    if (ImGui::InputFloat("Dachy Scale Y", &scale.y)) {
                        roofNodes.at(indexDachy)->setScale(scale);
                    }
                    if (ImGui::InputFloat("Dachy Scale Z", &scale.z)) {
                        roofNodes.at(indexDachy)->setScale(scale);
                    }
                }
                ImGui::Indent(-20.0f);
            }

            if (ImGui::CollapsingHeader("Swiatla")) {
                ImGui::Indent(20.0f);
                ImGui::Indent(-20.0f);
            }

            ImGui::Separator();

            if (ImGui::Checkbox("Wireframe Mode", &polygonMode)) {
                if (polygonMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            if (ImGui::Checkbox("VSync", &vsyncMode)) {
                if (vsyncMode) glfwSwapInterval(1);
                else glfwSwapInterval(0);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // Rendering

        currFrame = glfwGetTime();
        deltaTime = speed * (currFrame - lastFrame);
        lastFrame = currFrame;

        pointLight->setTranslation(glm::vec3(noInstancesSqrd * 1.5f * glm::sin(lastFrame * 0.1f) + noInstancesSqrd * 1.5f, 10.0f, noInstancesSqrd * 1.5f * glm::cos(lastFrame * 0.1f) + noInstancesSqrd * 1.5f));

        mainNode->updateSelfChildren(deltaTime);

        if (!editMode)
            processInput(window);

        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)(display_w) / display_h, 0.1f, 10000.0f);

        textureShader.use();
        textureShader.setUniformMat4("view", view);
        textureShader.setUniformMat4("projection", projection);

        pointLight->useLight(textureShader);
        directionalLight->useLight(textureShader);
        spotLight->useLight(textureShader);

        grassFloor->draw(textureShader);

        instanceShader.use();
        instanceShader.setUniformMat4("view", view);
        instanceShader.setUniformMat4("projection", projection);
        instanceShader.setUniform3fv("viewPos", camera.Position);

        pointLight->useLight(instanceShader);
        directionalLight->useLight(instanceShader);
        spotLight->useLight(instanceShader);

        housesInstancer->drawInstances(instanceShader);
        roofsInstancer->drawInstances(instanceShader);

        lightlessShader.use();
        lightlessShader.setUniformMat4("view", view);
        lightlessShader.setUniformMat4("projection", projection);

        pointLightModel->draw(lightlessShader);
        directionalLightModel->draw(lightlessShader);
        spotLightModel->draw(lightlessShader);
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
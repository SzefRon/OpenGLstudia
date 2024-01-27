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
#include "Cubemap.h"
#include "DoorNode.h"
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
bool steeringMode = false, follow = true;

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

    if (!follow && !steeringMode)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_RIGHT_SHIFT) {
            if (steeringMode)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouse = true;
            }
            steeringMode ^= true;
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
    Shader skyboxShader("..\\res\\shaders\\skybox.vert", "..\\res\\shaders\\skybox.frag");
    Shader reflectiveShader("..\\res\\shaders\\texture.vert", "..\\res\\shaders\\reflective.frag");
    Shader refractiveShader("..\\res\\shaders\\texture.vert", "..\\res\\shaders\\refractive.frag");
    Shader instanceReflectiveShader("..\\res\\shaders\\instances.vert", "..\\res\\shaders\\reflective.frag");
    Shader colorShader("..\\res\\shaders\\instances.vert", "..\\res\\shaders\\color.frag");

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

    bool polygonMode = false, vsyncMode = true;
    double lastFrame = 0.0, currFrame = 0.0f;
    float doorRotation = 0.0f;
    float acceleration = 0.0f, velocity = 0.0f;
    bool brake = false, snap = true;
    const int cameraCount = 2;
    bool *cameraMode = new bool[cameraCount];
    for (int i = 0; i < cameraCount; i++) cameraMode[i] = false;
    cameraMode[0] = true;

    std::vector<DoorNode *> doors;

    // Scene preparation

    const int noInstances = 10000;
    float offsetX = 8.0f;
    float offsetZ = 4.0f;
    glm::mat4 *houseMatrices = new glm::mat4[noInstances];
    glm::mat4 *roofMatrices = new glm::mat4[noInstances];
    glm::mat4 *floorMatrices = new glm::mat4[noInstances / 2];
    glm::mat4 *railMatrices = new glm::mat4[noInstances / 2];
    glm::mat4 *poleMatrices = new glm::mat4[noInstances / 4];

    std::deque<InstanceNode *> houseNodes;
    std::deque<InstanceNode *> roofNodes;

    GraphNode *mainNode = new GraphNode();
        vModel *houseModel = new vModel("..\\res\\models\\house\\house.obj", 1.0f);
        vModel *roofModel = new vModel("..\\res\\models\\roof\\roof.obj", 1.0f);
        vModel *floorModel = new vModel("..\\res\\models\\grass-floor\\grass-floor.obj", 1.0f);
        vModel *railModel = new vModel("..\\res\\models\\tramrail.obj", 1.0f);
        vModel *poleModel = new vModel("..\\res\\models\\trampole.obj", 1.0f);
        
        Instancer *housesInstancer = new Instancer(houseModel, noInstances, houseMatrices);
        Instancer *roofsInstancer = new Instancer(roofModel, noInstances, roofMatrices);
        Instancer *floorsInstancer = new Instancer(floorModel, noInstances / 2, floorMatrices);
        Instancer *railsInstancer = new Instancer(railModel, noInstances / 2, railMatrices);
        Instancer *polesInstancer = new Instancer(poleModel, noInstances / 4, poleMatrices);

        for (int i = 0; i < noInstances / 2; i++) {
            GraphNode *neighboursNode = new GraphNode();
            mainNode->addChild(neighboursNode);
            neighboursNode->setTranslation(glm::vec3(i * offsetX, 0.0f, 0.0f));
                InstanceNode *floorNode = new InstanceNode(floorMatrices[i], i, floorsInstancer);
                neighboursNode->addChild(floorNode);
                floorNode->setScale(glm::vec3(offsetX * 0.5f, 1.0f, offsetZ + 4.0f));
                for (int j = 0; j < 2; j++) {
                    int index = i * 2 + j;
                    int sign = j == 0 ? 1 : -1;
                    InstanceNode *houseNode = new InstanceNode(houseMatrices[index], index, housesInstancer);
                    neighboursNode->addChild(houseNode);
                    houseNode->setTranslation(glm::vec3(0.0f, 1.0f, sign * offsetZ));
                        InstanceNode *roofNode = new InstanceNode(roofMatrices[index], index, roofsInstancer);
                        houseNode->addChild(roofNode);
                        roofNode->setTranslation(glm::vec3(0.0f, 2.0f, 0.0f));
                }
                InstanceNode *railNode = new InstanceNode(railMatrices[i], i, railsInstancer);
                neighboursNode->addChild(railNode);
                railNode->setRotation(glm::vec3(0.0f, 3.14159f * 0.5f, 0.0f));
                railNode->setScale(glm::vec3(0.6f, 1.0f, offsetX * 0.5f));
        }
        InstanceNode *firstPoleNode;
        for (int i = 0; i < noInstances / 4; i++) {
            InstanceNode *poleNode = new InstanceNode(poleMatrices[i], i, polesInstancer);
            if (i == 0) firstPoleNode = poleNode;
            mainNode->addChild(poleNode);
            poleNode->setTranslation(glm::vec3(i * offsetX * 2.0f - 3.5f, 0.0f, 0.0f));
            poleNode->setRotation(glm::vec3(0.0f, -3.14159f * 0.5f, 0.0f));
            poleNode->setScale(glm::vec3(2.0f, 1.69f, 9.2f));
        }

        DirectionalLight *directionalLight = new DirectionalLight(glm::vec3(1.0f, 1.0f, 0.5f));
        mainNode->addChild(directionalLight);
            vModel *directionalLightModel = new vModel("..\\res\\models\\arrow\\arrow.obj", 1.0f);
            directionalLight->addChild(directionalLightModel);
        directionalLight->setTranslation(glm::vec3(0.0f, 10.0f, 0.0f));
        directionalLight->setRotation(glm::vec3(2.0f, 0.0f, -0.5f));

        GraphNode *tramNode = new GraphNode();
        mainNode->addChild(tramNode);
            GraphNode *cameraNode1 = new GraphNode();
            tramNode->addChild(cameraNode1);
            cameraNode1->setTranslation(glm::vec3(4.0f, 6.0f, 6.0f));

            vModel *tramModel = new vModel("..\\res\\models\\tram.obj", 1.0f);
            tramNode->addChild(tramModel);
            tramModel->setTranslation(glm::vec3(0.0f, 0.25f, 0.0f));
            tramModel->setRotation(glm::vec3(0.0f, 0.5f * 3.14159f, 0.0f));
            tramModel->setScale(glm::vec3(4.0f));
                GraphNode *tramCenter = new GraphNode();
                tramModel->addChild(tramCenter);
                tramCenter->setTranslation(glm::vec3(0.0f, 0.3f, 0.0f));
                    GraphNode *cameraNode2 = new GraphNode();
                    tramCenter->addChild(cameraNode2);
                    cameraNode2->setTranslation(glm::vec3(0.0f, 0.0f, 0.55f));

                glm::mat4 *tramDoorMatrices = new glm::mat4[14];
                vModel *tramDoor = new vModel("..\\res\\models\\tramdoor.obj", 1.0f);
                Instancer *tramDoorInstancer = new Instancer(tramDoor, 14, tramDoorMatrices);
                    doors.push_back(DoorNode::create(3, tramDoorMatrices, 0, tramDoorInstancer, -0.1f, false));
                    tramModel->addChild(doors[0]);
                    doors[0]->setTranslation(glm::vec3(-0.17f, 0.09f, -0.226f));

                    doors.push_back(DoorNode::create(2, tramDoorMatrices, 3, tramDoorInstancer, -0.1f, true));
                    tramModel->addChild(doors[1]);
                    doors[1]->setTranslation(glm::vec3(-0.17f, 0.09f, 0.215f));
                    doors[1]->setRotation(glm::vec3(0.0f, 3.14159f, 0.0f));
                    doors[1]->setScale(glm::vec3(1.0f, 1.0f, 0.75f));

                    doors.push_back(DoorNode::create(2, tramDoorMatrices, 5, tramDoorInstancer, -0.1f, false));
                    tramModel->addChild(doors[2]);
                    doors[2]->setTranslation(glm::vec3(-0.17f, 0.09f, 0.515f));
                    doors[2]->setScale(glm::vec3(1.0f, 1.0f, 0.75f));

                    doors.push_back(DoorNode::create(3, tramDoorMatrices, 7, tramDoorInstancer, -0.1f, true));
                    tramModel->addChild(doors[3]);
                    doors[3]->setTranslation(glm::vec3(0.17f, 0.09f, -0.226f));

                    doors.push_back(DoorNode::create(2, tramDoorMatrices, 10, tramDoorInstancer, -0.1f, false));
                    tramModel->addChild(doors[4]);
                    doors[4]->setTranslation(glm::vec3(0.17f, 0.09f, 0.215f));
                    doors[4]->setRotation(glm::vec3(0.0f, 3.14159f, 0.0f));
                    doors[4]->setScale(glm::vec3(1.0f, 1.0f, 0.75f));

                    doors.push_back(DoorNode::create(2, tramDoorMatrices, 12, tramDoorInstancer, -0.1f, true));
                    tramModel->addChild(doors[5]);
                    doors[5]->setTranslation(glm::vec3(0.17f, 0.09f, 0.515f));
                    doors[5]->setScale(glm::vec3(1.0f, 1.0f, 0.75f));

    mainNode->updateSelfChildren(deltaTime);

    std::vector<std::string> faces;
    faces.push_back("..\\res\\textures\\skybox\\right.jpg");
    faces.push_back("..\\res\\textures\\skybox\\left.jpg");
    faces.push_back("..\\res\\textures\\skybox\\top.jpg");
    faces.push_back("..\\res\\textures\\skybox\\bottom.jpg");
    faces.push_back("..\\res\\textures\\skybox\\front.jpg");
    faces.push_back("..\\res\\textures\\skybox\\back.jpg");
    Cubemap *cubemap = new Cubemap(faces);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
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
        if (steeringMode) {
            ImGui::SetWindowSize(ImVec2(300.0f, 100.0f));
            ImGui::Begin("Hello darkness my old friend!");

            if (ImGui::CollapsingHeader("Tram steering")) {
                ImGui::Indent(20.0f);
                if (ImGui::SliderFloat("Doors control", &doorRotation, -1.4f, 0.0f)) {
                    for (auto &door : doors) {
                        door->rotateOffset = doorRotation;
                        door->makeDirty();
                    }
                    mainNode->updateSelfChildren(deltaTime);
                }
                if (ImGui::SliderFloat("Acceleration", &acceleration, -1.0f, 1.0f)) {}
                if (ImGui::Checkbox("Brake", &brake)) {}
                ImGui::Indent(-20.0f);
            }
            
            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::Indent(20.0f);
                for (int i = 0; i < cameraCount; i++) {
                    if (ImGui::Checkbox(("Camera " + std::to_string(i + 1)).c_str(), &cameraMode[i])) {
                        if (!cameraMode[i]) cameraMode[i] = true;
                    }
                    if (cameraMode[i]) {
                        for (int j = 0; j < cameraCount; j++) {
                            if (j != i) {
                                cameraMode[j] = false;
                            }
                        }
                    }
                    if (i < cameraCount - 1) {
                        ImGui::SameLine();
                    }
                }
                if (ImGui::Checkbox("Follow tram", &follow)) {}
                if (ImGui::Checkbox("Snap to tram", &snap)) {}
                float *cameraPos[3] = {&cameraNode1->translation.x, &cameraNode1->translation.y, &cameraNode1->translation.z};
                if (ImGui::SliderFloat3("Camera 1 position", *cameraPos, -10.0f, 10.0f)) {}
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

        // pointLight->setTranslation(glm::vec3(noInstancesSqrd * 1.5f * glm::sin(lastFrame * 0.1f) + noInstancesSqrd * 1.5f, pointLight->translation.y, noInstancesSqrd * 1.5f * glm::cos(lastFrame * 0.1f) + noInstancesSqrd * 1.5f));

        if (!snap && !steeringMode)
            processInput(window);

        if (brake) {
            int sign = velocity >= 0.0f ? 1 : -1;
            velocity -= sign * 5.0f * deltaTime;
            if ((sign == 1 && velocity < 0.0f) || (sign == -1 && velocity > 0.0f)) {
                velocity = 0.0f;
            }
        }
        else {
            velocity += acceleration * deltaTime;
        }

        tramNode->setTranslation(glm::vec3(
            tramNode->translation.x + velocity * deltaTime,
            tramNode->translation.y,
            tramNode->translation.z
        ));

        mainNode->updateSelfChildren(deltaTime);

        if (snap) {
            if (cameraMode[0]) camera.Position = cameraNode1->model[3];
            else if (cameraMode[1]) camera.Position = cameraNode2->model[3];
        }
        if (follow) {
            glm::vec3 tramCenterPos = tramCenter->model[3];
            glm::vec3 viewDir = glm::normalize(tramCenterPos - camera.Position);
            camera.Yaw = glm::degrees(atan2(viewDir.z, viewDir.x));
            camera.Pitch = glm::degrees(atan2(viewDir.y, sqrt(viewDir.x * viewDir.x + viewDir.z * viewDir.z)));
        }
        camera.updateCameraVectors();

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

        // SkyBox

        glDepthMask(GL_FALSE);

        skyboxShader.use();
        glm::mat4 viewPos = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setUniformMat4("view", viewPos);
        skyboxShader.setUniformMat4("projection", projection);

        cubemap->draw();

        glDepthMask(GL_TRUE);

        // Textured

        textureShader.use();
        textureShader.setUniformMat4("view", view);
        textureShader.setUniformMat4("projection", projection);

        // pointLight->useLight(textureShader);
        directionalLight->useLight(textureShader);
        // spotLight->useLight(textureShader);
        // spotLight2->useLight(textureShader);

        // grassFloor->draw(textureShader);

        // Instanced

        instanceShader.use();
        instanceShader.setUniformMat4("view", view);
        instanceShader.setUniformMat4("projection", projection);
        instanceShader.setUniform3fv("viewPos", camera.Position);

        // pointLight->useLight(instanceShader);
        directionalLight->useLight(instanceShader);
        // spotLight->useLight(instanceShader);
        // spotLight2->useLight(instanceShader);

        housesInstancer->drawInstances(instanceShader);
        roofsInstancer->drawInstances(instanceShader);
        floorsInstancer->drawInstances(instanceShader);

        // Colored instanced

        colorShader.use();
        colorShader.setUniformMat4("view", view);
        colorShader.setUniformMat4("projection", projection);
        colorShader.setUniform3fv("viewPos", camera.Position);

        directionalLight->useLight(colorShader);

        glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        colorShader.setUniform4fv("color", color);
        railsInstancer->drawInstances(colorShader);

        color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        colorShader.setUniform4fv("color", color);
        polesInstancer->drawInstances(colorShader);

        // Lightless

        lightlessShader.use();
        lightlessShader.setUniformMat4("view", view);
        lightlessShader.setUniformMat4("projection", projection);

        // pointLightModel->draw(lightlessShader);
        // directionalLightModel->draw(lightlessShader);
        // spotLightModel->draw(lightlessShader);
        // spotLightModel2->draw(lightlessShader);
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        // Reflectives

        reflectiveShader.use();
        reflectiveShader.setUniformMat4("view", view);
        reflectiveShader.setUniformMat4("projection", projection);
        reflectiveShader.setUniform3fv("viewPos", camera.Position);

        // Refractives

        refractiveShader.use();
        refractiveShader.setUniformMat4("view", view);
        refractiveShader.setUniformMat4("projection", projection);
        refractiveShader.setUniform3fv("viewPos", camera.Position);

        color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        refractiveShader.setUniform4fv("color", color);
        directionalLightModel->draw(refractiveShader);

        color = glm::vec4(1.0f, 1.0f, 0.0f, 0.1f);
        refractiveShader.setUniform4fv("color", color);
        tramModel->draw(refractiveShader);

        // Instance refractives

        instanceReflectiveShader.use();
        instanceReflectiveShader.setUniformMat4("view", view);
        instanceReflectiveShader.setUniformMat4("projection", projection);
        instanceReflectiveShader.setUniform3fv("viewPos", camera.Position);

        color = glm::vec4(1.0f, 1.0f, 0.0f, 0.1f);
        instanceReflectiveShader.setUniform4fv("color", color);
        tramDoorInstancer->drawInstances(instanceReflectiveShader);

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
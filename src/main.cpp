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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

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
    Shader colorShader("..\\res\\shaders\\color.vert", "..\\res\\shaders\\color.frag");

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

    int width, height, nrChannels;
    unsigned char *data = stbi_load("..\\res\\textures\\stone.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    ImVec4 color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float rotateX = 0.0f, rotateY = 3.14159265f * 0.5f, zoom = -30.0f, speed = 1.0f;
    bool polygonMode = false, vsyncMode = true, precisionChanged = false;
    double lastFrame = 0.0, currFrame = 0.0f, deltaTime = 0.0;
    int precision = 10.0f;

    std::vector<GraphNode *> textureModels;
    std::vector<GraphNode *> colorModels;
    std::vector<Cone *> cones;

    GraphNode *planetNode, *planetRotationNode,
              *moonNode, *moonRotationNode,
              *doubleMoonNode, *doubleMoonRotationNode;
    Cone *cone; vModel *model; Cube *cube; Orbit *orbit;

    GraphNode *sunNode = new GraphNode(0.0f, 0.0f, 0.0f, 0.0f);
        // Sun
        GraphNode *sunRotationNode = new GraphNode(0.0f, 1.0f, 0.0f, 0.0f);
        sunNode->addChild(sunRotationNode);
            model = new vModel("..\\res\\models\\sun\\sun.obj", 1.5f);
            textureModels.push_back(model);
            sunRotationNode->addChild(model); 

        // Planet 1
        planetNode = new GraphNode(3.0f, 0.1f, 0.0f, 0.0f);
        sunNode->addChild(planetNode);
        orbit = new Orbit(100, 3.0f);
        sunNode->addChild(orbit);
        colorModels.push_back(orbit);
            planetRotationNode = new GraphNode(0.0f, 1.0f, 0.0f, 1.0f);
            planetNode->addChild(planetRotationNode);
                cone = new Cone(precision, 1.0f, 0.5f);
                colorModels.push_back(cone);
                cones.push_back(cone);
                planetRotationNode->addChild(cone);

            // Moon 1
            moonNode = new GraphNode(0.5f, 1.0f, 0.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, -1.0f, 0.0f, 0.0f);
                moonNode->addChild(moonRotationNode);
                    cone = new Cone(precision, 0.1f, 0.1f);
                    colorModels.push_back(cone);
                    cones.push_back(cone);
                    moonRotationNode->addChild(cone);

            // Moon 2
            moonNode = new GraphNode(1.0f, 2.0f, 1.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, 1.0f, 0.0f, 0.0f);
                moonNode->addChild(moonRotationNode);
                    cone = new Cone(precision, 0.2f, 0.2f);
                    colorModels.push_back(cone);
                    cones.push_back(cone);
                    moonRotationNode->addChild(cone);
        
        // Planet 2
        planetNode = new GraphNode(5.0f, 0.2f, 2.0f, 0.0f);
        sunNode->addChild(planetNode);
            planetRotationNode = new GraphNode(0.0f, 1.0f, 0.0f, 1.0f);
            planetNode->addChild(planetRotationNode);
                model = new vModel("..\\res\\models\\breadorus\\breadorus.obj", 1.0f);
                textureModels.push_back(model);
                planetRotationNode->addChild(model);

            // Moon 1
            moonNode = new GraphNode(1.5f, -2.0f, 1.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, 4.0f, 0.0f, -2.0f);
                moonNode->addChild(moonRotationNode);
                    model = new vModel("..\\res\\models\\breadorus\\breadorus.obj", 0.5f);
                    textureModels.push_back(model);
                    moonRotationNode->addChild(model);

                // Moon moon 1
                doubleMoonNode = new GraphNode(0.5f, -5.0f, 1.0f, 0.0f);
                moonNode->addChild(doubleMoonNode);
                    doubleMoonRotationNode = new GraphNode(0.0f, -2.0f, 0.0f, 3.0f);
                    doubleMoonNode->addChild(doubleMoonRotationNode);
                        model = new vModel("..\\res\\models\\breadorus\\breadorus.obj", 0.2f);
                        textureModels.push_back(model);
                        doubleMoonRotationNode->addChild(model);

        // Planet 3
        planetNode = new GraphNode(9.0f, 0.1f, 0.3f, 0.0f);
        sunNode->addChild(planetNode);
            planetRotationNode = new GraphNode(0.0f, 0.1f, 0.0f, -1.0f);
            planetNode->addChild(planetRotationNode);
                cube = new Cube(1.0f);
                colorModels.push_back(cube);
                planetRotationNode->addChild(cube);
            
            // Moon 1
            moonNode = new GraphNode(1.0f, 0.4f, 0.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, 5.0f, 0.0f, -0.1f);
                moonNode->addChild(moonRotationNode);
                    cube = new Cube(0.5f);
                    colorModels.push_back(cube);
                    moonRotationNode->addChild(cube);

        // Planet 4
        planetNode = new GraphNode(9.0f, 0.1f, 1.8f, 0.0f);
        sunNode->addChild(planetNode);
            planetRotationNode = new GraphNode(0.0f, -0.5f, 0.0f, 2.0f);
            planetNode->addChild(planetRotationNode);
                cube = new Cube(1.0f);
                colorModels.push_back(cube);
                planetRotationNode->addChild(cube);
            
            // Moon 1
            moonNode = new GraphNode(0.7f, 0.4f, 0.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, -5.0f, 0.0f, -0.1f);
                moonNode->addChild(moonRotationNode);
                    cube = new Cube(0.3f);
                    colorModels.push_back(cube);
                    moonRotationNode->addChild(cube);

        // Planet 5
        planetNode = new GraphNode(9.0f, 0.1f, 3.5f, 0.0f);
        sunNode->addChild(planetNode);
            planetRotationNode = new GraphNode(0.0f, -0.5f, 0.0f, 2.0f);
            planetNode->addChild(planetRotationNode);
                cube = new Cube(1.0f);
                colorModels.push_back(cube);
                planetRotationNode->addChild(cube);

            // Moon 1
            moonNode = new GraphNode(1.5f, 0.4f, 0.0f, 0.0f);
            planetNode->addChild(moonNode);
                moonRotationNode = new GraphNode(0.0f, 2.0f, 0.0f, -0.1f);
                moonNode->addChild(moonRotationNode);
                    cube = new Cube(0.7f);
                    colorModels.push_back(cube);
                    moonRotationNode->addChild(cube);


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
        {
            ImGui::SetWindowSize(ImVec2(300.0f, 100.0f));
            ImGui::Begin("Hello, motherfuckers!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::SliderFloat("Zoom", &zoom, -30.0f, 0.0f);
            ImGui::SliderAngle("Rotate X", &rotateX, -180.0f, 180.0f);
            ImGui::SliderAngle("Rotate Y", &rotateY, -180.0f, 180.0f);
            ImGui::SliderFloat("Speed", &speed, -3.0f, 3.0f, "%.1f");
            precisionChanged = ImGui::SliderInt("Precision", &precision, 2, 20);

            ImGui::Checkbox("Wireframe Mode", &polygonMode);
            ImGui::Checkbox("VSync", &vsyncMode);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if (polygonMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (vsyncMode) glfwSwapInterval(1);
        else glfwSwapInterval(0);

        if (precisionChanged) {
            for (auto &cone : cones) {
                cone->changePrecision(precision);
            }
        }

        // Rendering

        currFrame = glfwGetTime();
        deltaTime = speed * (currFrame - lastFrame);
        lastFrame = currFrame;

        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec4 vec = glm::vec4(color.x, color.y, color.z, color.w);
        
        glm::mat4 model = glm::mat4(1);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, zoom));
        view = glm::rotate(view, rotateX, glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, rotateY, glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)(display_w) / display_h, 0.1f, 100.0f);

        sunNode->updateSelfChildren(deltaTime);

        colorShader.use();
        colorShader.setUniformMat4("view", view);
        colorShader.setUniformMat4("projection", projection);
        colorShader.setUniform4fv("color", vec);

        for (auto &colorModel : colorModels) {
            colorModel->draw(colorShader);
        }

        textureShader.use();
        textureShader.setUniformMat4("model", model);
        textureShader.setUniformMat4("view", view);
        textureShader.setUniformMat4("projection", projection);

        for (auto &textureModel : textureModels) {
            textureModel->draw(textureShader);
        }
        
        // glBindVertexArray(0); // no need to unbind it every time 
 
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
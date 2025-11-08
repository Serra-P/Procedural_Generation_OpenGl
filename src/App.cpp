#include "App.hh"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Utils.hh"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Une class qui a la class

bool App::Init(int width, int height, const std::string& title)
{
    window_ = std::make_unique<Window>();
    if (!window_->Init(width, height, title))
        return false;

    input_ = std::make_unique<Input>(window_.get());
    camera_ = std::make_unique<Camera>();
    timer_ = std::make_unique<Timer>();

    glEnable(GL_DEPTH_TEST);

    heightmap_ = std::make_shared<Heightmap>(1280);
    heightmap_->Generate();

    terrain_ = std::make_unique<Terrain>(1280, 0.4f);

    postProcessor_ = std::make_unique<PostProcessor>(1280, 720);

    sun_.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
    sun_.color = glm::vec3(1.0f, 0.95f, 0.8f);
    sun_.intensity = 1.0f;

    glfwSetWindowUserPointer(window_->GetHandle(), camera_.get());
    glfwSetMouseButtonCallback(window_->GetHandle(), MouseButtonCallback);
    glfwSetCursorPosCallback(window_->GetHandle(), MouseCallback);
    glfwSetScrollCallback(window_->GetHandle(), ScrollCallback);

    double x, y;
    glfwGetCursorPos(window_->GetHandle(), &x, &y);
    camera_->ProcessMouse(x, y);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_->GetHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    running_ = true;
    return true;
}

void App::Run()
{
    while (running_ && !window_->ShouldClose())
    {
        float dt = timer_->DeltaTime();
        ProcessInput(dt);
        Update(dt);
        Render();
        window_->SwapBuffers();
        window_->PollEvents();
    }
}

void App::ProcessInput(float dt)
{
    if (input_->IsKeyPressed(GLFW_KEY_ESCAPE))
        running_ = false;

    camera_->ProcessInput(input_.get(), dt);

    if (input_->IsKeyPressed(GLFW_KEY_R))
    {
        heightmap_->Generate();
    }

    if (input_->IsKeyPressed(GLFW_KEY_C))
    {
        glm::vec3 pos = camera_->GetPosition();
        std::cout << "Camera: (" << pos.x << ", " << pos.y << ", " << pos.z
                  << ")" << std::endl;
    }

    if (input_->IsKeyPressed(GLFW_KEY_P))
    {
        enablePostProcessor_ = !enablePostProcessor_;
    }

    if (input_->IsKeyPressed(GLFW_KEY_G))
    {
        enableGrass_ = !enableGrass_;
    }

    if (input_->IsKeyPressed(GLFW_KEY_B))
    {
        enableRock_ = !enableRock_;
    }
}

void App::Update(float dt)
{}

void App::Render()
{
    if (enablePostProcessor_)
        postProcessor_->Bind();

    glClearColor(0.33f, 0.61f, 0.72f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 view = camera_->GetViewMatrix();
    glm::mat4 proj = camera_->GetProjectionMatrix(1280.0f / 720.0f);
    glm::vec3 camPos = camera_->GetPosition();

    terrain_->Draw(heightmap_, view, proj, camPos, sun_, enableGrass_,
                   enableRock_, enableTime_, enableHeight_);

    if (enablePostProcessor_)
        postProcessor_->Apply(heightmap_);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Enable sparkles");
    ImGui::Checkbox("PostProcessor", &enablePostProcessor_);
    ImGui::Checkbox("Grass", &enableGrass_);
    ImGui::Checkbox("Rock", &enableRock_);
    ImGui::Checkbox("ZA WARUDO", &enableTime_);
    ImGui::Checkbox("Gustave Eiffel", &enableHeight_);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::Shutdown()
{
    postProcessor_->Shutdown();
    window_->Shutdown();
}

#include "Window.hh"

#include <iostream>
#include <string>

#include "Logger.hh"

bool Window::Init(int width, int height, const std::string& title)
{
    if (!glfwInit())
    {
        Logger::Get().Log("Failed to init GLFW\n", Logger::Level::ERROR);
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_)
    {
        Logger::Get().Log("Failed to create window\n", Logger::Level::ERROR);
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        Logger::Get().Log("Failed to init GLEW\n", Logger::Level::ERROR);
        return false;
    }

    glViewport(0, 0, width, height);
    width_ = width;
    height_ = height;
    return true;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(window_);
}
void Window::SwapBuffers() const
{
    glfwSwapBuffers(window_);
}
void Window::PollEvents() const
{
    glfwPollEvents();
}
void Window::Shutdown()
{
    glfwTerminate();
}

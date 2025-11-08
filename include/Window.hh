#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
    Window() = default;
    ~Window() = default;

    bool Init(int width, int height, const std::string& title);
    void Shutdown();

    bool ShouldClose() const;
    void SwapBuffers() const;
    void PollEvents() const;

    GLFWwindow* GetHandle() const
    {
        return window_;
    }

private:
    GLFWwindow* window_ = nullptr;
    int width_, height_;
};

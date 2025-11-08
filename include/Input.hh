#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input
{
public:
    explicit Input(class Window* window);
    bool IsKeyPressed(int key) const;

private:
    GLFWwindow* window_;
};

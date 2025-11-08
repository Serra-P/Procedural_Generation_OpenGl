#include "Input.hh"

#include "Window.hh"

Input::Input(Window* window)
{
    window_ = window->GetHandle();
}

bool Input::IsKeyPressed(int key) const
{
    return glfwGetKey(window_, key) == GLFW_PRESS;
}

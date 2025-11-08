#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>

class Timer
{
public:
    Timer()
    {
        last_ = std::chrono::high_resolution_clock::now();
    }

    float GetTime() const
    {
        return currentTime_;
    }

    float DeltaTime()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff = now - last_;
        last_ = now;
        float now2 = glfwGetTime();
        currentTime_ = now2;
        return diff.count();
    }

private:
    std::chrono::high_resolution_clock::time_point last_;
    float currentTime_ = 0.0f;
};

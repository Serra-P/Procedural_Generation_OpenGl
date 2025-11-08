#include "Camera.hh"

#include <GLFW/glfw3.h>

#include "Input.hh"

Camera::Camera()
    : position_(0.0f, 10.0f, 3.0f)
    , front_(0.0f, 0.0f, -1.0f)
    , up_(0.0f, 1.0f, 0.0f)
    , right_(1.0f, 0.0f, 0.0f)
    , worldUp_(0.0f, 1.0f, 0.0f)
    , yaw_(-90.0f)
    , pitch_(-8.0f)
    , speed_(5.0f)
    , sensitivity_(0.1f)
    , fov_(45.0f)
{
    UpdateVectors();
}

void Camera::ProcessInput(Input* input, float dt)
{
    float velocity = speed_ * dt;

    if (input->IsKeyPressed(GLFW_KEY_W))
        position_ += front_ * velocity;
    if (input->IsKeyPressed(GLFW_KEY_S))
        position_ -= front_ * velocity;
    if (input->IsKeyPressed(GLFW_KEY_A))
        position_ -= right_ * velocity;
    if (input->IsKeyPressed(GLFW_KEY_D))
        position_ += right_ * velocity;

    if (input->IsKeyPressed(GLFW_KEY_SPACE))
        position_ += worldUp_ * velocity;
    if (input->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        position_ -= worldUp_ * velocity;
}

void Camera::ProcessMouse(double xpos, double ypos)
{
    if (firstMouse_)
    {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouse_ = false;
    }

    float xoffset = static_cast<float>(xpos - lastX_);
    float yoffset = static_cast<float>(lastY_ - ypos);
    lastX_ = xpos;
    lastY_ = ypos;

    xoffset *= sensitivity_;
    yoffset *= sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
    UpdateVectors();
}

void Camera::ProcessScroll(double yoffset)
{
    fov_ -= yoffset;
    if (fov_ < 20.0f)
        fov_ = 20.0f;
    if (fov_ > 80.0f)
        fov_ = 80.0f;
}

void Camera::UpdateVectors()
{
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    dir.y = sin(glm::radians(pitch_));
    dir.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(dir);

    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::GetProjectionMatrix(float aspect) const
{
    return glm::perspective(glm::radians(fov_), aspect, 0.1f, 200.0f);
}

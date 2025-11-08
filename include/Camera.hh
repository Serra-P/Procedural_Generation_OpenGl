#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Input;

class Camera
{
public:
    Camera();

    void ProcessInput(Input* input, float dt);
    void ProcessMouse(double xpos, double ypos);
    void ProcessScroll(double yoffset);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect) const;

    glm::vec3 GetPosition() const
    {
        return position_;
    }
    void SetPosition(const glm::vec3& pos)
    {
        position_ = pos;
    }

    void SetPitch(const float pitch)
    {
        pitch_ = pitch;
    }

private:
    void UpdateVectors();

private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;

    float yaw_;
    float pitch_;
    float speed_;
    float sensitivity_;
    float fov_;

    bool firstMouse_ = true;
    double lastX_ = 0.0;
    double lastY_ = 0.0;
};

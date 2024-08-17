#pragma once
#include "vulkanBase/VulkanUtil.h"
#include <chrono>
#include <glm/fwd.hpp>

constexpr float PI = 3.14159265358979323846f;
constexpr float TO_RADIANS = PI / 180.0f;

class Camera
{
public:
    Camera() = default;
    Camera(const glm::vec3& origin, float fovAngle, float windowWidth, float windowHeight);
    ~Camera() = default;

    void update(GLFWwindow* window);
    void onMouseMove(double xpos, double ypos, float& lastX, float& lastY);
    glm::mat4 getViewProjection(float nearPlane, float farPlane);
    glm::mat4 getOrthoProjectionMatrix();
    void setAspectRatio(float aspectRatio);

    float getElapsedSec();
    glm::vec3 getOrigin();
private:
    void updateElapsedTime();
    glm::mat4 calculateCameraToWorld() const;
    glm::mat4 getViewMatrix() const;

    glm::vec3 m_Origin = { 2, 2, -2 };
    float m_FovAngle = 90.f;

    glm::vec3 m_Forward = { 0, 0, 1 };
    glm::vec3 m_Up = { 0, 1, 0 };
    glm::vec3 m_Right = { 1, 0, 0 };

    float m_TotalPitch = 0.f;
    float m_TotalYaw = 0.f;

    int m_CameraSpeedFactor = 2;

    float m_OrthoSize{ 1.0f };
    float m_AspectRatio{ WIDTH / HEIGHT };

    std::chrono::high_resolution_clock::time_point m_LastTime;
    float m_ElapsedSec = 0.0f;
};
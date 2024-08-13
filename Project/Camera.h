#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>


#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

constexpr float PI = 3.14159265358979323846f;
constexpr float TO_RADIANS = PI / 180.0f;


class Camera
{
public:
    Camera() = default;

    Camera(const glm::vec3& _origin, float _fovAngle, float windowWidth, float windowHeight) :
        origin{ _origin },
        fovAngle{ _fovAngle },
        m_OrthoSize(1.0f),
        m_AspectRatio(windowWidth / windowHeight)
    {
        lastTime = std::chrono::high_resolution_clock::now();
    }

    glm::vec3 origin = { 2, 2, -2 };
    float fovAngle = 90.f;

    glm::vec3 forward = { 0, 0, 1 };
    glm::vec3 up = { 0, 1, 0 };
    glm::vec3 right = { 1, 0, 0 };

    float totalPitch = 0.f;
    float totalYaw = 0.f;

    int cameraSpeedFactor = 2;

    float m_OrthoSize{ 1.0f };
    float m_AspectRatio{ 1200.0f / 1000.0f };

    std::chrono::high_resolution_clock::time_point lastTime;
    float elapsedSec = 0.0f;

    void updateElapsedTime()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration<float>(currentTime - lastTime);
        elapsedSec = duration.count();

        constexpr float maxDeltaTime = 0.1f;
        if (elapsedSec > maxDeltaTime) {
            elapsedSec = maxDeltaTime;
        }

        lastTime = currentTime;
    }

    glm::mat4 calculateCameraToWorld()
    {
        return glm::mat4(
            glm::vec4(right, 0.0f),
            glm::vec4(up, 0.0f),
            glm::vec4(-forward, 0.0f),
            glm::vec4(origin, 1.0f)
        );
    }


    void onMouseMove(double xpos, double ypos, float& lastX, float& lastY)
    {
        float dx = float(xpos) - lastX;
        float dy = float(ypos) - lastY;

        lastX = float(xpos);
        lastY = float(ypos);

        const float sensitivity = 0.1f;

        totalYaw -= dx * sensitivity;
        totalPitch -= dy * sensitivity;

        totalPitch = std::clamp(totalPitch, -89.0f, 89.0f);

        const glm::mat4x4 pitchYawRotation = glm::yawPitchRoll(glm::radians(totalYaw), glm::radians(totalPitch), 0.0f);
        right = glm::normalize(glm::vec3(pitchYawRotation[0]));
        up = glm::normalize(glm::vec3(pitchYawRotation[1]));
        forward = glm::normalize(glm::vec3(-pitchYawRotation[2]));
    }

    void update(GLFWwindow* window)
    {
        updateElapsedTime();

        float baseSpeed = 500.0f;
        float acceleration = 100.0f;
        float adjustedSpeed = baseSpeed * cameraSpeedFactor * elapsedSec;

        glm::vec3 movement = glm::vec3(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            movement += forward * adjustedSpeed * acceleration;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            movement -= forward * adjustedSpeed * acceleration;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            movement += right * adjustedSpeed * acceleration;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            movement -= right * adjustedSpeed * acceleration;
        }

        glm::vec3 targetPosition = origin + movement;

        float lerpFactor = 0.1f * elapsedSec;
        origin = glm::mix(origin, targetPosition, lerpFactor);
    }

    glm::mat4 GetViewProjection(float nearPlane, float farPlane)
    {
        glm::mat4 projection = glm::perspective(glm::radians(fovAngle), m_AspectRatio, nearPlane, farPlane);
        projection[1][1] *= -1;
        glm::mat4 view = GetViewMatrix();
        glm::mat4 viewProjection = projection * view;

        return viewProjection;
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(origin, origin + forward, up);
    }

    glm::mat4 GetOrthoProjectionMatrix()
    {
        return glm::ortho(
            -m_OrthoSize * m_AspectRatio, m_OrthoSize * m_AspectRatio,
            -m_OrthoSize, m_OrthoSize,
            -1.0f, 1.0f);
    }

    void SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
    }
};
#include "Camera.h"
#include <algorithm>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(const glm::vec3& origin, float fovAngle, float windowWidth, float windowHeight) :
    m_Origin{ origin },
    m_FovAngle{ fovAngle },
    m_AspectRatio(windowWidth / windowHeight)
{
    m_LastTime = std::chrono::high_resolution_clock::now();
}

void Camera::updateElapsedTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<float>(currentTime - m_LastTime);
    m_ElapsedSec = duration.count();

    constexpr float maxDeltaTime = 0.1f;
    if (m_ElapsedSec > maxDeltaTime) {
        m_ElapsedSec = maxDeltaTime;
    }

    m_LastTime = currentTime;
}

glm::mat4 Camera::calculateCameraToWorld() const
{
    return glm::mat4(
        glm::vec4(m_Right, 0.0f),
        glm::vec4(m_Up, 0.0f),
        glm::vec4(-m_Forward, 0.0f),
        glm::vec4(m_Origin, 1.0f)
    );
}

void Camera::onMouseMove(GLFWwindow* window, double xpos, double ypos, float& lastX, float& lastY)
{
    float dx = float(xpos) - lastX;
    float dy = float(ypos) - lastY;

    lastX = float(xpos);
    lastY = float(ypos);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        updateLightDirection(dx, dy);
    }
    else {
        const float sensitivity = 0.1f;

        m_TotalYaw -= dx * sensitivity;
        m_TotalPitch -= dy * sensitivity;

        m_TotalPitch = std::clamp(m_TotalPitch, -89.0f, 89.0f);

        const glm::mat4x4 pitchYawRotation = glm::yawPitchRoll(glm::radians(m_TotalYaw), glm::radians(m_TotalPitch), 0.0f);
        m_Right = glm::normalize(glm::vec3(pitchYawRotation[0]));
        m_Up = glm::normalize(glm::vec3(pitchYawRotation[1]));
        m_Forward = glm::normalize(glm::vec3(-pitchYawRotation[2]));
    }
}

void Camera::updateLightDirection(float dx, float dy) {
    float sensitivity = 0.005f;

    m_TotalYaw -= dx * sensitivity;
    m_TotalPitch -= dy * sensitivity;

    glm::vec3 direction;
    direction.x = cos(m_TotalYaw) * cos(m_TotalPitch);
    direction.y = sin(m_TotalPitch);
    direction.z = sin(m_TotalYaw) * cos(m_TotalPitch);

    m_LightDirection = glm::normalize(direction);
}

void Camera::update(GLFWwindow* window)
{
    updateElapsedTime();

    float baseSpeed = 500.0f;
    float acceleration = 100.0f;
    float adjustedSpeed = baseSpeed * m_CameraSpeedFactor * m_ElapsedSec;

    glm::vec3 movement = glm::vec3(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movement += m_Forward * adjustedSpeed * acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movement -= m_Forward * adjustedSpeed * acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        movement += m_Right * adjustedSpeed * acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        movement -= m_Right * adjustedSpeed * acceleration;
    }

    glm::vec3 targetPosition = m_Origin + movement;

    float lerpFactor = 0.1f * m_ElapsedSec;
    m_Origin = glm::mix(m_Origin, targetPosition, lerpFactor);
}

glm::mat4 Camera::getViewProjection(float nearPlane, float farPlane)
{
    glm::mat4 projection = glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, nearPlane, farPlane);
    projection[1][1] *= -1;
    glm::mat4 view = getViewMatrix();
    return projection * view;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_Origin, m_Origin + m_Forward, m_Up);
}

glm::mat4 Camera::getOrthoProjectionMatrix()
{
    return glm::ortho(
        -m_OrthoSize * m_AspectRatio, m_OrthoSize * m_AspectRatio,
        -m_OrthoSize, m_OrthoSize,
        -1.0f, 1.0f
    );
}

void Camera::setAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
}

float Camera::getElapsedSec()
{
    return m_ElapsedSec;
}

glm::vec3 Camera::getOrigin()
{
    return m_Origin;
}

glm::vec3 Camera::getLightDirection() const {
    return m_LightDirection;
}
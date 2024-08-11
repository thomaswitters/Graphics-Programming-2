#include "Camera.h"

//Camera::Camera(const glm::vec3& position, float fovAngle)
//    : m_Position{ position },
//    m_TargetPosition{ m_Position },
//    m_AspectRatio{ 1.0f }, // Set default aspect ratio to 1.0f
//    m_FovAngle{ fovAngle },
//    m_TargetFovAngle{ fovAngle },
//    m_Pitch{ 0.0f },
//    m_TargetPitch{ 0.0f },
//    m_Yaw{ 0.0f },
//    m_TargetYaw{ 0.0f },
//    m_Right{ 1.0f, 0.0f, 0.0f },
//    m_Up{ 0.0f, 1.0f, 0.0f },
//    m_Forward{ 0.0f, 0.0f, -1.0f }
//{
//}
//
//void Camera::Update(GLFWwindow* window)
//{
//    // Handle mouse input
//    double mouseX, mouseY;
//    glfwGetCursorPos(window, &mouseX, &mouseY);
//
//    if (m_FirstMouse) {
//        m_LastMouseX = static_cast<float>(mouseX);
//        m_LastMouseY = static_cast<float>(mouseY);
//        m_FirstMouse = false;
//    }
//
//    float deltaX = static_cast<float>(mouseX) - m_LastMouseX;
//    float deltaY = m_LastMouseY - static_cast<float>(mouseY); // Reversed since y-coordinates go from bottom to top
//
//    m_LastMouseX = static_cast<float>(mouseX);
//    m_LastMouseY = static_cast<float>(mouseY);
//
//    m_TargetPitch += deltaY * ROTATE_SPEED;
//    m_TargetYaw -= deltaX * ROTATE_SPEED;
//
//    // Handle keyboard input
//    glm::vec3 movementInputVector{};
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        movementInputVector.z += 1;
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        movementInputVector.z -= 1;
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        movementInputVector.x += 1;
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        movementInputVector.x -= 1;
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//        movementInputVector.y += 1;
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//        movementInputVector.y -= 1;
//
//    m_IsBoosting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
//
//    float scrollDelta = m_ScrollDelta;
//    ChangeFovAngle(scrollDelta * -ZOOM_SPEED);
//    m_FovAngle = glm::mix(m_FovAngle, m_TargetFovAngle, m_DeltaTime / ZOOM_LERP_DURATION);
//
//    m_Pitch = glm::mix(m_Pitch, m_TargetPitch, m_DeltaTime / ROTATE_LERP_DURATION);
//    m_Yaw = glm::mix(m_Yaw, m_TargetYaw, m_DeltaTime / ROTATE_LERP_DURATION);
//
//    const glm::mat4 pitchYawRotation = glm::yawPitchRoll(glm::radians(m_Yaw), glm::radians(m_Pitch), 0.0f);
//    m_Right = pitchYawRotation[0];
//    m_Up = pitchYawRotation[1];
//    m_Forward = pitchYawRotation[2];
//
//    movementInputVector = pitchYawRotation * glm::vec4(movementInputVector, 0.0f);
//
//    m_TargetPosition += (movementInputVector * m_DeltaTime * KEY_MOVE_SPEED * (m_IsBoosting ? BOOST_SPEED_MULTIPLIER : 1.0f));
//
//    m_Position = glm::mix(m_Position, m_TargetPosition, m_DeltaTime / MOVE_LERP_DURATION);
//
//    UpdateViewMatrix();
//    UpdateProjectionMatrix();
//
//    // Reset scroll delta for the next frame
//    m_ScrollDelta = 0.0f;
//}
//
//glm::mat4 Camera::GetOrthoProjectionMatrix() const
//{
//    return glm::ortho(-m_OrthoSize * m_AspectRatio, m_OrthoSize * m_AspectRatio, -m_OrthoSize, m_OrthoSize, -1.0f, 1.0f);
//}
//
//void Camera::SetFovAngle(float fovAngle) { m_FovAngle = fovAngle; }
//
//void Camera::SetPosition(glm::vec3 position, bool teleport)
//{
//    m_TargetPosition = position;
//    if (teleport)
//        m_Position = position;
//}
//
//void Camera::SetNearClipping(float value) { m_NearClippingPlane = value; }
//
//void Camera::SetFarClipping(float value) { m_FarClippingPlane = value; }
//
//void Camera::SetPitch(float pitch)
//{
//    m_Pitch = pitch;
//    m_TargetPitch = pitch;
//}
//
//void Camera::SetYaw(float yaw)
//{
//    m_Yaw = yaw;
//    m_TargetYaw = yaw;
//}
//
//void Camera::ChangeFovAngle(float fovAngleChange)
//{
//    m_TargetFovAngle = std::clamp(m_TargetFovAngle + fovAngleChange, MIN_FOV, MAX_FOV);
//}
//
//void Camera::SetAspect(float aspectRatio) { m_AspectRatio = aspectRatio; }
//
//void Camera::UpdateViewMatrix()
//{
//    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
//}
//
//void Camera::UpdateProjectionMatrix()
//{
//    m_ProjectionMatrix = glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
//}
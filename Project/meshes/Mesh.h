#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> // For glm::quat
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, etc.
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr, if needed
#include "Vertex.h"
#include "DescriptorPool.h"
#include "VulkanBufferHandler.h"
#include "CommandBuffer.h"
#include "texture/Material.h"
#include <PhysicsEngine/PhysicsEngine.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class ShapeType {
    Box,
    Sphere
};

template <typename VertexType>
class Mesh {
public:
    void initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue, const std::vector<VertexType> vertices, std::vector<uint32_t> indices);
    void draw(CommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const;
    void cleanUp(const VkDevice& device);

    const std::vector<VertexType>& getVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_Indices; }

    void setVertices(const std::vector<VertexType>& vertices) { m_Vertices = vertices; }
    void setIndices(const std::vector<uint32_t>& indices) { m_Indices = indices; }

    const DescriptorPool& getDescriptorPool() const { return m_DescriptorPool; }

    static  Mesh<Vertex2D> CreateRectangle(glm::vec2 center, float width, float height, const glm::vec3& color);
    static Mesh<Vertex2D> CreateRectangle(glm::vec2 center, float width, float height, const std::vector<glm::vec3>& colors);

    static Mesh<Vertex2D> CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& color, int nrOfVertexes);
    static Mesh<Vertex2D> CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& innerColor, const glm::vec3& outerColor, int nrOfVertexes);


    void createPhysicsBody(PhysicsEngine& physicsEngine, float mass, glm::vec3 m_BoundingBox, ShapeType shapeType);
    void updatePhysics(float deltaTime);
    void applyImpulseOnce(const btVector3& impulse);

    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    std::shared_ptr<Material> m_Material{};
    std::unique_ptr<btRigidBody> m_PhysicsBody = nullptr;
private:
    std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};

    std::vector<VertexType> m_Vertices{};
    std::vector<uint32_t> m_Indices{};

    float m_BoundingBoxWidth{};
    float m_BoundingBoxHeight{};
    float m_BoundingBoxDepth{};

    bool m_ImpulseApplied = false;
};



template <typename VertexType>
void Mesh<VertexType>::initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue, const std::vector<VertexType> vertices, std::vector<uint32_t> indices) {
    m_Vertices = vertices;
    m_Indices = indices;

    size_t vertexBufferSize = sizeof(VertexType) * m_Vertices.size();
    size_t indexBufferSize = sizeof(uint32_t) * m_Indices.size();

    auto stagingVertexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBufferSize
    );

    stagingVertexBuffer->upload(vertexBufferSize, m_Vertices.data());

    m_VertexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBufferSize
    );


    stagingVertexBuffer->copyBuffer(queueFamily, *m_VertexBuffer, graphicsQueue);
    stagingVertexBuffer->destroy(device);

    auto stagingIndexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        indexBufferSize
    );


    stagingIndexBuffer->upload(indexBufferSize, m_Indices.data());

    m_IndexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBufferSize
    );

    stagingIndexBuffer->copyBuffer(queueFamily, *m_IndexBuffer, graphicsQueue);
    stagingIndexBuffer->destroy(device);
}

template <typename VertexType>
void Mesh<VertexType>::draw(CommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const {
    VkBuffer vertexBuffers[] = { m_VertexBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer.getVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer.getVkCommandBuffer(), m_IndexBuffer->getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer.getVkCommandBuffer(), static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

template <typename VertexType>
void Mesh<VertexType>::cleanUp(const VkDevice& device) {
    m_VertexBuffer->destroy(device);
    m_IndexBuffer->destroy(device);
}

template <typename VertexType>
void Mesh<VertexType>::createPhysicsBody(PhysicsEngine& physicsEngine, float mass, glm::vec3 m_BoundingBox, ShapeType shapeType) {
    std::unique_ptr<btCollisionShape> shape;
    m_BoundingBoxWidth = m_BoundingBox.x;
    m_BoundingBoxHeight = m_BoundingBox.y;
    m_BoundingBoxDepth = m_BoundingBox.z;

    if (shapeType == ShapeType::Box) {
        btVector3 halfExtents(m_BoundingBoxWidth / 2, m_BoundingBoxHeight / 2, m_BoundingBoxDepth / 2);
        shape = std::make_unique<btBoxShape>(halfExtents);
    }
    else if (shapeType == ShapeType::Sphere) {
        float radius = m_BoundingBoxWidth / 2;
        shape = std::make_unique<btSphereShape>(radius);
    }

    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, inertia);
    }

    auto motionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), shape.get(), inertia);
    m_PhysicsBody = std::make_unique<btRigidBody>(rbInfo);

    physicsEngine.getDynamicsWorld()->addRigidBody(m_PhysicsBody.get());

    shape.release();
    motionState.release();
}

template <typename VertexType>
void Mesh<VertexType>::applyImpulseOnce(const btVector3& impulse) {
    if (m_PhysicsBody && !m_ImpulseApplied) {
        m_PhysicsBody->applyCentralImpulse(impulse);
        m_ImpulseApplied = true;
    }
}

template <typename VertexType>
void Mesh<VertexType>::updatePhysics(float deltaTime) {
    if (m_PhysicsBody) {
        btTransform trans;
        m_PhysicsBody->getMotionState()->getWorldTransform(trans);

        glm::vec3 scale = glm::vec3(
            glm::length(glm::vec3(m_ModelMatrix[0])),
            glm::length(glm::vec3(m_ModelMatrix[1])),
            glm::length(glm::vec3(m_ModelMatrix[2]))
        );

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()));

        glm::quat rotation(
            trans.getRotation().getW(),
            trans.getRotation().getX(),
            trans.getRotation().getY(),
            trans.getRotation().getZ()
        );
        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        m_ModelMatrix = translationMatrix * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }
}

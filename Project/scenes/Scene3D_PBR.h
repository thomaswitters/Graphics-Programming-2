#pragma once
#include "SceneBase.h"

template <typename VertexType>
class Scene3D_PBR : public SceneBase<VertexType> {
public:
    void createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager);
    void draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode);
    void update(float deltaTime) override;
};

template <typename VertexType>
void Scene3D_PBR<VertexType>::createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) {
    auto texture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_diffuse.png");
    auto texture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_normal.png");
    auto texture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_specular.png");
    auto texture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_gloss.png");

    auto myMaterial = materialManager.createMaterial(device, { texture, texture2, texture3, texture4 });

    auto brickTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_diffuse.png");
    auto brickTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_normal_small.png");
    auto brickTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_specular.png");
    auto brickTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_gloss.png");

    auto myBrickMaterial = materialManager.createMaterial(device, { brickTexture, brickTexture2, brickTexture3, brickTexture4 });

    auto defaultTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/uv_grid_3.png");
    auto defaultTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/defaultNormal.png");
    auto defaultTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/defaultBlack.png");
    auto defaultTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/uv_grid.png");

    auto mydefaultTextureMaterial = materialManager.createMaterial(device, { defaultTexture, defaultTexture2, defaultTexture3, defaultTexture4 });

    auto dirtTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Diffuse_small.png");
    auto dirtTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Normal_small.png");
    auto dirtTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Specular_small.png");
    auto dirtTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Gloss_small.png");

    auto myDirtTextureMaterial = materialManager.createMaterial(device, { dirtTexture, dirtTexture2, dirtTexture3, dirtTexture4 });


    Mesh<VertexType> vehicle;
    std::vector<VertexType> vehicleVertices;
    std::vector<uint32_t> vehicleIndices;

    if (ObjLoader::loadObjFile("models/vehicle.obj", vehicleVertices, vehicleIndices)) {
        vehicle.setVertices(vehicleVertices);
        vehicle.setIndices(vehicleIndices);
        vehicle.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { 0.5f, 0.5f, 0 }) *
            glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });
        vehicle.m_pMaterial = myMaterial;

        addMesh(vehicle, device, physDevice, queueFamily, graphicsQueue);
    }

    Mesh<VertexType> square;
    std::vector<VertexType> squareVertices;
    std::vector<uint32_t> squareIndices;

    if (ObjLoader::loadObjFile("models/square.obj", squareVertices, squareIndices)) {
        square.setVertices(squareVertices);
        square.setIndices(squareIndices);
        square.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -5.5f, 0.5, 0 }) * rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        square.m_pMaterial = myBrickMaterial;

        addMesh(square, device, physDevice, queueFamily, graphicsQueue);
    }


    Mesh<VertexType> sphere2;
    std::vector<VertexType> sphere2Vertices;
    std::vector<uint32_t> sphere2Indices;

    if (ObjLoader::loadObjFile("models/sphere.obj", sphere2Vertices, sphere2Indices)) {

        sphere2.setVertices(sphere2Vertices);
        sphere2.setIndices(sphere2Indices);
        sphere2.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -8.5f, 0.5f, 0 });
        sphere2.m_pMaterial = myDirtTextureMaterial;

        addMesh(sphere2, device, physDevice, queueFamily, graphicsQueue);
    }







    //bouncy ball
    {
        Mesh<VertexType> sphere;
        std::vector<VertexType> sphereVertices;
        std::vector<uint32_t> sphereIndices;


        if (ObjLoader::loadObjFile("models/sphere.obj", sphereVertices, sphereIndices)) {
            sphere.setVertices(sphereVertices);
            sphere.setIndices(sphereIndices);

            btVector3 initialPosition(-10.0f, 30.3f, 7.0f);
            btQuaternion initialRotation(0, 0, 0, 1);
            btTransform initialTransform(initialRotation, initialPosition);

            sphere.createPhysicsBody(physicsEngine, 1.f, glm::vec3(2.0f, 2.0f, 2.0f), ShapeType::Sphere, true, 0.9f);

            sphere.m_pPhysicsBody->setWorldTransform(initialTransform);
            sphere.m_ModelMatrix = glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
            sphere.m_pMaterial = mydefaultTextureMaterial;

            addMesh(sphere, device, physDevice, queueFamily, graphicsQueue);
        }

        Mesh<VertexType> square2;
        std::vector<VertexType> square2Vertices{
        {glm::vec3{-0.5f, 0.0f, -0.5f}, glm::vec3{1.f, 0.f, 1.f}, glm::vec2{0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec3{1.f, 0.f, 0.f}},
        {glm::vec3{0.5f, 0.0f, -0.5f}, glm::vec3{0.f, 1.f, 1.f}, glm::vec2{1.f, 0.f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec3{1.f, 0.f, 0.f}},
        {glm::vec3{0.5f, 0.0f,  0.5f}, glm::vec3{0.f, 0.f, 1.f}, glm::vec2{1.f, 1.f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec3{1.f, 0.f, 0.f}},
        {glm::vec3{-0.5f, 0.0f,  0.5f}, glm::vec3{1.f, 1.f, 0.f}, glm::vec2{0.f, 1.f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec3{1.f, 0.f, 0.f}},
        };

        std::vector<uint32_t> square2Indices{
            0, 1, 2,
            0, 2, 3
        };

        square2.setVertices(square2Vertices);
        square2.setIndices(square2Indices);

        btVector3 initialPosition(-10.0f, 5.0f, 7.0f);
        btQuaternion initialRotation(0, 0, 0, 1);
        btTransform initialTransform(initialRotation, initialPosition);

        square2.createPhysicsBody(physicsEngine, 1000.f, glm::vec3(5.0f, 0.01f, 5.0f), ShapeType::Box, false, 1.0f);
        square2.m_pPhysicsBody->setWorldTransform(initialTransform);

        square2.m_ModelMatrix = glm::scale(glm::mat4(1.0f), { 5.0f, 5.0f, 5.0f });
        square2.m_pMaterial = mydefaultTextureMaterial;

        addMesh(square2, device, physDevice, queueFamily, graphicsQueue);
    }


    // wall and ball
    {
        Mesh<VertexType> sphere;
        std::vector<VertexType> sphereVertices;
        std::vector<uint32_t> sphereIndices;


        if (ObjLoader::loadObjFile("models/sphere.obj", sphereVertices, sphereIndices)) {
            sphere.setVertices(sphereVertices);
            sphere.setIndices(sphereIndices);

            btVector3 initialPosition(-80.5f, 6.3f, 1);
            btQuaternion initialRotation(0, 0, 0, 1);
            btTransform initialTransform(initialRotation, initialPosition);

            sphere.createPhysicsBody(physicsEngine, 10.f, glm::vec3(2.0f, 2.0f, 2.0f), ShapeType::Sphere, false);

            sphere.m_pPhysicsBody->setWorldTransform(initialTransform);
            sphere.m_ModelMatrix = glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
            sphere.m_pMaterial = myDirtTextureMaterial;

            addMesh(sphere, device, physDevice, queueFamily, graphicsQueue);
        }

        const int numberOfCubesPerSide = 5;
        const float cubeSize = 0.1f;
        const float spacing = 0.81f;

        std::vector<VertexType> cubeVertices;
        std::vector<uint32_t> cubeIndices;

        if (ObjLoader::loadObjFile("models/square.obj", cubeVertices, cubeIndices)) {
            for (int x = 0; x < numberOfCubesPerSide; ++x) {
                for (int y = 0; y < numberOfCubesPerSide; ++y) {
                    for (int z = 0; z < numberOfCubesPerSide; ++z) {
                        Mesh<VertexType> smallCube;

                        smallCube.setVertices(cubeVertices);
                        smallCube.setIndices(cubeIndices);

                        float xOffset = x * (cubeSize + spacing);
                        float yOffset = 4.5f + y * (cubeSize + spacing);
                        float zOffset = -1 + z * (cubeSize + spacing);
                        btVector3 initialPosition(xOffset, yOffset, zOffset);
                        btQuaternion initialRotation(0, 0, 0, 1);
                        btTransform initialTransform(initialRotation, initialPosition);

                        smallCube.createPhysicsBody(physicsEngine, 0.7f, glm::vec3(cubeSize * 9.1f, cubeSize * 9.1f, cubeSize * 9.1f), ShapeType::Box, false);
                        smallCube.m_pPhysicsBody->setWorldTransform(initialTransform);

                        smallCube.m_ModelMatrix = glm::scale(glm::mat4(1.0f), { cubeSize, cubeSize, cubeSize });
                        smallCube.m_pMaterial = myBrickMaterial;

                        addMesh(smallCube, device, physDevice, queueFamily, graphicsQueue);
                    }
                }
            }
        }
    }
}

template <typename VertexType>
void Scene3D_PBR<VertexType>::draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode) {
    UniformBufferObject3D_PBR ubo3D{};
    ubo3D.viewProjection = camera.getViewProjection(0.1f, 200.f);
    ubo3D.viewPosition = glm::vec4(camera.getOrigin(), 1.0f);
    ubo3D.lightDirection = camera.getLightDirection();

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

    for (auto& mesh : m_Meshes) {
        PushConstantsPBR meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;
        meshPushConstant.renderMode = renderMode;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        if (mesh.m_pMaterial != nullptr)
        {
            graphicsPipeline.updateMaterial(commandBuffer.getVkCommandBuffer(), *mesh.m_pMaterial);
        }

        mesh.draw(commandBuffer);
    }
}

template <typename VertexType>
void Scene3D_PBR<VertexType>::update(float deltaTime) {
    constexpr float maxSimulationTimestep = 1.0f / 60.0f;
    float simulationTimestep = std::min(deltaTime, maxSimulationTimestep);
    physicsEngine.stepSimulation(simulationTimestep);

    if (!m_Meshes.empty()) {
        Mesh<VertexType>& mesh = m_Meshes[3];
        mesh.updatePhysics(deltaTime);
    }

    if (!m_Meshes.empty()) {
        Mesh<VertexType>& mesh = m_Meshes[4];
        mesh.updatePhysics(deltaTime);
    }

    if (!m_Meshes.empty()) {
        Mesh<VertexType>& mesh = m_Meshes[5];

        const float forceMagnitude = 100.0f;
        mesh.applyImpulseOnce(btVector3(forceMagnitude, 0.0f, 0.0f));
        mesh.updatePhysics(deltaTime);
    }

    float numberOfCubes = 125.0f;
    for (int i = 0; i < numberOfCubes; i++)
    {
        Mesh<VertexType>& mesh = m_Meshes[i + 6];
        mesh.updatePhysics(deltaTime);
    }



    constexpr float rotationSpeed = glm::radians(90.0f);
    m_RotationAngle += rotationSpeed * deltaTime;

    if (m_Meshes.size() > 1) {
        float radiusX = 25.0f;
        float radiusZ = 15.0f;
        float radiusY = 15.0f;
        float speed = 0.8f;
        float time = m_RotationAngle * speed;

        float x = radiusX * cos(time);
        float z = radiusZ * sin(time);
        float y = radiusY * sin(time * 2);

        glm::vec2 direction = glm::normalize(glm::vec2(cos(time), sin(time)));
        float forwardAngle = atan2(direction.y, direction.x);

        float verticalSpeed = radiusY * cos(time * 2);
        float tiltAngle = atan2(verticalSpeed, sqrt(x * x + z * z));

        m_Meshes[0].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-7.5f, 3.5f, 0))
            * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
            * glm::rotate(glm::mat4(1.0f), forwardAngle, glm::vec3(0.0f, -1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), tiltAngle, glm::vec3(0.0f, 0.0f, 1.0f))
            * glm::rotate(glm::mat4(1.0f), m_RotationAngle * 1.7f, glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
    }

    if (!m_Meshes.empty()) {
        m_Meshes[1].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, 0.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(1.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });
    }

    if (!m_Meshes.empty()) {
        m_Meshes[2].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-8.5f, 0.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
    }
}

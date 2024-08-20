# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

file(COPY models DESTINATION .)

set(SOURCES
    "main.cpp"
    "vulkanbase/VulkanBase.h" 
    "vulkanbase/VulkanBase.cpp" 
    "vulkanbase/VulkanUtil.h"
    "vulkanbase/VulkanUtil.cpp"
    "stb/stb_image.h"
    "MachineShader.h" 
    "MachineShader.cpp"    
    "buffers/CommandBuffer.h" 
    "buffers/CommandBuffer.cpp"       
    "loadObjFile.h" 
    "buffers/DataBuffer.h" 
    "buffers/DataBuffer.cpp" 
    "CommandPool.h" 
    "CommandPool.cpp"     
    "Vertex.h"           
    "GraphicsPipeline.h" 
    "GraphicsPipeline.cpp" 
    "DescriptorPool.h" 
    "DescriptorPool.cpp" 
    "VulkanDeviceManager.h" 
    "VulkanDeviceManager.cpp" 
    "SwapChain.h" 
    "SwapChain.cpp" 
    "meshes/Mesh.h" 
    "meshes/Mesh.cpp" 
    "Camera.h" 
    "Camera.cpp" 
    "texture/Texture.h" 
    "texture/Texture.cpp" 
    "texture/Material.h" 
    "texture/Material.cpp" 
    "texture/MaterialManager.h" 
    "texture/MaterialManager.cpp"
    "physicsEngine/PhysicsEngine.h" 
    "physicsEngine/PhysicsEngine.cpp"
    "scenes/SceneBase.h" 
    "scenes/Scene2D.h" 
    "scenes/Scene3D_PBR.h" 
    "scenes/Scene3D.h" 
)

add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Shaders)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw BulletDynamics BulletCollision LinearMath)

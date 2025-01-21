include(cmake/Variables.cmake)

#file(GLOB APPLICATION_HEADER ${APPLICATION_HDR_DIR}/*.h)
#file(GLOB APPLICATION_INLINE ${APPLICATION_HDR_DIR}/*.hpp)
#file(GLOB APPLICATION_SOURCE ${APPLICATION_SRC_DIR}/*.cpp)

file(GLOB ASSETS_SOURCE ${ASSETS_SRC_DIR}/*.cpp)
file(GLOB ASSETS_HEADER ${ASSETS_HDR_DIR}/*.h)
file(GLOB ASSETS_INLINE ${ASSETS_HDR_DIR}/*.hpp)

file(GLOB ASSETS_COMPONENTS_SOURCE ${ASSETS_COMPONENTS_SRC_DIR}/*.cpp)
file(GLOB ASSETS_COMPONENTS_HEADER ${ASSETS_COMPONENTS_HDR_DIR}/*.h)
file(GLOB ASSETS_COMPONENTS_INLINE ${ASSETS_COMPONENTS_HDR_DIR}/*.hpp)

file(GLOB ASSETS_SERIALIZERS_SOURCE ${ASSETS_SERIALIZERS_SRC_DIR}/*.cpp)
file(GLOB ASSETS_SERIALIZERS_HEADER ${ASSETS_SERIALIZERS_HDR_DIR}/*.h)
file(GLOB ASSETS_SERIALIZERS_INLINE ${ASSETS_SERIALIZERS_HDR_DIR}/*.hpp)

file(GLOB ASSETS_GLTF_SOURCE ${ASSETS_GLTF_SRC_DIR}/*.cpp)
file(GLOB ASSETS_GLTF_HEADER ${ASSETS_GLTF_HDR_DIR}/*.h)
file(GLOB ASSETS_GLTF_INLINE ${ASSETS_GLTF_HDR_DIR}/*.hpp)


file(GLOB CAMERA_SOURCE ${CAMERA_SRC_DIR}/*.cpp)
file(GLOB CAMERA_HEADER ${CAMERA_HDR_DIR}/*.h)
file(GLOB CAMERA_INLINE ${CAMERA_HDR_DIR}/*.hpp)

file(GLOB CORE_SOURCE ${CORE_SRC_DIR}/*.cpp)
file(GLOB CORE_HEADER ${CORE_HDR_DIR}/*.h)
file(GLOB CORE_INLINE ${CORE_HDR_DIR}/*.hpp)
#
file(GLOB ENTITY_SOURCE ${ENTITY_SRC_DIR}/*.cpp)
file(GLOB ENTITY_HEADER ${ENTITY_HDR_DIR}/*.h)
file(GLOB ENTITY_INLINE ${ENTITY_HDR_DIR}/*.hpp)
#
file(GLOB EVENT_SOURCE ${EVENT_SRC_DIR}/*.cpp)
file(GLOB EVENT_HEADER ${EVENT_HDR_DIR}/*.h)
file(GLOB EVENT_INLINE ${EVENT_HDR_DIR}/*.hpp)
#
file(GLOB GLA_SOURCE ${GLA_SRC_DIR}/*.cpp)
file(GLOB GLA_HEADER ${GLA_HDR_DIR}/*.h)
file(GLOB GLA_INLINE ${GLA_HDR_DIR}/*.hpp)
#
file(GLOB GLA_VULKAN_SOURCE ${VULKAN_SRC_DIR}/*.cpp)
file(GLOB GLA_VULKAN_HEADER ${VULKAN_HDR_DIR}/*.h)
file(GLOB GLA_VULKAN_INLINE ${VULKAN_HDR_DIR}/*.hpp)

file(GLOB GLA_VULKAN_RAY_TRACE_SOURCE ${VULKAN_RAY_TRACE_SRC_DIR}/*.cpp)
file(GLOB GLA_VULKAN_RAY_TRACE_HEADER ${VULKAN_RAY_TRACE_HDR_DIR}/*.h)
file(GLOB GLA_VULKAN_RAY_TRACE_INLINE ${VULKAN_RAY_TRACE_HDR_DIR}/*.hpp)

file(GLOB GLA_VULKAN_SCENE_SOURCE ${VULKAN_SCENE_SRC_DIR}/*.cpp)
file(GLOB GLA_VULKAN_SCENE_HEADER ${VULKAN_SCENE_HDR_DIR}/*.h)
file(GLOB GLA_VULKAN_SCENE_INLINE ${VULKAN_SCENE_HDR_DIR}/*.hpp)

#
#file(GLOB RENDERER_SOURCE ${RENDERER_SRC_DIR}/*.cpp)
#file(GLOB RENDERER_HEADER ${RENDERER_HDR_DIR}/*.h)
#file(GLOB RENDERER_INLINE ${RENDERER_HDR_DIR}/*.hpp)
#
file(GLOB ROOT_SOURCE ${SRC_DIR}/*.cpp)
file(GLOB ROOT_HEADER ${HDR_DIR}/*.h)
file(GLOB ROOT_INLINE ${HDR_DIR}/*.hpp)

file(GLOB SHARED_HEADERS ${SHADERS_DIR}/*.h ${SHADERS_DIR}/compress.glsl)

file(GLOB SCENE_SOURCE ${SCENE_SRC_DIR}/*.cpp)
file(GLOB SCENE_HEADER ${SCENE_HDR_DIR}/*.h)
file(GLOB SCENE_INLINE ${SCENE_HDR_DIR}/*.hpp)

#file(GLOB STB_SOURCE ${STB_DIR}/*.cpp)
#file(GLOB STB_HEADER ${STB_DIR}/*.h)
#file(GLOB STB_INLINE ${STB_DIR}/*.hpp)

file(GLOB TINY_GLTF_SOURCE ${TINY_GLTF_SRC_DIR}/*.cpp)
file(GLOB TINY_GLTF_HEADER ${TINY_GLTF_HDR_DIR}/*.h)
file(GLOB TINY_GLTF_INLINE ${TINY_GLTF_HDR_DIR}/*.hpp)

file(GLOB WINDOW_SOURCE ${WINDOW_SRC_DIR}/*.cpp)
file(GLOB WINDOW_HEADER ${WINDOW_HDR_DIR}/*.h)
file(GLOB WINDOW_INLINE ${WINDOW_HDR_DIR}/*.hpp)
#
file(GLOB WINDOW_GLFW_SOURCE ${WINDOW_GLFW_SRC_DIR}/*.cpp)
file(GLOB WINDOW_GLFW_HEADER ${WINDOW_GLFW_HDR_DIR}/*.h)
file(GLOB WINDOW_GLFW_INLINE ${WINDOW_GLFW_HDR_DIR}/*.hpp)

#
#source_group("Header Files/application" FILES ${APPLICATION_HEADER})
#source_group("Inline Files/application" FILES ${APPLICATION_INLINE})
#source_group("Source Files/application" FILES ${APPLICATION_SOURCE})
#
#source_group("Header Files/camera" FILES ${CAMERA_HEADER})
#source_group("Inline Files/camera" FILES ${CAMERA_INLINE})
#source_group("Source Files/camera" FILES ${CAMERA_SOURCE})
#
source_group("Header Files/core" FILES ${CORE_HEADER})
source_group("Inline Files/core" FILES ${CORE_INLINE})
source_group("Source Files/core" FILES ${CORE_SOURCE})
#
#source_group("Header Files/ecs" FILES ${ECS_HEADER})
#source_group("Inline Files/ecs" FILES ${ECS_INLINE})
#source_group("Source Files/ecs" FILES ${ECS_SOURCE})
#
#source_group("Header Files/event" FILES ${EVENT_HEADER})
#source_group("Inline Files/event" FILES ${EVENT_INLINE})
#source_group("Source Files/event" FILES ${EVENT_SOURCE})
#
source_group("Header Files/gla" FILES ${GLA_HEADER})
source_group("Inline Files/gla" FILES ${GLA_INLINE})
source_group("Source Files/gla" FILES ${GLA_SOURCE})
#
source_group("Inline VULKAN Files/gla/vulkan" FILES ${GLA_VULKAN_INLINE})
source_group("Source VULKAN Files/gla/vulkan" FILES ${GLA_VULKAN_SOURCE})
source_group("Header VULKAN Files/gla/vulkan" FILES ${GLA_VULKAN_HEADER})
#
#source_group("Header Files/renderer" FILES ${RENDERER_HEADER})
#source_group("Inline Files/renderer" FILES ${RENDERER_INLINE})
#source_group("Source Files/renderer" FILES ${RENDERER_SOURCE})
#
#source_group("Header Files/" FILES ${ROOT_HEADER})
#source_group("Inline Files/" FILES ${ROOT_INLINE})
#source_group("Source Files/" FILES ${ROOT_SOURCE})
#
#source_group("Header Files/scene" FILES ${SCENE_HEADER})
#source_group("Inline Files/scene" FILES ${SCENE_INLINE})
#source_group("Source Files/scene" FILES ${SCENE_SOURCE})
#
#source_group("Header Files/stb" FILES ${STB_HEADER})
#source_group("Inline Files/stb" FILES ${STB_INLINE})
#source_group("Source Files/stb" FILES ${STB_SOURCE})
#
#source_group("Header Files/windows" FILES ${WINDOW_HEADER})
#source_group("Inline Files/windows" FILES ${WINDOW_INLINE})
#source_group("Source Files/window" FILES ${WINDOW_SOURCE})
#
#source_group("Header Files/windows" FILES ${WINDOW_GLFW_HEADER})
#source_group("Inline Files/windows" FILES ${WINDOW_GLFW_INLINE})
#source_group("Source Files/window" FILES ${WINDOW_GLFW_SOURCE})
set(WUNDER_RENDERER_HEADERS
        #${APPLICATION_HEADER}
        #${APPLICATION_INLINE}
        ${ASSETS_HEADER}
        ${ASSETS_INLINE}
        ${ASSETS_COMPONENTS_HEADER}
        ${ASSETS_COMPONENTS_INLINE}
        ${ASSETS_SERIALIZERS_HEADER}
        ${ASSETS_SERIALIZERS_INLINE}
        ${ASSETS_GLTF_HEADER}
        ${ASSETS_GLTF_INLINE}
        ${CAMERA_HEADER}
        ${CAMERA_HEADER}
        ${CORE_HEADER}
        ${CORE_INLINE}
        ${ENTITY_HEADER}
        ${ENTITY_INLINE}
        ${EVENT_HEADER}
        ${EVENT_INLINE}
        ${GLA_HEADER}
        ${GLA_INLINE}
        ${GLA_VULKAN_HEADER}
        ${GLA_VULKAN_INLINE}
        ${GLA_VULKAN_RAY_TRACE_HEADER}
        ${GLA_VULKAN_RAY_TRACE_INLINE}
        ${GLA_VULKAN_SCENE_HEADER}
        ${GLA_VULKAN_SCENE_INLINE}
        ${ROOT_HEADER}
        ${ROOT_INLINE}
        #${RENDERER_HEADER}
        #${RENDERER_INLINE}
        ${SHARED_HEADERS}
        ${SCENE_HEADER}
        ${SCENE_INLINE}
        #${STB_HEADER}
        #${STB_INLINE}
        ${TINY_GLTF_HEADER}
        ${TINY_GLTF_INLINE}
        ${WINDOW_HEADER}
        ${WINDOW_INLINE}
        ${WINDOW_GLFW_HEADER}
        ${WINDOW_GLFW_INLINE}
)

set(WUNDER_RENDERER_SOURCES
        #${APPLICATION_SOURCE}
        ${ASSETS_SOURCE}
        ${ASSETS_COMPONENTS_SOURCE}
        ${ASSETS_SERIALIZERS_SOURCE}
        ${ASSETS_GLTF_SOURCE}
        ${CAMERA_SOURCE}
        ${CORE_SOURCE}
        #${ECS_SOURCE}
        ${EVENT_SOURCE}
        ${GLA_SOURCE}
        ${GLA_VULKAN_SOURCE}
        ${GLA_VULKAN_RAY_TRACE_SOURCE}
        ${GLA_VULKAN_SCENE_SOURCE}
        #${RENDERER_SOURCE}
        ${ROOT_SOURCE}
        ${SCENE_SOURCE}
        #${STB_SOURCE}
        ${TINY_GLTF_SOURCE}
        ${WINDOW_SOURCE}
        ${WINDOW_GLFW_SOURCE}
)
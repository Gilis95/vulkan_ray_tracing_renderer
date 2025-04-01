################################################################################################
#Project Header directory
set(HDR_DIR ${PROJECT_SOURCE_DIR}/include)

set(RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
set(SHADERS_DIR ${RESOURCES_DIR}/shaders)

set(APPLICATION_HDR_DIR ${HDR_DIR}/application)
set(ASSETS_HDR_DIR ${HDR_DIR}/assets)
set(ASSETS_COMPONENTS_HDR_DIR ${ASSETS_HDR_DIR}/components)
set(ASSETS_SERIALIZERS_HDR_DIR ${ASSETS_HDR_DIR}/serializers)
set(ASSETS_GLTF_HDR_DIR ${ASSETS_SERIALIZERS_HDR_DIR}/gltf)
set(ASSETS_GLTF_MESH_HDR_DIR ${ASSETS_GLTF_HDR_DIR}/mesh)
set(CAMERA_HDR_DIR ${HDR_DIR}/camera)
set(CORE_HDR_DIR ${HDR_DIR}/core)
set(ENTITY_HDR_DIR ${HDR_DIR}/entity)
set(EVENT_HDR_DIR ${HDR_DIR}/event)
set(GLA_HDR_DIR ${HDR_DIR}/gla)
set(VULKAN_HDR_DIR ${GLA_HDR_DIR}/vulkan)
set(VULKAN_DESCRIPTORS_HDR_DIR ${VULKAN_HDR_DIR}/descriptors)
set(VULKAN_RASTERIZE_HDR_DIR ${VULKAN_HDR_DIR}/rasterize)
set(VULKAN_RAY_TRACE_HDR_DIR ${VULKAN_HDR_DIR}/ray-trace)
set(VULKAN_SCENE_HDR_DIR ${VULKAN_HDR_DIR}/scene)
set(RENDERER_HDR_DIR ${HDR_DIR}/renderer)
set(SCENE_HDR_DIR ${HDR_DIR}/scene)
set(TINY_GLTF_HDR_DIR ${HDR_DIR}/tinygltf)
set(WINDOW_HDR_DIR ${HDR_DIR}/window)
set(WINDOW_GLFW_HDR_DIR ${WINDOW_HDR_DIR}/glfw)

################################################################################################
#Project source directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(APPLICATION_SRC_DIR ${SRC_DIR}/application)
set(ASSETS_SRC_DIR ${SRC_DIR}/assets)
set(ASSETS_COMPONENTS_SRC_DIR ${ASSETS_SRC_DIR}/components)
set(ASSETS_SERIALIZERS_SRC_DIR ${ASSETS_SRC_DIR}/serializers)
set(ASSETS_GLTF_SRC_DIR ${ASSETS_SERIALIZERS_SRC_DIR}/gltf)
set(ASSETS_GLTF_MESH_SRC_DIR ${ASSETS_GLTF_SRC_DIR}/mesh)
set(CAMERA_SRC_DIR ${SRC_DIR}/camera)
set(CORE_SRC_DIR ${SRC_DIR}/core)
set(EVENT_SRC_DIR ${SRC_DIR}/event)
set(ECS_SRC_DIR ${SRC_DIR}/ecs)
set(GLA_SRC_DIR ${SRC_DIR}/gla)
set(VULKAN_SRC_DIR ${GLA_SRC_DIR}/vulkan)
set(VULKAN_DESCRIPTORS_SRC_DIR ${VULKAN_SRC_DIR}/descriptors)
set(VULKAN_RASTERIZE_SRC_DIR ${VULKAN_SRC_DIR}/rasterize)
set(VULKAN_RAY_TRACE_SRC_DIR ${VULKAN_SRC_DIR}/ray-trace)
set(VULKAN_SCENE_SRC_DIR ${VULKAN_SRC_DIR}/scene)
set(RENDERER_SRC_DIR ${SRC_DIR}/renderer)
set(SCENE_SRC_DIR ${SRC_DIR}/scene)
set(TINY_GLTF_SRC_DIR ${SRC_DIR}/tinygltf)
set(WINDOW_SRC_DIR ${SRC_DIR}/window)
set(WINDOW_GLFW_SRC_DIR ${WINDOW_SRC_DIR}/glfw)

################################################################################################
#Dependencies directory
#set(STB_DIR ${PROJECT_SOURCE_DIR}/vendor/stb_image)

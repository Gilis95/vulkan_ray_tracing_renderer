project(wunder-sandbox)

add_definitions(-DWUNDER_ENABLE_LOG -DSK_PLATFORM_LINUX -DTRACY_ENABLE -DGLFW_INCLUDE_NONE
        -DIMGUI_IMPL_VULKAN_USE_GLAD)

include(cmake/FetchDependencies.cmake)
include(cmake/Files.cmake)

add_executable(wunder-sandbox ${WUNDER_EDITOR_HEADERS} ${WUNDER_EDITOR_SOURCES})

target_include_directories(wunder-sandbox PRIVATE ${HDR_DIR} ${wunder-renderer_INCLUDES} )


target_link_libraries(wunder-sandbox PRIVATE glfw glm spdlog Vulkan imgui wunder::wunder-renderer)

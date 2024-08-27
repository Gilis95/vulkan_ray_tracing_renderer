project(wunder-renderer VERSION 1.0.0 LANGUAGES CXX)

add_definitions(-DWANDER_ENABLE_LOG -D_CRT_SECURE_NO_WARNINGS -DWANDER_ENGINE_DEBUG -DTRACY_ENABLE -DGLFW_INCLUDE_NONE)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DWUNDER_LINUX=1)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWUNDER_WINDOWS=1)
endif ()

#Debug
add_definitions(-DENABLE_ASSERTS=1 -DWUNDER_ENABLE_LOG=1)

option(WANDER_ENGINE_INSTALL "Generate installation target" OFF)

include(cmake/Variables.cmake)
include(cmake/Files.cmake)

glad_add_library(Vulkan REPRODUCIBLE LOADER API vulkan=1.3)

################################################################################################
add_library(wunder-renderer STATIC ${WUNDER_RENDERER_HEADERS} ${WUNDER_RENDERER_SOURCES})

target_include_directories(wunder-renderer PUBLIC
        $<BUILD_INTERFACE: ${HDR_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(wunder-renderer PRIVATE
        EnTT
        Vulkan
        tinygltf
        glfw
        glm
        spdlog
        TracyClient
        GPUOpen::VulkanMemoryAllocator
        glslang
        shaderc
        shaderc_util
        glslc
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-reflect
        spirv-cross-cpp
)

if (WANDER_ENGINE_INSTALL)
    # cmake install dirs
    include(GNUInstallDirs)

    set(version_config "${CMAKE_CURRENT_BINARY_DIR}/wunderEngineConfigVersion.cmake")
    set(project_config "${CMAKE_CURRENT_BINARY_DIR}/wunderEngineConfig.cmake")
    set(targets_export_name "wunderEngineTargets")
    set(namespace "wunder::")

    # Include module with function 'write_basic_package_version_file'
    include(CMakePackageConfigHelpers)

    # Configure 'gladConfigVersion.cmake'
    # PROJECT_VERSION is used as a VERSION
    write_basic_package_version_file("${version_config}" COMPATIBILITY SameMajorVersion)

    # Configure 'tracyConfig.cmake'
    # Uses targets_export_name variable.
    configure_package_config_file(
            "Config.cmake.in"
            "${project_config}"
            INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/wunder)

    # Targets:
    install(
            TARGETS wunder-renderer
            EXPORT "${targets_export_name}"
            LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

    install(FILES ${APPLICATION_HEADER} ${APPLICATION_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/application)

    install(FILES ${CAMERA_HEADER} ${CAMERA_HEADER}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/camera)

    install(FILES ${CORE_HEADER} ${CORE_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/core)

    install(FILES ${ECS_HEADER} ${ECS_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/ecs)

    install(FILES ${EVENT_HEADER} ${EVENT_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/event)

    install(FILES ${GLA_HEADER} ${GLA_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/gla)

    install(FILES ${GLA_OPENGL_HEADER} ${GLA_OPENGL_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/gla/platform/opengl)

    install(FILES ${ROOT_HEADER} ${ROOT_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/)

    install(FILES ${RENDERER_HEADER} ${RENDERER_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/renderer)

    install(FILES ${SCENE_HEADER} ${SCENE_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/scene)

    install(FILES ${WINDOW_HEADER} ${WINDOW_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/window)

    install(FILES ${WINDOW_GLFW_HEADER} ${WINDOW_GLFW_INLINE}
            DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/wunder/window)

    # Install wunderConfig.cmake, wunderConfigVersion.cmake
    install(
            FILES "${project_config}" "${version_config}"
            DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/wunder)

    # Create and install wunderTargets.cmake
    install(
            EXPORT "${targets_export_name}"
            NAMESPACE "${namespace}"
            DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/wunder)
endif ()

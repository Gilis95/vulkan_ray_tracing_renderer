project(wunder-renderer VERSION 1.0.0 LANGUAGES CXX)

include(cmake/Files.cmake)
include(cmake/Variables.cmake)

add_definitions(-DWANDER_ENABLE_LOG -D_CRT_SECURE_NO_WARNINGS -DWANDER_ENGINE_DEBUG -DTRACY_ENABLE -DGLFW_INCLUDE_NONE)
message(hui="${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DWANDER_LINUX=1)
endif ()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_definitions(-DWANDER_WINDOWS=1)
endif ()

#Debug
add_definitions(-DENABLE_ASSERTS=1 -DWUNDER_ENABLE_LOG=1)

option(WANDER_ENGINE_INSTALL "Generate installation target" OFF)

glad_add_library(Vulkan REPRODUCIBLE LOADER API vulkan=1.3 EXTENSIONS
        #Nvidia
        VK_NV_ray_tracing_validation,
        VK_NV_glsl_shader,
        VK_NV_device_diagnostics_config,
        VK_NV_device_diagnostic_checkpoints,
        #Nvidia
        #Embedded
        VK_KHR_device_group,
        VK_KHR_dedicated_allocation,
        VK_KHR_ray_tracing_pipeline,
        VK_KHR_swapchain,
        VK_KHR_display,
        VK_KHR_display_swapchain,
        VK_KHR_map_memory2,
        VK_KHR_get_memory_requirements2,
        VK_KHR_bind_memory2,
        VK_KHR_get_physical_device_properties2,
        VK_KHR_buffer_device_address,
        VK_KHR_maintenance1,
        VK_KHR_maintenance2,
        VK_KHR_maintenance3,
        VK_KHR_maintenance4,
        VK_KHR_maintenance5,
        VK_KHR_maintenance6,
        VK_KHR_maintenance7,
        VK_KHR_acceleration_structure,
        VK_KHR_surface,
        VK_KHR_deferred_host_operations,
        VK_KHR_shader_clock,
        #Embedded
        VK_EXT_validation_features,
        VK_EXT_validation_flags,
        VK_EXT_validation_cache,
        VK_EXT_debug_utils,
        VK_EXT_debug_marker,
        VK_EXT_debug_report,
        VK_EXT_buffer_device_address,
        VK_EXT_opacity_micromap,
        VK_EXT_global_priority,
        VK_EXT_image_drm_format_modifier,
        VK_EXT_full_screen_exclusive,
        VK_EXT_pipeline_creation_cache_control
)

set_source_files_properties(
        ${VULKAN_SRC_DIR}/vma_implementation.cpp
        ${TINY_GLTF_SRC_DIR}/stb_image.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-error")

################################################################################################
add_library(wunder-renderer STATIC ${WUNDER_RENDERER_HEADERS} ${WUNDER_RENDERER_SOURCES})

target_compile_options(wunder-renderer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion          # Warn on implicit type conversions (int to float, etc.)
#        -Wsign-conversion     # Warn on signed/unsigned mismatches
#        -Wshadow              # Warn when a variable declaration shadows another
        -Wnon-virtual-dtor    # Warn if a class with virtual functions has a non-virtual destructor
#        -Wold-style-cast      # Warn on C-style casts (encourage C++-style casts)
#        -Woverloaded-virtual  # Warn if a virtual function is hiding a base class overload
#        -Wduplicated-cond     # Warn on repeated conditions in `if` or loops
#        -Wduplicated-branches # Warn if branches of conditionals are identical
#        -Wnull-dereference    # Warn on possible null pointer dereferencing
#        -Wdouble-promotion    # Warn when `float` is implicitly promoted to `double`
        -Wmissing-declarations # Warn if non-`static` functions lack a declaration
#        -fsanitize=address    # Detect memory errors (buffer overflows, use-after-free, etc.)
#        -fsanitize=undefined  # Detect undefined behavior (signed overflows, etc.)
)

target_include_directories(wunder-renderer PUBLIC
        $<BUILD_INTERFACE: ${HDR_DIR}>
        $<INSTALL_INTERFACE:include>
        ${shaderc_SOURCE_DIR}/glslc/src
)

target_link_libraries(wunder-renderer PUBLIC
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

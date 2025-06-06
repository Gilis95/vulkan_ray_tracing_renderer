include(FetchContent)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
        SOURCE_SUBDIR cmake
)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.8.5
)

FetchContent_Declare(
        tinygltf
        GIT_REPOSITORY git@github.com:syoyo/tinygltf.git
        GIT_TAG v2.9.3
)

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE INTERNAL "" FORCE)

FetchContent_MakeAvailable(glad glfw glm spdlog tinygltf)

include(cmake/FetchShadercDependency.cmake)
include(cmake/FetchSPIRVCrossDependency.cmake)
include(cmake/FetchVMADependency.cmake)

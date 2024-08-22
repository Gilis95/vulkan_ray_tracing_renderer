include(FetchContent)

FetchContent_Declare(
        EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.7.1
)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Gilis95/glad
        GIT_TAG origin/glad2
        SOURCE_SUBDIR cmake
)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.4
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.8.5
)

FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/Gilis95/tracy.git
        GIT_TAG 747a3cd
)

FetchContent_Declare(
        vulkan-memory-allocator
        GIT_REPOSITORY git@github.com:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG b8e5747
)

set(VMA_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(EnTT glad glfw glm spdlog tracy vulkan-memory-allocator)

include(cmake/FetchShadercDependency.cmake)
include(cmake/FetchSPIRVCrossDependency.cmake)
FetchContent_Declare(
        spirv-cross
        GIT_REPOSITORY git@github.com:KhronosGroup/SPIRV-Cross.git
        GIT_TAG vulkan-sdk-1.3.290.0
)

set(SPIRV_CROSS_ENABLE_CPP ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_STATIC ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spirv-cross)

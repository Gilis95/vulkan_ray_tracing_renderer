FetchContent_Declare(
        vulkan-memory-allocator
        GIT_REPOSITORY git@github.com:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG b8e5747
)

set(VMA_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(vulkan-memory-allocator)
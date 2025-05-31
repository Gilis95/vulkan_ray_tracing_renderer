include(FetchContent)

#spirv-headers (NEEDED ONLY FOR SPIRV-TOOLS)
FetchContent_Declare(
        spirv-headers
        GIT_REPOSITORY git@github.com:KhronosGroup/SPIRV-Headers
        GIT_TAG vulkan-sdk-1.4.313.0
)

#spirv-tools (NEEDED ONLY FOR SHADERC)
FetchContent_Declare(
        spirv-tools
        GIT_REPOSITORY git@github.com:KhronosGroup/SPIRV-Tools
        GIT_TAG vulkan-sdk-1.4.313.0
)

#spirv-tools (NEEDED ONLY FOR SHADERC)
FetchContent_Declare(
        glslang
        GIT_REPOSITORY git@github.com:KhronosGroup/glslang
        GIT_TAG 15.3.0
)

FetchContent_Declare(
        shaderc
        GIT_REPOSITORY git@github.com:google/shaderc.git
        GIT_TAG v2025.2
)

set(SHADERC_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spirv-headers spirv-tools glslang shaderc)

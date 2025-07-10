include(FetchContent)


FetchContent_Declare(
        imgui
        GIT_REPOSITORY git@github.com:Gilis95/imgui.git
        GIT_TAG docking
)

set(IMGUI_VULKAN ON CACHE INTERNAL "" FORCE)
set(IMGUI_GLFW ON CACHE INTERNAL "" FORCE)

FetchContent_MakeAvailable(imgui)

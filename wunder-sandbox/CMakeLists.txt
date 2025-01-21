project(wunder-sandbox)

add_definitions(-DSTINKY_ENABLE_LOG -DSK_PLATFORM_LINUX -DTRACY_ENABLE -DGLFW_INCLUDE_NONE)

set(HDR_DIR ${PROJECT_SOURCE_DIR}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(HDR_FILES
        ${HDR_DIR}/wunder_application.h
        ${HDR_DIR}/wunder_layer.h
        )

set(SRC_FILES
        ${SRC_DIR}/wunder_application.cpp
        ${SRC_DIR}/wunder_layer.cpp
        )


add_executable(wunder-sandbox ${SRC_FILES} ${HDR_FILES})

target_include_directories(wunder-sandbox PRIVATE ${wunder-renderer_INCLUDES} ${HDR_DIR})

target_link_libraries(wunder-sandbox PRIVATE EnTT glfw glm spdlog wunder::wunder-renderer TracyClient)

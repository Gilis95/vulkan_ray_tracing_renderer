include(cmake/Variables.cmake)

file(GLOB ROOT_SOURCE ${SRC_DIR}/*.cpp)
file(GLOB ROOT_HEADER ${HDR_DIR}/*.h)
file(GLOB ROOT_INLINE ${HDR_DIR}/*.hpp)

file(GLOB GUI_SOURCE ${GUI_SRC_DIR}/*.cpp)
file(GLOB GUI_HEADER ${GUI_HDR_DIR}/*.h)
file(GLOB GUI_INLINE ${GUI_HDR_DIR}/*.hpp)

set(WUNDER_EDITOR_HEADERS
        ${ROOT_HEADER}
        ${ROOT_INLINE}
        ${GUI_HEADER}
        ${GUI_INLINE}
)

set(WUNDER_EDITOR_SOURCES
        ${ROOT_SOURCE}
        ${GUI_SOURCE}
)

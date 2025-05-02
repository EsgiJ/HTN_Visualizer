cmake_minimum_required(VERSION 3.15)
project(HTNVisualizer)

set(CMAKE_CXX_STANDARD 17)

# Dýþ baðýmlýlýklar
add_subdirectory(extern/imgui)
add_subdirectory(extern/glfw)
add_subdirectory(extern/tinyxml2)
add_subdirectory(extern/imgui-node-editor)

file(GLOB_RECURSE CORE_SRC src/core/*.cpp)
file(GLOB_RECURSE CORE_HDR include/htn/core/*.h)

file(GLOB_RECURSE EDITOR_SRC src/editor/*.cpp)
file(GLOB_RECURSE EDITOR_HDR include/htn/editor/*.h)

file(GLOB_RECURSE APP_SRC src/app/*.cpp)
file(GLOB_RECURSE APP_HDR include/htn/app/*.h)

source_group("Source Files\\Core" FILES ${CORE_SRC})
source_group("Header Files\\Core" FILES ${CORE_HDR})

source_group("Source Files\\Editor" FILES ${EDITOR_SRC})
source_group("Header Files\\Editor" FILES ${EDITOR_HDR})

source_group("Source Files\\App" FILES ${APP_SRC})
source_group("Header Files\\App" FILES ${APP_HDR})

set(SOURCES
    ${APP_SRC} ${APP_HDR}
    ${CORE_SRC} ${CORE_HDR}
    ${EDITOR_SRC} ${EDITOR_HDR}
    extern/imgui/imgui.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(HTNVisualizer 
    ${SOURCES} 
    ${NODE_EDITOR_SOURCES}
)

# Include direktörleri
target_include_directories(HTNVisualizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui-node-editor
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/tinyxml2
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/IconFontCppHeaders
)

# Link kütüphaneleri
target_link_libraries(HTNVisualizer PRIVATE
    imgui
    glfw
    tinyxml2
    opengl32
    imgui_node_editor
)
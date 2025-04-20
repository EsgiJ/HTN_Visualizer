cmake_minimum_required(VERSION 3.15)
project(HTNVisualizer)

set(CMAKE_CXX_STANDARD 17)

# imguiConfig.cmake dosyasýný bulun ve kullanýn
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui")

# imgui'yi bulmaya çalýþýn
find_package(imgui REQUIRED)

add_subdirectory(extern/imgui)
add_subdirectory(extern/imnodes)
add_subdirectory(extern/tinyxml2)
add_subdirectory(extern/glfw)


set(SOURCES
    src/main.cpp
    src/htn_parser.cpp
    extern/imnodes/imnodes.cpp
)

add_executable(HTNVisualizer ${SOURCES})

target_include_directories(HTNVisualizer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imnodes
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/tinyxml2
)

target_link_libraries(HTNVisualizer
    PRIVATE
    imgui
    imnodes
    glfw
    tinyxml2
    opengl32
)

if (WIN32)
    target_link_libraries(HTNVisualizer PRIVATE opengl32)
endif()
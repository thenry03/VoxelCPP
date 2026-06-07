cmake_minimum_required(VERSION 3.20)
project(VoxelCPP VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json so editors (VS Code IntelliSense) use the
# exact include paths and defines from the build.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_subdirectory(third_party/glm)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(VoxelCPP ${SOURCES})

target_include_directories(VoxelCPP PRIVATE
    src
    third_party/glad/include
    third_party/glfw/include
    third_party/glm
    third_party/stb
)

target_link_libraries(VoxelCPP PRIVATE
    glad
    glfw
    glm::glm
)

# Prevent GLFW from including the system OpenGL header; GLAD provides it.
target_compile_definitions(VoxelCPP PRIVATE GLFW_INCLUDE_NONE)

target_compile_options(VoxelCPP PRIVATE -Wall -Wextra -Wpedantic)

set_target_properties(VoxelCPP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
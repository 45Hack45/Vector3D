## Setup minimum required version for CMake
cmake_minimum_required(VERSION 3.15)

project(vector3d VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

option(VECTOR_3D_BUILD_EXECUTABLE "Build the executable" ON)

# Disable PLOG Tests
SET(PLOG_BUILD_TESTS OFF)
add_subdirectory(include/plog)

# Vulkan
find_package(Vulkan REQUIRED)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

add_subdirectory(libraries/glfw-3.4)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLM Build unit tests")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
SET(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(libraries/glm EXCLUDE_FROM_ALL)

# ## Set C++ standard/compile-time options
# add_library(project_options INTERFACE)
# target_compile_features(project_options INTERFACE cxx_std_17)

SET(ENGINE_INCLUDE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

# add_executable(${PROJECT_NAME} main.cpp)

# target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm)


# cmake_minimum_required(VERSION 3.16)
# project(vulkanLearning)

# find_package(Vulkan REQUIRED)



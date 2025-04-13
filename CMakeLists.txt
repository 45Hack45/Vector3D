## Setup minimum required version for CMake
cmake_minimum_required(VERSION 3.15)

project(vector3d VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

option(VECTOR_3D_BUILD_EXECUTABLE "Build the executable" ON)

# Disable PLOG Tests
SET(PLOG_BUILD_TESTS OFF)
add_subdirectory(include/plog EXCLUDE_FROM_ALL)

add_definitions( -DBOOST_ALL_NO_LIB )
find_package(Boost REQUIRED)

# Vulkan
find_package(Vulkan REQUIRED)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} EXCLUDE_FROM_ALL)

SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLM Build unit tests")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
SET(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(libraries/glfw-3.4)
add_subdirectory(include/glm EXCLUDE_FROM_ALL)

# ## Set C++ standard/compile-time options
# add_library(project_options INTERFACE)
# target_compile_features(project_options INTERFACE cxx_std_17)

SET(ENGINE_INCLUDE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS} ${Chrono_INCLUDE_DIR})

set(vector_3d_LIBS glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} Vulkan::Vulkan glm::glm plog ${CHRONO_TARGETS})

add_subdirectory(src)

# add_executable(${PROJECT_NAME} main.cpp)

# target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm)

# Add the vector_3d executable to the project if specified
if(VECTOR_3D_BUILD_EXECUTABLE)
    message(STATUS "-- Adding Core Engine Executable")
    add_executable(vector_3d_exe "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    target_include_directories(vector_3d_exe PUBLIC
        ${ENGINE_INCLUDE}
    )
    target_link_libraries(vector_3d_exe PRIVATE vector_3d)
endif()

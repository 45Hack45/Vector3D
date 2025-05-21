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

# Project Chrono
find_package(Chrono
             COMPONENTS Vehicle
             CONFIG)

set(Chrono_INCLUDE_DIR "" CACHE STRING "Project Chrono include path")

# Return now if Chrono or a required component was not found.
if (NOT Chrono_FOUND)
  message("Could not find Chrono or one of its required modules")
  return()
endif()
message(STATUS "CHRONO_STATIC = ${CHRONO_STATIC}")


IF (WIN32)
# set stuff for windows
set(WINMM_LIB_PATH C:/Windows/System32/winmm.dll)
ELSE()
# set stuff for other systems
set(WINMM_LIB_PATH )
ENDIF()

SET(ENGINE_INCLUDE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src $${CMAKE_SOURCE_DIR}/include/OBJ-Loader-master/Source ${Boost_INCLUDE_DIRS} ${Chrono_INCLUDE_DIR})
set(vector_3d_LIBS glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} Vulkan::Vulkan glm::glm plog ${CHRONO_TARGETS} ${WINMM_LIB_PATH})

add_subdirectory(src)

# Add the vector_3d executable to the project if specified
if(VECTOR_3D_BUILD_EXECUTABLE)
    message(STATUS "-- Adding Core Engine Executable")
    add_executable(vector_3d_exe "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    target_include_directories(vector_3d_exe PUBLIC
        ${ENGINE_INCLUDE}
    )
    target_link_libraries(vector_3d_exe PUBLIC vector_3d)
endif()

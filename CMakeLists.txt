# # Setup minimum required version for CMake
cmake_minimum_required(VERSION 3.15)
include(CPack)
set(CPACK_GENERATOR "ZIP")

project(vector3d VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

option(VECTOR_3D_BUILD_EXECUTABLE "Build the executable" ON)

# Disable PLOG Tests
# SET(PLOG_BUILD_TESTS OFF)
add_subdirectory(include/plog EXCLUDE_FROM_ALL)

add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED)

# Vulkan
set(Vulkan_ROOT "$ENV{VULKAN_SDK}")
find_package(Vulkan REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} EXCLUDE_FROM_ALL)

message(STATUS "GLFW")
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLM Build unit tests")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
SET(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(libraries/glfw-3.4)
add_subdirectory(include/glm EXCLUDE_FROM_ALL)

set(EiGEN3_INCLUDE_DIR "" CACHE STRING "Eigen include path")


message(STATUS "Chrono")
# Project Chrono
find_package(Chrono
  COMPONENTS Vehicle
  CONFIG)

# set(Chrono_INCLUDE_DIR "" CACHE STRING "Project Chrono include path")

# Return now if Chrono or a required component was not found.
if(NOT Chrono_FOUND)
  message("Could not find Chrono or one of its required modules")
  return()
endif()

message(STATUS "CHRONO_STATIC = ${CHRONO_STATIC}")

# IF(WIN32)
#   # set stuff for windows
#   set(WINMM_LIB_PATH C:/Windows/System32/winmm.dll)
# ELSE()
#   # set stuff for other systems
#   set(WINMM_LIB_PATH)
# ENDIF()

SET(ENGINE_INCLUDE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include/OBJ-Loader-master/Source ${CMAKE_SOURCE_DIR}/src/thirdparty/imgui ${Boost_INCLUDE_DIRS} ${Chrono_INCLUDE_DIR} ${EiGEN3_INCLUDE_DIR} ${CHRONO_INCLUDE_DIRS})
set(vector_3d_LIBS glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} Vulkan::Vulkan glm::glm plog ${CHRONO_TARGETS} ${WINMM_LIB_PATH} ${CHRONO_LIBRARIES} ${X11_X11_LIB})

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

message(STATUS "ENGINE_INCLUDE = ${ENGINE_INCLUDE}")

add_subdirectory(src)

# Add the vector_3d executable to the project if specified
if(VECTOR_3D_BUILD_EXECUTABLE)
  message(STATUS "-- Adding Core Engine Executable")
  add_executable(vector_3d_exe WIN32 "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
  target_include_directories(vector_3d_exe PRIVATE
    ${ENGINE_INCLUDE}
  )
  target_link_libraries(vector_3d_exe PRIVATE vector_3d)
  if(MSVC)
    set_target_properties(vector_3d_exe PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  endif()

  # Copy resources
  add_custom_command(TARGET vector_3d_exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/resources
      $<TARGET_FILE_DIR:vector_3d_exe>/resources
  )

  # Copy external DLLs
  add_custom_command(TARGET vector_3d_exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/external_dll
      $<TARGET_FILE_DIR:vector_3d_exe>
  )

set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/package")

message((STATUS "${CMAKE_BINARY_DIR}/resources"))

# Custom target "package_zip"
add_custom_target(package_zip ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${PACKAGE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vector_3d_exe> "${PACKAGE_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${PACKAGE_DIR}/resources/"
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/Vector3D.zip" --format=zip -- "${PACKAGE_DIR}"
    # WORKING_DIRECTORY "${PACKAGE_DIR}"
    COMMENT "Packaging Vector3D into Vector3D.zip"
)

add_dependencies(package_zip vector_3d_exe)

endif()

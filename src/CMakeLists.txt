
message(STATUS "-- Generating Core Engine")

set(vector_3d_core_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp
)

set(vector_3d_core_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/object_ptr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.h
)

set(vector_3d_core_FILES
    ${vector_3d_core_SOURCES}
    ${vector_3d_core_HEADERS}
)

add_subdirectory(utils)

set(vector_3d_FILES
    ${vector_3d_core_FILES}
    ${vector_3d_utils_FILES}
)

set(vector_3d_LIBS glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm plog)

# Add the vector_3d library to the project
message(STATUS "-- Adding Core Engine Library")

add_library(vector_3d ${vector_3d_FILES})
target_include_directories(vector_3d PUBLIC
    ${ENGINE_INCLUDE}
)
target_link_libraries(vector_3d PRIVATE ${vector_3d_LIBS})


# Add the vector_3d executable to the project if specified
if(VECTOR_3D_BUILD_EXECUTABLE)
    message(STATUS "-- Adding Core Engine Executable")
    add_executable(vector_3d_exe "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" ${vector_3d_FILES})
    target_include_directories(vector_3d_exe PUBLIC
        ${ENGINE_INCLUDE}
    )
    target_link_libraries(vector_3d_exe PRIVATE ${vector_3d_LIBS})
endif()


message(STATUS "-- Generating Core Engine")

set(vector_3d_graphics_backend_SOURCES
    # ${CMAKE_CURRENT_SOURCE_DIR}/rendering/graphics_backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/vulkan_backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl_backend.cpp
)

set(vector_3d_graphics_backend_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/graphics_backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/null_graphics_backend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/vulkan_backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl_backend.h
)

set(vector_3d_core_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
)

set(vector_3d_core_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/object_ptr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.h
)

set(vector_3d_core_FILES
    ${vector_3d_core_SOURCES}
    ${vector_3d_core_HEADERS}
    ${vector_3d_graphics_backend_SOURCES}
    ${vector_3d_graphics_backend_HEADERS}
)

add_subdirectory(utils)

set(vector_3d_FILES
${vector_3d_core_FILES}
${vector_3d_utils_FILES}
)

# Add the vector_3d library to the project
message(STATUS "-- Adding Core Engine Library")

add_library(vector_3d ${vector_3d_FILES})
target_include_directories(vector_3d PUBLIC
    ${ENGINE_INCLUDE}
)

# Set properties for the executable target
target_compile_definitions(vector_3d PUBLIC "CHRONO_DATA_DIR=\"${CHRONO_DATA_DIR}\"") 

if(MSVC)
    set_target_properties(vector_3d PROPERTIES MSVC_RUNTIME_LIBRARY "${CHRONO_MSVC_RUNTIME_LIBRARY}")
endif()

target_link_libraries(vector_3d PUBLIC ${vector_3d_LIBS})
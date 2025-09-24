message(STATUS "-- Generating Core Engine")

message(STATUS "-- CORE")

set(vector_3d_core_SOURCES
    ${CMAKE_SOURCE_DIR}/src/component.cpp
    ${CMAKE_SOURCE_DIR}/src/engine.cpp
    ${CMAKE_SOURCE_DIR}/src/entity.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/scene.cpp
    ${CMAKE_SOURCE_DIR}/src/transform.cpp
    ${CMAKE_SOURCE_DIR}/src/window.cpp
)

set(vector_3d_core_HEADERS
    ${CMAKE_SOURCE_DIR}/src/DefinitionCore.hpp
    ${CMAKE_SOURCE_DIR}/src/asset.h
    ${CMAKE_SOURCE_DIR}/src/camera.hpp
    ${CMAKE_SOURCE_DIR}/src/component.h
    ${CMAKE_SOURCE_DIR}/src/engine.h
    ${CMAKE_SOURCE_DIR}/src/entity.h
    ${CMAKE_SOURCE_DIR}/src/object_ptr.hpp
    ${CMAKE_SOURCE_DIR}/src/scene.h
    ${CMAKE_SOURCE_DIR}/src/transform.h
    ${CMAKE_SOURCE_DIR}/src/window.h
)

set(vector_3d_core_FILES
    ${vector_3d_core_SOURCES}
    ${vector_3d_core_HEADERS}
)

message(STATUS "-- PHYSICS")

set(vector_3d_physics_SOURCES
    ${CMAKE_SOURCE_DIR}/src/physics/Vehicle.cpp
    # ${CMAKE_SOURCE_DIR}/src/physics/VehicleInteractiveController.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/collider.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/physics.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/rigidbody.cpp
)

set(vector_3d_physics_HEADERS
    ${CMAKE_SOURCE_DIR}/src/physics/DefinitionPhysics.hpp
    ${CMAKE_SOURCE_DIR}/src/physics/Vehicle.h
    # ${CMAKE_SOURCE_DIR}/src/physics/VehicleInteractiveController.h
    ${CMAKE_SOURCE_DIR}/src/physics/collider.h
    ${CMAKE_SOURCE_DIR}/src/physics/physics.h
    ${CMAKE_SOURCE_DIR}/src/physics/rigidbody.h
    ${CMAKE_SOURCE_DIR}/src/physics/test_chorno.hpp
    ${CMAKE_SOURCE_DIR}/src/physics/utils.hpp
)

set(vector_3d_physics_FILES
    ${vector_3d_physics_SOURCES}
    ${vector_3d_physics_HEADERS}
)

message(STATUS "-- RENDERING")

set(vector_3d_rendering_SOURCES
    ${CMAKE_SOURCE_DIR}/src/rendering/GizmosManager.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/graphics_backend.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/mesh_renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/opengl_backend.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/shader.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/vulkan_backend.cpp
)

set(vector_3d_rendering_HEADERS
    ${CMAKE_SOURCE_DIR}/src/rendering/GizmosManager.h
    ${CMAKE_SOURCE_DIR}/src/rendering/graphics_backend.h
    ${CMAKE_SOURCE_DIR}/src/rendering/mesh.h
    ${CMAKE_SOURCE_DIR}/src/rendering/mesh_renderer.h
    ${CMAKE_SOURCE_DIR}/src/rendering/model.h
    ${CMAKE_SOURCE_DIR}/src/rendering/null_graphics_backend.hpp
    ${CMAKE_SOURCE_DIR}/src/rendering/opengl_backend.h
    ${CMAKE_SOURCE_DIR}/src/rendering/primitives.hpp
    ${CMAKE_SOURCE_DIR}/src/rendering/rendering_def.h
    ${CMAKE_SOURCE_DIR}/src/rendering/shader.h
    ${CMAKE_SOURCE_DIR}/src/rendering/vulkan_backend.h
)

set(vector_3d_rendering_FILES
    ${vector_3d_rendering_SOURCES}
    ${vector_3d_rendering_HEADERS}
)

message(STATUS "-- UTILS")

set(vector_3d_utils_SOURCES
)

set(vector_3d_utils_HEADERS
    ${CMAKE_SOURCE_DIR}/src/utils/exception.hpp
    ${CMAKE_SOURCE_DIR}/src/utils/keyed_stable_collection.hpp
    ${CMAKE_SOURCE_DIR}/src/utils/utils.hpp
)

set(vector_3d_utils_FILES
    ${vector_3d_utils_SOURCES}
    ${vector_3d_utils_HEADERS}
)

message(STATUS "-- --")

set(vector_3d_FILES
    ${vector_3d_core_FILES}
    ${vector_3d_physics_FILES}
    ${vector_3d_rendering_FILES}
    ${vector_3d_utils_FILES}
)

# ## === AUTO-GENERATED SECTION ENDS HERE ===

# Add the vector_3d library to the project
message(STATUS "-- Adding Core Engine Library")

add_library(vector_3d STATIC ${vector_3d_FILES})
target_include_directories(vector_3d PUBLIC
    ${ENGINE_INCLUDE}
)
# C:/Boost/include/boost-1_87

# Set properties for the executable target
target_compile_definitions(vector_3d PUBLIC "CHRONO_DATA_DIR=\"${CMAKE_SOURCE_DIR}/resources\"")

if(MSVC)
    set_target_properties(vector_3d PROPERTIES MSVC_RUNTIME_LIBRARY "${CHRONO_MSVC_RUNTIME_LIBRARY}")
endif()

target_link_libraries(vector_3d PUBLIC ${vector_3d_LIBS})

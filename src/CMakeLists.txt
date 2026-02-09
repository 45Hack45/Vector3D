message(STATUS "-- Generating Core Engine")

message(STATUS "-- CORE")

set(Vector3D_core_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ModelLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/asset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window.cpp
)

set(Vector3D_core_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/DefinitionCore.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ModelLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ModelManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/asset.h
    ${CMAKE_CURRENT_SOURCE_DIR}/camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/object_ptr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.h
    ${CMAKE_CURRENT_SOURCE_DIR}/transform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window.h
)

set(Vector3D_core_FILES
    ${Vector3D_core_SOURCES}
    ${Vector3D_core_HEADERS}
)

message(STATUS "-- DEMOS")

set(Vector3D_demos_SOURCES
)

set(Vector3D_demos_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/demos/demo_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/demos/demo_defines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/demos/demo_vehicle.hpp
)

set(Vector3D_demos_FILES
    ${Vector3D_demos_SOURCES}
    ${Vector3D_demos_HEADERS}
)

message(STATUS "-- EDITOR")

set(Vector3D_editor_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/ComponentRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/Editor.cpp
)

set(Vector3D_editor_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/ComponentRegistry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/Editor.h
)

set(Vector3D_editor_FILES
    ${Vector3D_editor_SOURCES}
    ${Vector3D_editor_HEADERS}
)

message(STATUS "-- INPUT")

set(Vector3D_input_SOURCES
)

set(Vector3D_input_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/input/InputDevice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/InputKeyCodes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/InputKeys.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/InputManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/input/KeyboardDevice.hpp
)

set(Vector3D_input_FILES
    ${Vector3D_input_SOURCES}
    ${Vector3D_input_HEADERS}
)

message(STATUS "-- PHYSICS")

set(Vector3D_physics_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/Vehicle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/VehicleInteractiveController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/collider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/physics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/rigidbody.cpp
)

set(Vector3D_physics_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/ConstrainLink.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/DefinitionPhysics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/Vehicle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/VehicleInteractiveController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/collider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/physics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/rigidbody.h
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/test_chorno.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/utils.hpp
)

set(Vector3D_physics_FILES
    ${Vector3D_physics_SOURCES}
    ${Vector3D_physics_HEADERS}
)

message(STATUS "-- RENDERING")

set(Vector3D_rendering_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/GizmosManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/graphics_backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/mesh_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl_backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/vulkan_backend.cpp
)

set(Vector3D_rendering_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/GizmosManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/graphics_backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/mesh_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/null_graphics_backend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl_backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/primitives.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/rendering_def.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendering/vulkan_backend.h
)

set(Vector3D_rendering_FILES
    ${Vector3D_rendering_SOURCES}
    ${Vector3D_rendering_HEADERS}
)

message(STATUS "-- THIRDPARTY")

set(Vector3D_thirdparty_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
)

set(Vector3D_thirdparty_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imstb_rectpack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imstb_textedit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imstb_truetype.h
)

set(Vector3D_thirdparty_FILES
    ${Vector3D_thirdparty_SOURCES}
    ${Vector3D_thirdparty_HEADERS}
)

message(STATUS "-- UTILS")

set(Vector3D_utils_SOURCES
)

set(Vector3D_utils_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/exception.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/keyed_stable_collection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils.hpp
)

set(Vector3D_utils_FILES
    ${Vector3D_utils_SOURCES}
    ${Vector3D_utils_HEADERS}
)

message(STATUS "-- --")

set(Vector3D_FILES
    ${Vector3D_core_FILES}
    ${Vector3D_demos_FILES}
    ${Vector3D_editor_FILES}
    ${Vector3D_input_FILES}
    ${Vector3D_physics_FILES}
    ${Vector3D_rendering_FILES}
    ${Vector3D_thirdparty_FILES}
    ${Vector3D_utils_FILES}
)

# ## === AUTO-GENERATED SECTION ENDS HERE ===

# Add the vector_3d library to the project
message(STATUS "-- Adding Core Engine Library")

set(extra_thirdparty_FILES
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_glfw.h
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_glfw.cpp
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.h
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.cpp
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/misc/cpp/imgui_stdlib.h
${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/misc/cpp/imgui_stdlib.cpp
)

if(VECTOR_3D_SHARED)
    add_library(vector_3d SHARED ${Vector3D_FILES} ${extra_thirdparty_FILES})
else()
    add_library(vector_3d STATIC ${Vector3D_FILES} ${extra_thirdparty_FILES})
endif()

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

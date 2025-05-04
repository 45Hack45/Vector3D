
message(STATUS "-- Generating Utils")

set(vector_3d_utils_SOURCES
)

set(vector_3d_utils_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/keyed_stable_collection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils.hpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/utils/vector_ptr.hpp
)

set(vector_3d_utils_FILES
    ${vector_3d_utils_SOURCES}
    ${vector_3d_utils_HEADERS}
)

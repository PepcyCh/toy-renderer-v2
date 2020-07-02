add_library(raytracer
    RayTraceViewer.cpp
    BVHTree.cpp
)

target_include_directories(raytracer
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(raytracer
    PUBLIC scene stb
)
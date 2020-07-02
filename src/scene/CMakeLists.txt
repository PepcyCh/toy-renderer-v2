add_library(scene
    BasicShape.cpp
    Camera.cpp
    Light.cpp
    Material.cpp
    Scene.cpp
    Shape.cpp
    Texture.cpp
    Triangle.cpp
    Intersection.cpp
)

target_include_directories(scene
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(scene
    PUBLIC pepcy::geomath pepcy::gid
)
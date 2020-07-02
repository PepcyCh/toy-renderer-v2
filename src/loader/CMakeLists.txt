add_library(loader
    OBJLoader.cpp
)

target_include_directories(loader
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(loader
    PUBLIC scene
)
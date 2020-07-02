add_library(saver
    OBJSaver.cpp
)

target_include_directories(saver
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(saver
    PUBLIC scene
)
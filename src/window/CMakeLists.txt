add_library(window
    Window.cpp
    InputManager.cpp
)

target_include_directories(window
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(window
    PUBLIC imgui
)
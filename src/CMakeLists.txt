add_subdirectory(geomath)
add_subdirectory(gid)
add_subdirectory(scene)
add_subdirectory(OpenGL)
add_subdirectory(raytracer)
add_subdirectory(window)
add_subdirectory(loader)
add_subdirectory(saver)

configure_file(
    defines.h.in
    defines.h
)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC opengl_mgr raytracer window loader saver
)
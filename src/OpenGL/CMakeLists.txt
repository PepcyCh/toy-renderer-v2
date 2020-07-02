add_library(opengl_mgr
    FrameBuffer.cpp
    LightManager.cpp
    OpenGLViewer.cpp
    RenderTask.cpp
    Shader.cpp
    ShaderManager.cpp
    TextureManager.cpp
    VertexArray.cpp
    VertexArrayManager.cpp
)

target_include_directories(opengl_mgr
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(opengl_mgr
    PUBLIC glad stb scene
)
add_library(gid
    GID.cpp
    GIDManager.cpp
)
add_library(pepcy::gid ALIAS gid)

target_include_directories(gid
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
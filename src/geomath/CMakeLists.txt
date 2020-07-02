add_library(geomath INTERFACE)
add_library(pepcy::geomath ALIAS geomath)

target_include_directories(geomath
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)


include_directories ( ${CMAKE_SOURCE_DIR}/tools  ${CMAKE_SOURCE_DIR}/vendor/SDL2/include)

file ( GLOB ENGINE_SOURCE *.cpp )
file ( GLOB ENGINE_HEADERS *.h )

add_executable( mongrel ${ENGINE_SOURCE} ${ENGINE_HEADERS} )

target_link_libraries(mongrel vendor SDL2-static common glbsp glvis "-framework OpenGL" -lz)

get_target_property(ENGINE_BIN mongrel LOCATION)

add_custom_command(TARGET mongrel
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/artifacts
    COMMAND cp ${ENGINE_BIN} ${CMAKE_SOURCE_DIR}/artifacts/mongrel
)


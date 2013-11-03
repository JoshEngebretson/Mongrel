
include_directories ( ${CMAKE_SOURCE_DIR}/tools  ${CMAKE_SOURCE_DIR}/vendor/SDL2/include)

file ( GLOB ENGINE_SOURCE *.cpp )
file ( GLOB ENGINE_HEADERS *.h )

add_executable( engine ${ENGINE_SOURCE} ${ENGINE_HEADERS} )

target_link_libraries(engine vendor SDL2 common glbsp glvis "-framework OpenGL" -lz)

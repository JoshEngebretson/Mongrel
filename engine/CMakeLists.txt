
include_directories ( ${CMAKE_SOURCE_DIR}/tools )

file ( GLOB ENGINE_SOURCE *.cpp )

add_executable( engine ${ENGINE_SOURCE} )

target_link_libraries(engine vendor common glbsp glvis "-framework OpenGL" -lz)

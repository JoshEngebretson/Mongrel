file ( GLOB VLUMPY_SOURCE *.cpp )
file ( GLOB VLUMPY_HEADERS *.h )

add_executable(vlumpy ${VLUMPY_SOURCE} ${VLUMPY_HEADERS} )
target_link_libraries(vlumpy common -lz)


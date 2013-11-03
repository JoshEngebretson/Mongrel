file ( GLOB ACC_SOURCE *.c )
file ( GLOB ACC_HEADERS *.h )

add_executable(acc ${ACC_SOURCE} ${ACC_HEADERS} )

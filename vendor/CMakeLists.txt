
file ( GLOB JPEG_SOURCE jpeg9/*.c )
file ( GLOB JPEG_HEADER jpeg9/*.h )

file ( GLOB ZLIB_SOURCE zlib/*.c )
file ( GLOB ZLIB_HEADER zlib/*.h )

file ( GLOB PNG_SOURCE lpng/*.c )
file ( GLOB PNG_HEADER lpng/*.h )

add_library( vendor STATIC ${JPEG_SOURCE} ${ZLIB_SOURCE} ${PNG_SOURCE} ${JPEG_HEADER} ${ZLIB_HEADER} ${PNG_HEADER} )

add_subdirectory(SDL2)

add_subdirectory(mygui)



file ( GLOB JPEG_SOURCE jpeg9/*.c )
file ( GLOB PNG_SOURCE lpng/*.c )

add_library( vendor STATIC ${JPEG_SOURCE} ${PNG_SOURCE} )

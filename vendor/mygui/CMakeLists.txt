

file ( GLOB MYGUI_SOURCE src/*.cpp )
file ( GLOB MYGUI_HEADER include/*.h )

add_library( mygui STATIC ${MYGUI_SOURCE} ${MYGUI_HEADER} )

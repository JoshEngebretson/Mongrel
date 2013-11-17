
add_definitions(-DGLEW_STATIC)
add_definitions(-DGL_GLEXT_PROTOTYPES)

file ( GLOB MYGUI_SOURCE src/*.cpp )
file ( GLOB MYGUI_HEADER include/*.h )

file ( GLOB MYGUI_OPENGL_SOURCE platforms/OpenGL/src/*.cpp )
file ( GLOB MYGUI_OPENGL_HEADER platforms/OpenGL/include/*.h )

if (APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wall -Wctor-dtor-privacy -Winit-self -Woverloaded-virtual -Wcast-qual -Wwrite-strings -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

# MyGUI_UString.h ignored from warnings because of this
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
endif(APPLE)


add_library( mygui STATIC ${MYGUI_SOURCE} ${MYGUI_HEADER}
                          ${MYGUI_OPENGL_SOURCE} ${MYGUI_OPENGL_HEADER}
                          platforms/OpenGL/include/GL/glew.c
                          platforms/OpenGL/include/FileSystemInfo/FileSystemInfo.h )

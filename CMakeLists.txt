project(Mongrel)

cmake_minimum_required(VERSION 2.8)

# set(CMAKE_OSX_ARCHITECTURES "i386")

list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if ( MSVC )

    #find_package (Direct3D)

    #if (DIRECT3D_FOUND)
    #    include_directories (${DIRECT3D_INCLUDE_DIR})
    #else()
    #    message( FATAL_ERROR "Direct3D not found." )
    #endif ()

    include_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include")
    link_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86")

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

endif( MSVC )

set(DATADIR ${CMAKE_SOURCE_DIR}/artifacts)

include_directories( common vendor/jpeg9 vendor/lpng vendor/zlib )

add_subdirectory(vendor)
add_subdirectory(common)
add_subdirectory(tools)
add_subdirectory(engine)

add_subdirectory(basev)
add_subdirectory(progs)


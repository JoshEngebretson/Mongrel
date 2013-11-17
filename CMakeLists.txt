project(Mongrel)

cmake_minimum_required(VERSION 2.8)

# set(CMAKE_OSX_ARCHITECTURES "i386")

INCLUDE (CheckIncludeFiles)
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

    #add_definitions(-DZONE_DEBUG)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D__FLTUSED__)

    # Warnings as errors is /WX in C and CXX flags
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE2")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /arch:SSE2")
    #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")

    STRING(REPLACE "/GX" "" CMAKE_CXX_FLAGS_INIT ${CMAKE_CXX_FLAGS_INIT})
    STRING(REPLACE "/GX" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    #statically link
    FOREACH(flag_var CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL
    CMAKE_C_FLAGS_RELWITHDEBINFO )
        IF(${flag_var} MATCHES "/MD")
            STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MD")

        IF(${flag_var} MATCHES "/MDd")
            STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MDd")

        SET(${flag_var} "${${flag_var}} /MP4")

    ENDFOREACH(flag_var)

endif( MSVC )

set(DATADIR ${CMAKE_SOURCE_DIR}/artifacts)

include_directories( common vendor/jpeg9 vendor/lpng vendor/zlib )
include_directories( vendor/mygui/include vendor/mygui/platforms/OpenGL/include)

add_subdirectory(vendor)
add_subdirectory(common)
add_subdirectory(tools)
add_subdirectory(engine)

add_subdirectory(basev)
add_subdirectory(progs)


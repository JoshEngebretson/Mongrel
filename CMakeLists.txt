project(Mongrel)

cmake_minimum_required(VERSION 2.8)

# set(CMAKE_OSX_ARCHITECTURES "i386")

set(DATADIR ${CMAKE_SOURCE_DIR}/artifacts)

include_directories( common vendor/jpeg9 vendor/lpng )

add_subdirectory(vendor)
add_subdirectory(common)
add_subdirectory(tools)
add_subdirectory(engine)

add_subdirectory(basev)
add_subdirectory(progs)


project(Mongrel)

cmake_minimum_required(VERSION 2.8)

include_directories( common vendor/jpeg9 vendor/lpng )

add_subdirectory(vendor)
add_subdirectory(common)
add_subdirectory(tools)
add_subdirectory(engine)


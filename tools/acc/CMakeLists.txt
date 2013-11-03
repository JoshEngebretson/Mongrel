file ( GLOB ACC_SOURCE *.c )
file ( GLOB ACC_HEADERS *.h )

add_executable(acc ${ACC_SOURCE} ${ACC_HEADERS} )

get_target_property(ACC_BIN acc LOCATION)

add_custom_command(TARGET acc
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/artifacts
    COMMAND cp ${ACC_BIN} ${CMAKE_SOURCE_DIR}/artifacts/acc
)

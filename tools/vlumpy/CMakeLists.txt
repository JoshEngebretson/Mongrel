file ( GLOB VLUMPY_SOURCE *.cpp )
file ( GLOB VLUMPY_HEADERS *.h )

add_executable(vlumpy ${VLUMPY_SOURCE} ${VLUMPY_HEADERS} )
target_link_libraries(vlumpy common vendor)

get_target_property(VLUMPY_BIN vlumpy LOCATION)



if (MSVC)

add_custom_command(TARGET vlumpy
        POST_BUILD
        COMMAND echo f | xcopy /F /Y \"${VLUMPY_BIN}\" \"${CMAKE_SOURCE_DIR}/artifacts/vlumpy.exe\"
    )

else()

add_custom_command(TARGET vlumpy
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/artifacts
    COMMAND cp ${VLUMPY_BIN} ${CMAKE_SOURCE_DIR}/artifacts/vlumpy
)

endif()



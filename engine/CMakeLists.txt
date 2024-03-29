
include_directories ( ${CMAKE_SOURCE_DIR}/tools  ${CMAKE_SOURCE_DIR}/vendor/SDL2/include)

file ( GLOB ENGINE_SOURCE *.cpp )
file ( GLOB ENGINE_HEADERS *.h )

add_definitions( -DDEVELOPER=1 )

add_executable( mongrel ${ENGINE_SOURCE} ${ENGINE_HEADERS} )

get_target_property(ENGINE_BIN mongrel LOCATION)

if (MSVC)

target_link_libraries(mongrel vendor SDL2 common glbsp glvis OpenGL32 kernel32 advapi32 COMCTL32 COMDLG32 USER32 ADVAPI32 GDI32 WINMM WSOCK32 Ws2_32 Imm32 Version)

add_custom_command(TARGET mongrel
        POST_BUILD
        COMMAND echo f | xcopy /F /Y \"${ENGINE_BIN}\" \"${CMAKE_SOURCE_DIR}/artifacts/mongrel.exe\"
    )

else()

target_link_libraries( mongrel vendor SDL2 common glbsp glvis
                              "-framework AudioUnit"
                              "-framework Carbon"
                              "-framework Cocoa"
                              "-framework CoreAudio"
                              "-framework ForceFeedback"
                              "-framework IOKit"
                              "-framework OpenGL"
                              "-framework CoreServices" )

add_custom_command(TARGET mongrel
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/artifacts
    COMMAND cp ${ENGINE_BIN} ${CMAKE_SOURCE_DIR}/artifacts/mongrel
)

endif()





add_definitions(-DIN_VCC)

add_executable(vcc	
	${CMAKE_SOURCE_DIR}/engine/vc_class.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_class.h
	${CMAKE_SOURCE_DIR}/engine/vc_constant.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_constant.h
	${CMAKE_SOURCE_DIR}/engine/vc_emit_context.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_emit_context.h
	${CMAKE_SOURCE_DIR}/engine/vc_error.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_error.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_array.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_array.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_assign.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_assign.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_base.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_base.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_cast.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_cast.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_field.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_field.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_invoke.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_invoke.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_literal.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_literal.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_local.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_local.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_misc.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_misc.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_type.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_type.h
	${CMAKE_SOURCE_DIR}/engine/vc_expr_unary_binary.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_expr_unary_binary.h
	${CMAKE_SOURCE_DIR}/engine/vc_field.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_field.h
	${CMAKE_SOURCE_DIR}/engine/vc_lexer.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_lexer.h
	${CMAKE_SOURCE_DIR}/engine/vc_location.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_location.h
	${CMAKE_SOURCE_DIR}/engine/vc_member.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_member.h
	${CMAKE_SOURCE_DIR}/engine/vc_method.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_method.h
	${CMAKE_SOURCE_DIR}/engine/vc_modifiers.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_modifiers.h
	${CMAKE_SOURCE_DIR}/engine/vc_package.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_package.h
	${CMAKE_SOURCE_DIR}/engine/vc_parser.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_parser.h
	${CMAKE_SOURCE_DIR}/engine/vc_property.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_property.h
	${CMAKE_SOURCE_DIR}/engine/vc_state.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_state.h
	${CMAKE_SOURCE_DIR}/engine/vc_statement.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_statement.h
	${CMAKE_SOURCE_DIR}/engine/vc_struct.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_struct.h
	${CMAKE_SOURCE_DIR}/engine/vc_type.cpp
	${CMAKE_SOURCE_DIR}/engine/vc_type.h
	vcc.cpp
	vcc.h
)
target_link_libraries(vcc vendor common )

get_target_property(VCC_BIN vcc LOCATION)

if (MSVC)

add_custom_command(TARGET vcc
        POST_BUILD
        COMMAND echo f | xcopy /F /Y \"${VCC_BIN}\" \"${CMAKE_SOURCE_DIR}/artifacts/vcc.exe\"
    )

else()

add_custom_command(TARGET vcc
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/artifacts
    COMMAND cp ${VCC_BIN} ${CMAKE_SOURCE_DIR}/artifacts/vcc
)
endif()




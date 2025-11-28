add_library(neocad_command STATIC
	src/ExtrudeCommand.cpp
	src/InsertBoxCommand.cpp
	src/InsertCylinderCommand.cpp
	src/InsertGlobalParameterCommand.cpp
	src/ConnectExpressionCommand.cpp
	src/UpdateParameterCommand.cpp
)

target_include_directories(neocad_command PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_command
PUBLIC
	glm::glm
	neocad_core
	neocad_domain
)

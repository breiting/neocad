add_library(neocad_editor STATIC
	src/Editor.cpp
	src/InsertPointTool.cpp
	src/InsertLineTool.cpp
	src/InsertCircleTool.cpp
)

target_include_directories(neocad_editor PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_editor
PUBLIC
	neocad_core
	neocad_domain
)

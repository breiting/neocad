add_library(neocad_editor STATIC
	src/Editor.cpp
	src/InsertCircleTool.cpp
	src/InsertPointTool.cpp
	src/SketchCurveTool.cpp
	src/ViewController.cpp
)

target_include_directories(neocad_editor PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_editor
PUBLIC
	project_settings
	neocad_core
	neocad_domain
)

add_executable(neocad
	src/main.cpp
)

target_link_libraries(neocad
PRIVATE
	glm::glm
	neocad_core
	neocad_domain
	neocad_occt
	neocad_command
	neocad_editor
	neocad_ui
	neocad_vis
)

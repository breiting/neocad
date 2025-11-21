add_executable(headless
	src/main.cpp
)

target_link_libraries(headless
PRIVATE
	glm::glm
	neocad_core
	neocad_domain
	neocad_occt
	neocad_command
)

add_library(neocad_ui STATIC
	src/Window.cpp
)


target_include_directories(neocad_ui
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_ui
PUBLIC
	glfw
	glad
	glm::glm
	neocad_editor
)

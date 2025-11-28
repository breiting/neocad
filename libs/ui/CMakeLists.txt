add_library(neocad_ui STATIC
	src/Window.cpp
	src/GraphEditorSystem.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)


target_include_directories(neocad_ui
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${imnodes_SOURCE_DIR}
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

target_link_libraries(neocad_ui
PUBLIC
	glfw
	glad
	glm::glm
	neocad_core
	neocad_domain
	neocad_command
	imgui
	imnodes
)

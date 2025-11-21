add_library(neocad_rendering STATIC
	src/CanvasRenderer2D.cpp
	src/FontLoaderSTB.cpp
	src/GlyphAtlas.cpp
	src/TextRenderer2D.cpp
	src/FontSystem.cpp
)

target_include_directories(neocad_rendering
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern/stb
)

target_link_libraries(neocad_rendering
PUBLIC
	project_settings
	glad
	glm::glm
	neocad_core
	neocad_ui
)

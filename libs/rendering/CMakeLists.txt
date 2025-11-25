add_library(neocad_rendering STATIC
	src/BaseGeometry.cpp
	src/Camera2D.cpp
	src/Camera3D.cpp
	src/DirectionalLight.cpp
	src/FlatShadedMaterial.cpp
	src/FontLoaderSTB.cpp
	src/FontSystem.cpp
	src/GlyphAtlas.cpp
	src/LineSet.cpp
	src/LineSetMaterial.cpp
	src/Mesh.cpp
	src/PointCloud.cpp
	src/Renderer.cpp
	src/RenderingSystem.cpp
	src/Shader.cpp
#	src/StlReader.cpp
	src/TextRenderer2D.cpp
	src/ViewController.cpp
)

target_include_directories(neocad_rendering
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern
	${CMAKE_CURRENT_SOURCE_DIR}/../../extern/stb
	${GEN_INC_DIR}
)

target_link_libraries(neocad_rendering
PUBLIC
	project_settings
	glad
	glm::glm
	neocad_core
	neocad_ui
)

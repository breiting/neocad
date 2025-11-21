add_library(neocad_core STATIC
	src/Logger.cpp
)

target_include_directories(neocad_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_core
PUBLIC
	glm::glm
)


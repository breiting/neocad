add_library(neocad_domain STATIC
	src/GeometrySystem.cpp
	src/Registry.cpp
	src/PrimitiveFactory.cpp
)

target_include_directories(neocad_domain PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(neocad_domain
PUBLIC
	neocad_core
	glm::glm
	Eigen3::Eigen
)


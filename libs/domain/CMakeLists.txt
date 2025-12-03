add_library(ontoflow_domain STATIC
	src/GeometrySystem.cpp
	src/Registry.cpp
	src/PrimitiveFactory.cpp
	src/ExpressionSystem.cpp
	src/FeatureEvaluationSystem.cpp
)

target_include_directories(ontoflow_domain PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ontoflow_domain
PUBLIC
	ontoflow_core
	glm::glm
	Eigen3::Eigen
)


add_executable(test_ontoflow
	core/TestLogger.cpp
	domain/TestRegistry.cpp
	domain/DomainTests.cpp
	occt/TestOCCTBackend.cpp
	lua/TestLuaBasic.cpp
	editor/TestEditor.cpp
	command/TestCommandStack.cpp
)

target_include_directories(test_ontoflow PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(test_ontoflow PRIVATE
	GTest::gtest_main
	project_settings
	ontoflow_core
	ontoflow_domain
	ontoflow_lua
	ontoflow_occt
	ontoflow_editor
	ontoflow_command
	ontoflow_vis
)

add_test(
	NAME test_ontoflow
	COMMAND $<TARGET_FILE:test_ontoflow>
)


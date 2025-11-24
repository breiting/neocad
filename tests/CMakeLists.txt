add_executable(test_neocad
	domain/TestGeometrySystem.cpp
	scripting/TestLuaBasic.cpp
	editor/TestEditor.cpp
)

target_include_directories(test_neocad PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(test_neocad PRIVATE
	GTest::gtest_main
	project_settings
	neocad_domain
	neocad_scripting
	neocad_occt
	neocad_editor
)

add_test(
	NAME test_neocad
	COMMAND $<TARGET_FILE:test_neocad>
)


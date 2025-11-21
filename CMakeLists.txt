cmake_minimum_required(VERSION 3.31.1)
project(neocad VERSION 0.1 LANGUAGES CXX)

# Produce json file for neovim clangd completion
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load compiler settings target
include(cmake/CompilerSettings.cmake)

# Load embed text functionality and create assets folder
include(cmake/EmbedText.cmake)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/assets")

# Dependencies via FetchContent
include(FetchContent)

# ----------------------------------------------------------
# GLM
# ----------------------------------------------------------
FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# ----------------------------------------------------------
# GLFW
# ----------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# ----------------------------------------------------------
# GLAD
# ----------------------------------------------------------
add_library(glad STATIC extern/glad/src/gl.cpp)
target_include_directories(glad PUBLIC extern/glad/include)

# ----------------------------------------------------------
# Google Test
# ----------------------------------------------------------
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# ----------------------------------------------------------
# Eigen
# ----------------------------------------------------------
FetchContent_Declare(
		eigen
		GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
		GIT_TAG 5.0.1
)
FetchContent_MakeAvailable(eigen)

# -------------------------
# Shaders (embed to headers)
# -------------------------
set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
set(GEN_INC_DIR    "${CMAKE_BINARY_DIR}/generated")

# embed_text_to_header("${SHADER_SRC_DIR}/shader.vert"  "${GEN_INC_DIR}/assets/shader_vert.h"  shader_vert_glsl)
# embed_text_to_header("${SHADER_SRC_DIR}/shader.frag"  "${GEN_INC_DIR}/assets/shader_frag.h"  shader_frag_glsl)

add_subdirectory(libs)
add_subdirectory(apps/headless)

enable_testing()
add_subdirectory(tests)

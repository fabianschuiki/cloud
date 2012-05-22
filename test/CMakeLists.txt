add_executable (test main.c)
target_link_libraries (test cloud)
include_directories (${PROJECT_BINARY_DIR})
include_directories (${PROJECT_BINARY_DIR})

add_executable (cloud-cli
	main.c)
set_target_properties (cloud-cli PROPERTIES OUTPUT_NAME cloud)
target_link_libraries (cloud-cli cloud)
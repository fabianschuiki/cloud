include_directories (${PROJECT_BINARY_DIR})

add_executable (cloud-cli
	main.c
	option-parser.c
	account.c)
set_target_properties (cloud-cli PROPERTIES OUTPUT_NAME cloud)
target_link_libraries (cloud-cli cloud)
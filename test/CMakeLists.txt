include_directories(${PROJECT_BINARY_DIR})

add_executable(cloud-test test)
target_link_libraries(cloud-test cloud)
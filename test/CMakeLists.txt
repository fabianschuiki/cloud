include_directories(${PROJECT_SOURCE_DIR}/daemon)

add_executable(cloud-test test)
target_link_libraries(cloud-test cloud)
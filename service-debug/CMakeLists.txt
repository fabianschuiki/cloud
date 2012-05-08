include_directories(${PROJECT_BINARY_DIR})

add_executable(cloud-debugd service.c)
add_library(cloud-debug client.c)

target_link_libraries(cloud-debugd cloud-service)
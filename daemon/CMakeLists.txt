set(sources cloud-daemon event-loop)

include_directories(lib)

add_executable(cloudd ${sources})
target_link_libraries(cloudd cloud)

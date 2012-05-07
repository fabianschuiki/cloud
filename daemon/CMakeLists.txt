add_executable(cloudd cloud-daemon event-loop connection)
add_library(cloud cloud-client connection)
add_library(cloud-service cloud-service connection)

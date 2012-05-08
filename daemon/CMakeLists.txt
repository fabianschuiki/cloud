set (CLOUD_VERSION_MAJOR 0)
set (CLOUD_VERSION_MINOR 1)

configure_file(cloud.h.in ${PROJECT_BINARY_DIR}/cloud.h)
configure_file(cloud-service.h.in ${PROJECT_BINARY_DIR}/service.h)

configure_file(object-public.h ${PROJECT_BINARY_DIR}/object.h)

add_executable(cloud-daemon
	cloud-daemon
	socket
	daemon/client
	daemon/service
	event-loop
	object
	buffer
	connection)

add_library(cloud
	cloud-client
	socket
	event-loop
	connection
	object
	buffer
	client/daemon)

add_library(cloud-service
	cloud-service
	socket
	event-loop
	connection
	object
	buffer
	service/daemon)

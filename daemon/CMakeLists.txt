add_executable(cloud-daemon
	cloud-daemon
	socket
	daemon/client
	daemon/service
	event-loop
	connection)

add_library(cloud
	cloud-client
	socket
	event-loop
	connection
	client/daemon)

add_library(cloud-service
	cloud-service
	socket
	event-loop
	connection
	service/daemon)

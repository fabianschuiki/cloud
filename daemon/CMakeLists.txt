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

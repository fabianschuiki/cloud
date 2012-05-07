add_executable(cloudd
	cloud-daemon
	socket
	daemon/client
	daemon/service
	event-loop
	connection)

add_library(cloud
	cloud-client
	connection)

add_library(cloud-service
	cloud-service
	socket
	event-loop
	connection
	service/daemon)

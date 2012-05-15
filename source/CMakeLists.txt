set (CLOUD_VERSION_MAJOR 0)
set (CLOUD_VERSION_MINOR 1)

# Strips the leading copyright from a given header string.
macro (strip_copyright str)
	string(REGEX REPLACE "/\\*.*\\*/[ \n\r\t]*" "" ${str} "${${str}}")
endmacro ()

# Reads in the named header file and strips its leading comment.
macro (read_header name var)
	file (READ ${name} ${var})
	strip_copyright (${var})
endmacro ()


read_header (service-public.h SERVICE_H)
read_header (object-public.h OBJECT_H)
read_header (client/client-public.h CLIENT_H)
read_header (fd-public.h FD_H)
read_header (run-loop-public.h RUNLOOP_H)
read_header (account-public.h ACCOUNT_H)

configure_file (cloud.h.in ${PROJECT_BINARY_DIR}/cloud.h)
configure_file (cloud-service.h.in ${PROJECT_BINARY_DIR}/cloud-service.h)
include_directories (${PROJECT_BINARY_DIR})


add_executable (cloudd
	daemon.c
	socket.c
	daemon/client.c
	daemon/service.c
	run-loop.c
	object.c
	buffer.c
	list.c
	connection.c)

add_library (cloud
	client/client.c
	socket.c
	connection.c
	run-loop.c
	object.c
	buffer.c
	account.c
	list.c
	client/daemon.c)

add_library (cloud-service
	service.c
	socket.c
	run-loop.c
	connection.c
	object.c
	buffer.c)

add_executable (cloud-cli
	cli/main.c
	cli/option-parser.c
	cli/account.c)
set_target_properties (cloud-cli PROPERTIES OUTPUT_NAME cloud)
target_link_libraries (cloud-cli cloud)

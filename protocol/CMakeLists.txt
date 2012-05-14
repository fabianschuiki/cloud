find_program (XSLTPROC xsltproc)

set (PROTOCOL_HTML ${CMAKE_BINARY_DIR}/protocol/cloud.html)

add_custom_command (
	OUTPUT ${PROTOCOL_HTML}
	COMMAND ${XSLTPROC} -o ${PROTOCOL_HTML} protocol.xsl cloud.xml
	DEPENDS protocol.xsl cloud.xml
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target (protocol ALL DEPENDS ${PROTOCOL_HTML})

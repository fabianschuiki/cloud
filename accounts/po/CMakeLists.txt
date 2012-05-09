find_program (INTLTOOL_UPDATE intltool-update)
find_program (MSGFMT msgfmt)

set (DATA_DIR ${PROJECT_SOURCE_DIR}/accounts/data)

file (MAKE_DIRECTORY ${LOCALE_DIR})
find_program (INTLTOOL_UPDATE intltool-update)
find_program (MSGFMT msgfmt)

set (LOCALEDIR ${PROJECT_BINARY_DIR}/accounts/locale)

file (MAKE_DIRECTORY ${LOCALEDIR})
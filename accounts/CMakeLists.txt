find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM gtkmm-3.0)

if (GTKMM_FOUND)
	set (DATA_DIR ${PROJECT_SOURCE_DIR}/accounts/data)
	set (LOCALE_DIR ${PROJECT_BINARY_DIR}/accounts/locale)
	
	configure_file(config.h.in ${PROJECT_BINARY_DIR}/accounts/config.h)
	include_directories(${PROJECT_BINARY_DIR}/accounts)

	file(GLOB_RECURSE sources RELATIVE ${PROJECT_SOURCE_DIR}/accounts source/*.cpp)
	
	add_executable(cloud-accounts ${sources})

	include_directories(${PROJECT_BINARY_DIR})
	target_link_libraries(cloud-accounts cloud)
	
	link_directories (${GTKMM_LIBRARY_DIRS})
	include_directories (${GTKMM_INCLUDE_DIRS})
	target_link_libraries(cloud-accounts ${GTKMM_LIBRARIES})
else ()
	message ("not building cloud-accounts")
endif ()
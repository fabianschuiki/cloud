find_package(PkgConfig REQUIRED)

set (DATA_DIR ${PROJECT_SOURCE_DIR}/accounts/data)
set (LOCALE_DIR ${PROJECT_BINARY_DIR}/accounts/locale)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/accounts/config.h)
include_directories(${PROJECT_BINARY_DIR}/accounts)

# gather source files
file(GLOB_RECURSE sources RELATIVE ${PROJECT_SOURCE_DIR}/accounts source/*.cpp)

add_executable(cloud-accounts ${sources})

# cloud
include_directories(${PROJECT_BINARY_DIR})
target_link_libraries(cloud-accounts cloud)

# gtkmm3
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
link_directories     (${GTKMM_LIBRARY_DIRS})
include_directories  (${GTKMM_INCLUDE_DIRS})
target_link_libraries(cloud-accounts ${GTKMM_LIBRARIES})
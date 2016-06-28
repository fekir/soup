cmake_minimum_required(VERSION 3.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# flags for win builds
include(WIN_FLAGS)
if(MSVC)
	# flags for msvc compiler
	include(MSVC_FLAGS)
else()
	# flags for gcc (mingw)/clang compiler
	include(GCC_FLAGS)
endif()

option(DONOTSAFEREGKEY "All changes will be lost when rebooting (for development)" ON)

if(DONOTSAFEREGKEY)
	add_definitions( -DDONOTSAFEREGKEY )
endif()


set(APP_NAME "soup")


set(SOUP_MAJOR_NUMBER 0)
set(SOUP_MINOR_NUMBER 5)
set(SOUP_BUILD_NUMBER 0)
set(SOUP_REVISION_NUMBER 1)
set(SOUP_PRODUCT_DECRIPTION "soup (stop unwanted programs) is a program for preventing execution of unwanted software")
set(SOUP_PRODUCT_NAME "soup")
set(SOUP_LICENSE_NOTIICE "Copyright (C) 2014-2016 Federico Kircheis")


set (CMAKE_CXX_STANDARD 14)

add_definitions( -DSOUP_VERSION_NUMBER="${SOUP_MAJOR_NUMBER}.${SOUP_MINOR_NUMBER}.${SOUP_BUILD_NUMBER}.${SOUP_REVISION_NUMBER}" )

configure_file(
	"${PROJECT_SOURCE_DIR}/res/info.in.rc" "${PROJECT_BINARY_DIR}/res/info.rc"
)

configure_file(
	"${PROJECT_SOURCE_DIR}/res/compatibility.manifest" "${PROJECT_BINARY_DIR}/res/compatibility.manifest"
)

set(WIN_LIBRARIES_TO_LINK
	Rpcrt4 KtmW32 wevtapi
)

find_package(catch REQUIRED)
include_directories(${CATCH_INCLUDE_DIRS})

add_subdirectory(lib)
add_subdirectory(gui_qt)


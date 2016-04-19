cmake_minimum_required(VERSION 3.3)

project(${APP_NAME} CXX)

set(HEADER_FILES
	#autoplay.hpp
	policy.hpp
	evtlog.hpp

	# C++ syntax for windows functions
	uuid.hpp
	registry.hpp

	# RAII support for windows types
	win_handles.hpp

	# common
	common.hpp
	IniParser.hpp
)

set(SOURCE_FILES
	IniParser.cpp
	registry.cpp
	policy.cpp
)


add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})


set(TEST_FILES
	test/settings.hpp
	test/test_uuid.cpp
	test/test_policy.cpp
	test/test_registry.cpp
	test/test_ini.cpp
	test/test_common.cpp
	test/test_evtlog.cpp
)

source_group("Test Files" FILES ${TEST_FILES})

set(RC_FILES
	#../res/info.rc
	#../res/info.in.rc
	"${CMAKE_CURRENT_BINARY_DIR}/../res/info.rc"
)

add_definitions( -DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/data/" )
set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES} ${RC_FILES})
target_link_libraries(${PROJECT_NAME_TEST} ${WIN_LIBRARIES_TO_LINK})
target_compile_definitions(${PROJECT_NAME_TEST} PUBLIC "DONOTSAFEREGKEY") # unit test should never change (at least permanently) state of system
target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${PROJECT_SOURCE_DIR})

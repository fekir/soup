cmake_minimum_required(VERSION 3.3)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
#find_package(Qt5Concurrent REQUIRED)
#find_package(Qt5Test REQUIRED)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(${APP_NAME}Qt CXX)

set(GUIQT_FILES

	qtcommon.hpp

	mainwindow.ui
	mainwindow.hpp
	mainwindow.cpp

	policysheet.ui
	policysheet.hpp
	policysheet.cpp

	singlepolicysheetinterface.hpp

	singlepolicysheet.ui
	singlepolicysheet.hpp
	singlepolicysheet.cpp

	singlepolicysheetdoubleext.cpp
	singlepolicysheetdoubleext.hpp
	singlepolicysheetdoubleext.ui

	policysetting.cpp
	policysetting.hpp
	policysetting.ui

	policyeventlog.cpp
	policyeventlog.hpp
	policyeventlog.ui

	diffdialog.cpp
	diffdialog.hpp
	diffdialog.ui

	aboutdialog.cpp
	aboutdialog.hpp
	aboutdialog.ui

	../res/resources.qrc

)

set(RC_FILES
	#../res/info.rc
	#../res/info.in.rc
	"${PROJECT_BINARY_DIR}/../res/info.rc"
)

include_directories(../lib)
include_directories(../res)
add_definitions(${Qt5Widgets_DEFINITIONS})

#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static")          # move assignment was implictly defined as deleted
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
#message("${CMAKE_EXE_LINKER_FLAGS}")
add_executable(${PROJECT_NAME} WIN32 main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${GUIQT_FILES} ${RC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${APP_NAME}")

set(QT_LIBRARIES_TO_LINK
	Qt5::Core Qt5::Gui Qt5::Widgets
)

target_link_libraries(${PROJECT_NAME} ${APP_NAME} ${WIN_LIBRARIES_TO_LINK} ${QT_LIBRARIES_TO_LINK})

# Instruct CMake to run moc automatically
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOUIC TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTORCC TRUE)

set(DEPLOY_DIRECTORY "C:/Work/Deploy")

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DEPLOY_DIRECTORY}
)
#set(CUSTOM_COMMAND "windeployqt ${DEPLOY_DIRECTORY}")
#install(CODE "execute_process(COMMAND ${CUSTOM_COMMAND})")

set(TEST_FILES
	test/main.cpp
	test/test_diffdialog.cpp
	test/test_aboutdialog.cpp
	test/test_policysettings.cpp
	test/test_singlepolicysheet.cpp
	test/test_singlepolicysheetdoubleext.cpp
	test/test_policyeventlog.cpp
)

set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp
	${SOURCE_FILES} ${HEADER_FILES} ${GUIQT_FILES}
	${TEST_FILES}
)

target_link_libraries(${PROJECT_NAME_TEST} ${APP_NAME} ${WIN_LIBRARIES_TO_LINK} ${QT_LIBRARIES_TO_LINK})

# Instruct CMake to run moc automatically
set_target_properties(${PROJECT_NAME_TEST} PROPERTIES AUTOMOC TRUE)
set_target_properties(${PROJECT_NAME_TEST} PROPERTIES AUTOUIC TRUE)
set_target_properties(${PROJECT_NAME_TEST} PROPERTIES AUTORCC TRUE)

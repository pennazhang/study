cmake_minimum_required(VERSION 3.1)
project(ir-daemon VERSION 0.2.0)

set( CMAKE_SYSTEM_PROCESSOR aarch64 )
## If user/dev does not want Unit Tests, it can be set to ON to Skip Unit Testing
OPTION(SKIP_UNIT_TEST "Set to OFF to use Unit Tests" OFF)

find_package(PkgConfig)
find_package(ConfigManager REQUIRED)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    ADD_DEFINITIONS(-DDBUS_SYSTEM_BUS)
    message("DBUS Configuration: SYSTEM")
else()
    message("DBUS Configuration: SESSION")
endif()

#if (NOT ${SKIP_UNIT_TEST} STREQUAL "ON")
#message (STATUS "Unit Test is being included in the Build")
#add_subdirectory(src/Tests)
#endif()

execute_process( COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/xml/CmdHandlerManager.xml
                    --proxy=${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/CmdHandlerManagerProxy-glue.h)
execute_process( COMMAND dbusxx-xml2cpp $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/xml/CmsfsIr.xml
                    --proxy=${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/CmsfsIrProxy-glue.h )

set(FILES_SOURCES   src/main.cpp
                    src/utility/jsIrUtils.cpp
                    src/utility/IRDataBuffer.cpp
                    src/utility/MsgQueue.cpp					
                    src/dbus/CmdHandlerInterface.cpp
                    src/dbus/CmsfsIrInterface.cpp
                    src/client/ClientSetting.cpp
                    src/client/IRRecvThread.cpp
                    src/client/TCPClientThread.cpp
                    src/server/IRSendThread.cpp
                    src/server/TCPServerThread.cpp
                    src/server/ServerSetting.cpp
                    )

include_directories(${CONFIGMANAGER_INCLUDES}
                    src/dbus/
                    src/utility/
                    src/
                    src/client
                    src/server
                    src/common
                    )
set(EXECUTABLE_BIN  ir-daemon)

add_executable(${EXECUTABLE_BIN} ${FILES_SOURCES} )

target_link_libraries(${EXECUTABLE_BIN}
    ${CONFIGMANAGER_LIBRARIES}
    jsoncpp
    dbus-c++-1
    hxlog2
    )

set (CMAKE_CXX_FLAGS "-ldbus-c++-1 -std=c++14 -lpthread -g ${CMAKE_CXX_FLAGS} -Wall")
set_property(TARGET PROPERTY CXX_STANDARD 14)

install(TARGETS ${EXECUTABLE_BIN} DESTINATION bin)

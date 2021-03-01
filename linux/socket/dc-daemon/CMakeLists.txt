cmake_minimum_required(VERSION 3.1)
project(dc-daemon VERSION 0.1.0)

#set( CMAKE_SYSTEM_PROCESSOR aarch64 )
## If user/dev does not want Unit Tests, it can be set to ON to Skip Unit Testing
OPTION(SKIP_UNIT_TEST "Set to OFF to use Unit Tests" OFF)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    ADD_DEFINITIONS(-DDBUS_SYSTEM_BUS)
    message("DBUS Configuration: SYSTEM")
else()
    message("DBUS Configuration: SESSION")
    include_directories(/usr/include/jsoncpp)
endif()

#if (NOT ${SKIP_UNIT_TEST} STREQUAL "ON")
#message (STATUS "Unit Test is being included in the Build")
#add_subdirectory(src/Tests)
#endif()

#execute_process( COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/xml/CmdHandlerManager.xml
#                    --proxy=${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/CmdHandlerManagerProxy-glue.h)
#execute_process( COMMAND dbusxx-xml2cpp $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/xml/CmsfsSystem.xml
#                    --proxy=${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/CmsfsSystem_proxy.h )

#add_custom_command(OUTPUT ./src/CmsfsSystemProxy-glue.h PRE_BUILD
#    COMMAND "dbusxx-xml2cpp" ARGS "$ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/xml/CmsfsSystem.xml" 
#"--proxy=${CMAKE_CURRENT_SOURCE_DIR}/src/CmsfsSystemProxy-glue.h"
#)

set(FILES_SOURCES   src/main.cpp
                    src/dbus/commonInterface.cpp
                    src/utility/utility.cpp
                    src/utility/dcUtility.cpp
					src/utility/thread.cpp
					src/network/socket.cpp
					src/network/tcpServer.cpp
					src/network/tcpSocket.cpp
					src/network/udpSocket.cpp
					src/network/sslLib.cpp
					src/network/secureTCPServer.cpp
					src/network/secureTCPSocket.cpp
					src/network/channelIndex.cpp
					src/network/networkThread.cpp
					src/utility/msgThread.cpp
					src/utility/msgQueue.cpp
                    src/utility/serializer.cpp
                    src/utility/byteArray.cpp
					src/module/moduleThread.cpp
					src/module/protocol.cpp
					src/message.cpp
                    )

include_directories(${CONFIGMANAGER_INCLUDES}
                    src/dbus/
                    src/utility/
                    src/network/
                    src/module/
                    src/
                    )
set(EXECUTABLE_BIN  dc-daemon)

add_executable(${EXECUTABLE_BIN} ${FILES_SOURCES} )

target_link_libraries(${EXECUTABLE_BIN}
    ${CONFIGMANAGER_LIBRARIES}
    jsoncpp
    dbus-c++-1
    ssl
    crypto
#    hxlog2
    )
  
set (CMAKE_CXX_FLAGS " -std=c++14 -pthread -g ${CMAKE_CXX_FLAGS} -Wall -Wno-error=deprecated-declarations")
set_property(TARGET PROPERTY CXX_STANDARD 14)

install(TARGETS ${EXECUTABLE_BIN} DESTINATION bin)

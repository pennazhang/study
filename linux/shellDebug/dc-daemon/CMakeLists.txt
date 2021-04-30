cmake_minimum_required(VERSION 3.1)
project(dc-daemon VERSION 0.1.0)

message("DBUS Configuration: SESSION")
ADD_DEFINITIONS(-DDBUS_SESSION_BUS)

include_directories(/usr/include/jsoncpp /usr/include/dbus-c++-1)

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
					src/utility/msgThread.cpp
					src/utility/msgQueue.cpp
                    src/utility/serializer.cpp
                    src/utility/byteArray.cpp
					src/message.cpp
					src/mainThread.cpp
					src/tcpSocketThread.cpp
					src/udpSocketThread.cpp
					src/dcProtocol.cpp
					src/dbus/dcDaemonServer.cpp
                    )

include_directories(${CONFIGMANAGER_INCLUDES}
                    src/dbus/
                    src/utility/
                    src/network/
                    src/module/
                    src/
                    ${CONFIGMANAGER_INCLUDES}
                    )
set(EXECUTABLE_BIN  dc-daemon)

add_executable(${EXECUTABLE_BIN} ${FILES_SOURCES} )

target_link_libraries(${EXECUTABLE_BIN}
    ${CONFIGMANAGER_LIBRARIES}
    jsoncpp
    dbus-c++-1
    ssl
    crypto
    )
  
set (CMAKE_CXX_FLAGS " -std=c++14 -pthread -g ${CMAKE_CXX_FLAGS} -Wall -Wno-error=deprecated-declarations")
set_property(TARGET PROPERTY CXX_STANDARD 14)

install(TARGETS ${EXECUTABLE_BIN} DESTINATION bin)

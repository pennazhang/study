cmake_minimum_required(VERSION 3.1)
project(ir-daemon VERSION 0.2.0)

include_directories(/usr/include/jsoncpp /usr/include/dbus-c++-1)

execute_process( COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/xml/commonInterface.xml
                    --adaptor=${CMAKE_CURRENT_SOURCE_DIR}/src/dbus/commonInterface_adaptor.h) 

set(FILES_SOURCES   src/main.cpp
                    src/utility/utility.cpp
					src/utility/thread.cpp
                    src/dbus/commonInterface.cpp
                    src/dbus/dbusThread.cpp
					src/helloSetting.cpp
                    )

include_directories(${CONFIGMANAGER_INCLUDES}
                    src/dbus/
                    src/utility/
                    src/
                    src/client
                    src/server
                    src/common
                    )
set(EXECUTABLE_BIN  hello-daemon)

add_executable(${EXECUTABLE_BIN} ${FILES_SOURCES} )

target_link_libraries(${EXECUTABLE_BIN}
    ${CONFIGMANAGER_LIBRARIES}
    jsoncpp
    dbus-c++-1
    )

set (CMAKE_CXX_FLAGS "-ldbus-c++-1 -std=c++14 -pthread -g ${CMAKE_CXX_FLAGS} -Wall")
set_property(TARGET PROPERTY CXX_STANDARD 14)

install(TARGETS ${EXECUTABLE_BIN} DESTINATION bin)

QT       += network
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += . $$PWD/../common
OBJECTS_DIR = $$PWD/out/obj
MOC_DIR = $$PWD/out/moc
DESTDIR = $$PWD/out/bin
CONFIG += console

#COMMON_DIR = $$PWD/../../common

# Header file
HEADERS +=  $$PWD/../src/mainApplication.h \
	$$PWD/../src/main.h \

SOURCES += $$PWD/../src/main.cpp \
	$$PWD/../src/mainApplication.cpp \

win32 {
	DEFINES += _WINDOWS_
#    LIBS += \
#        $$PWD/../libs/win7/i386/setupapi.lib \
#        $$PWD/../libs/win7/ia64/setupapi.lib \
#        $$PWD/../libs/wxp/i386/setupapi.lib
}

linux-arm-g++ {
	DEFINES += _LINUX_ARM_
	
#	HEADERS += $$COMMON_DIR/Hal.h \
#				$$COMMON_DIR/shellDebug/tcpServerFrame.h \
#				$$COMMON_DIR/shellDebug/shellFrame.h
	
#	SOURCES += $$COMMON_DIR/Hal.cpp \
#				$$COMMON_DIR/shellDebug/shellFrame.cpp \
#				$$COMMON_DIR/shellDebug/tcpServerFrame.cpp \
#				$$COMMON_DIR/shellDebug/commonLib.cpp
					
}

linux-g++*{
	DEFINES += _LINUX_
}

QT       += core network
TEMPLATE = app
CONFIG += console

OBJECTS_DIR = $$PWD/out/obj
MOC_DIR = $$PWD/out/moc
RCC_DIR = $$PWD/out/$$BUILDDIR/rcc
DESTDIR = $$PWD/out/bin

INCLUDEPATH += $$PWD/include

SOURCES += $$PWD/src/main.cpp \

win32 {
	DEFINES += __WINDOWS__ 
#    LIBS += \
#        $$PWD/../libs/win7/i386/setupapi.lib \
#        $$PWD/../libs/win7/ia64/setupapi.lib \
#        $$PWD/../libs/wxp/i386/setupapi.lib
}

linux-arm-g++ {
	DEFINES += __LINUX__ 
	
#	HEADERS += $$COMMON_DIR/Hal.h \
#				$$COMMON_DIR/shellDebug/tcpServerFrame.h \
#				$$COMMON_DIR/shellDebug/shellFrame.h
	
#	SOURCES += $$COMMON_DIR/Hal.cpp \
#				$$COMMON_DIR/shellDebug/shellFrame.cpp \
#				$$COMMON_DIR/shellDebug/tcpServerFrame.cpp \
#				$$COMMON_DIR/shellDebug/commonLib.cpp
					
}

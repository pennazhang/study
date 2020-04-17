QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
OBJECTS_DIR = $$PWD/out/obj
MOC_DIR = $$PWD/out/moc
RCC_DIR = $$PWD/out/$$BUILDDIR/rcc
DESTDIR = $$PWD/out/bin

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp

HEADERS += \
    $$PWD/src/mainwindow.h

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

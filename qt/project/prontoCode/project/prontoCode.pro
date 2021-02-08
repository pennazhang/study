QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(debug, debug|release){
    BUILDDIR = debug
} else {
    BUILDDIR = release
}
OBJECTS_DIR = $$PWD/out/$$BUILDDIR/obj
MOC_DIR = $$PWD/out/$$BUILDDIR/moc
RCC_DIR = $$PWD/out/$$BUILDDIR/$$BUILDDIR/rcc
DESTDIR = $$PWD/out/$$BUILDDIR/bin
INCLUDEPATH += $$PWD/../headers
RESOURCES += $$PWD/../resources/prontoCode.qrc
RC_FILE += 	$$PWD/../resources/prontoCode.rc
DEFINES += _CRT_SECURE_NO_WARNINGS 

# Header file
HEADERS += \
    $$PWD/../headers/mainwindow.h \

SOURCES += \
    $$PWD/../sources/main.cpp \
    $$PWD/../sources/mainwindow.cpp \

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

TRANSLATIONS = $$PWD/../resources/demoGui_en.ts $$PWD/../resources/prontoCode_zh.ts
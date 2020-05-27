#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T14:46:51
#
#-------------------------------------------------

QT       += core gui network gui-private sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DVi
TEMPLATE = app
INCLUDEPATH += .

CONFIG += static 

#Arrange output folders
CONFIG(debug, debug|release){
    BUILDDIR = debug
} else {
    BUILDDIR = release
}
OBJECTS_DIR = $$PWD/../out/$$BUILDDIR/obj
UI_DIR = $$PWD/../out/$$BUILDDIR/ui
MOC_DIR = $$PWD/../out/$$BUILDDIR/moc
RCC_DIR = $$PWD/../out/$$BUILDDIR/rcc
DESTDIR = $$PWD/../out/$$BUILDDIR/bin
INCLUDEPATH += $$PWD/../headers
RESOURCES += $$PWD/../resources/DVi.qrc
RC_FILE += 	$$PWD/../resources/DVi.rc
DEFINES += _CRT_SECURE_NO_WARNINGS 

# Header file
HEADERS +=  $$PWD/../headers/Types.h \
            $$PWD/../headers/mainWidget.h \
            $$PWD/../headers/mainApplication.h \
            $$PWD/../headers/commonLib.h \
			$$PWD/../headers/IniSetting.h \
			$$PWD/../headers/DeviceSocket.h \
			$$PWD/../headers/simpleQtLogger.h \
			$$PWD/../headers/backgroundWidget.h \
			$$PWD/../headers/broadcastCommunication.h \
			$$PWD/../headers/deviceSearchingDialog.h \
			$$PWD/../headers/MarkButton.h \				
			$$PWD/../headers/ButtonSlider.h \
			$$PWD/../headers/UpgradeDialog.h \
			$$PWD/../headers/ZoomDlg.h \			
			$$PWD/../headers/WaittingWidget.h \	
			$$PWD/../headers/JsonTable.h \	
			$$PWD/../headers/DeviceData.h \	
			$$PWD/../headers/SQLiteDB.h \	
			$$PWD/../headers/InputWidget.h \	
			$$PWD/../headers/EQData.h \	
			$$PWD/../headers/PresetData.h \	
			$$PWD/../headers/InputEQEditor.h \	
			$$PWD/../headers/EQWidget.h \	
			$$PWD/../headers/FormatOutput.h \	
			$$PWD/../headers/plotBiquad.h \	
			
SOURCES +=  $$PWD/../sources/main.cpp \
			$$PWD/../sources/mainWidget.cpp \
			$$PWD/../sources/mainApplication.cpp \
            $$PWD/../sources/commonLib.cpp \
			$$PWD/../sources/IniSetting.cpp \
			$$PWD/../sources/DeviceSocket.cpp \
			$$PWD/../sources/simpleQtLogger.cpp \
			$$PWD/../sources/backgroundWidget.cpp \
			$$PWD/../sources/broadcastCommunication.cpp \
			$$PWD/../sources/deviceSearchingDialog.cpp \
			$$PWD/../sources/MarkButton.cpp \
			$$PWD/../sources/ButtonSlider.cpp \
			$$PWD/../sources/UpgradeDialog.cpp \
			$$PWD/../sources/ZoomDlg.cpp \			
			$$PWD/../sources/WaittingWidget.cpp \	
			$$PWD/../sources/JsonTable.cpp \	
			$$PWD/../sources/DeviceData.cpp \	
			$$PWD/../sources/SQLiteDB.cpp \	
			$$PWD/../sources/InputWidget.cpp \	
			$$PWD/../sources/EQData.cpp \	
			$$PWD/../sources/PresetData.cpp \	
			$$PWD/../sources/InputEQEditor.cpp \	
			$$PWD/../sources/EQWidget.cpp \	
			$$PWD/../sources/FormatOutput.cpp \	
			$$PWD/../sources/plotBiquad.cpp \	
			
TRANSLATIONS = $$PWD/../resources/DVi_en.ts $$PWD/../resources/DVi_zh.ts

macx:
{
DEFINES += MACX
}

windows:
{
DEFINES += WINDOWS
}


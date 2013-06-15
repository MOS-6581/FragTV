#-------------------------------------------------
#
# Project created by QtCreator 2012-12-26T05:24:48
#
#-------------------------------------------------

isEqual(QT_MAJOR_VERSION, 5) {
QT += core gui widgets network xml   
} else {
QT += core gui network xml  
}

TARGET = FragTVServer
TEMPLATE = app


FORMS += ../../Gui/serverWindow.ui \
         ../../Gui/syncHelper.ui

		 
include(server.pri)
include(../shared.pri)
include(../dmtx.pri)


RESOURCES += ../../Icons/GFX.qrc
win32:RC_FILE = ../../Icons/FragTVServer.rc

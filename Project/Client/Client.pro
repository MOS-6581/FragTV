#-------------------------------------------------
#
# Project created by QtCreator 2012-12-26T05:24:48
#
#-------------------------------------------------

isEqual(QT_MAJOR_VERSION, 5) {
QT += core gui widgets network xml webkit webkitwidgets 
} else {
QT += core gui network xml webkit 
}

TARGET = FragTV
TEMPLATE = app


FORMS += ../../Gui/clientWindow.ui


include(client.pri)
include(browser.pri)
include(../shared.pri)
include(../dmtx.pri)


RESOURCES += ../../Icons/GFX.qrc
win32:RC_FILE = ../../Icons/FragTV.rc

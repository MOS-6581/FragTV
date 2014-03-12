#-------------------------------------------------
#
# Project created by QtCreator 2012-12-26T05:24:48
#
#-------------------------------------------------

isEqual(QT_MAJOR_VERSION, 5) {
QT += core widgets network xml   
} else {
QT += core network xml  
}

QT -= gui

TARGET = FragTV-Dedicated
TEMPLATE = app
CONFIG += console
		 
include(dediserver.pri)


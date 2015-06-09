#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T16:31:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webcamCaptureNewDesign
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../src/MediaFoundation/MediaFoundation_Callback.cpp \
    ../src/MediaFoundation/MediaFoundation_Utils.cpp \
    ../src/MediaFoundation/MediaFoundation_Backend.cpp \
    ../src/MediaFoundation/MediaFoundation_Camera.cpp \
    ../src/Utils.cpp

HEADERS  += mainwindow.h \
    ../include/MediaFoundation/MediaFoundation_Callback.h \
    ../include/MediaFoundation/MediaFoundation_Utils.h \
    ../include/MediaFoundation/MediaFoundation_Backend.h \
    ../include/MediaFoundation/MediaFoundation_Camera.h \
    ../include/BackendInterface.h \
    ../include/CameraInterface.h \
    ../include/Types.h \
    ../include/Utils.h

FORMS    += mainwindow.ui

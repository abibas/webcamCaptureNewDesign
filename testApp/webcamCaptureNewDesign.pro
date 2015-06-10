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
    ../include/backend_implementation.h \
    ../include/video_property.h \
    ../include/format.h \
    ../include/pixel_buffer.h \
    ../include/video_property_range.h \
    ../include/camera_information.h \
    ../include/capability.h \
    ../include/backend_interface.h \
    ../include/camera_interface.h \
    ../include/utils.h \
    ../include/MediaFoundation/media_foundation_backend.h \
    ../include/MediaFoundation/media foundation_callback.h \
    ../include/MediaFoundation/media_foundation_utils.h \
    ../include/MediaFoundation/media_foundation_camera.h \
    ../include/MediaFoundation/media foundation_callback.h \
    ../include/media_foundation/media foundation_callback.h \
    ../include/media_foundation/media_foundation_backend.h \
    ../include/media_foundation/media_foundation_camera.h \
    ../include/media_foundation/media_foundation_utils.h \
    ../include/media_foundation/media_foundation_callback.h

FORMS    += mainwindow.ui

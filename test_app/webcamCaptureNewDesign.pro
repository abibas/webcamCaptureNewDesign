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
    ../src/utils.cpp \
    ../src/media_foundation/media_foundation_backend.cpp \
    ../src/media_foundation/media_foundation_callback.cpp \
    ../src/media_foundation/media_foundation_camera.cpp \
    ../src/media_foundation/media_foundation_utils.cpp

HEADERS  += mainwindow.h \
    ../include/backend_interface.h \
    ../include/camera_interface.h \
    ../include/backend_implementation.h \
    ../include/video_property.h \
    ../include/format.h \
    ../include/pixel_buffer.h \
    ../include/video_property_range.h \
    ../include/camera_information.h \
    ../include/capability.h \
    ../src/utils.h \
    ../src/media_foundation/media foundation_callback.h \
    ../src/media_foundation/media_foundation_backend.h \
    ../src/media_foundation/media_foundation_camera.h \
    ../src/media_foundation/media_foundation_utils.h \
    ../include/image_format_converter.h \
    ../include/backend_factory.h \
    ../src/media_foundation/media_foundation_callback.h

FORMS    += mainwindow.ui

#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T16:31:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webcamCaptureNewDesign
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ../include/ \

DEFINES +=  MEDIA_FOUNDATION \
            DEBUG_VERSION \
            #V4L \
            #AV_FOUNDATION

SOURCES += main.cpp\
    ../src/utils.cpp \
    ../src/media_foundation/media_foundation_backend.cpp \
    ../src/media_foundation/media_foundation_callback.cpp \
    ../src/media_foundation/media_foundation_camera.cpp \
    ../src/media_foundation/media_foundation_utils.cpp \
    ../src/backend_factory.cpp \
    mainwindow.cpp \
    videoform.cpp \
    cameraform.cpp

HEADERS  += \
    ../include/backend_interface.h \
    ../include/camera_interface.h \
    ../include/backend_implementation.h \
    ../include/video_property.h \
    ../include/format.h \
    ../include/pixel_buffer.h \
    ../include/video_property_range.h \
    ../include/camera_information.h \
    ../include/capability.h \
    ../include/image_format_converter.h \
    ../include/backend_factory.h \
    ../src/utils.h \
    ../src/media_foundation/media foundation_callback.h \
    ../src/media_foundation/media_foundation_backend.h \
    ../src/media_foundation/media_foundation_camera.h \
    ../src/media_foundation/media_foundation_utils.h \
    ../src/media_foundation/media_foundation_callback.h \
    mainwindow.h \
    videoform.h \
    cameraform.h \

FORMS    += \
    mainwindow.ui \
    videoform.ui \
    cameraform.ui

win32: LIBS +=  -lMfplat\
                -lOle32\
                -lShlwapi

win32: LIBS += -lMf

win32: LIBS += -lmfuuid

win32: LIBS += -lmfreadwrite

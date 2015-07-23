#-------------------------------------------------
#
# Project created by QtCreator 2015-07-23T05:00:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webcamCaptureNewDesign
TEMPLATE = app


SOURCES += \
    test_app/cameraform.cpp \
    test_app/main.cpp \
    test_app/mainwindow.cpp \
    test_app/videoform.cpp \
    src/backend_factory.cpp \
    src/capability_tree_builder.cpp \
    src/unique_id.cpp

HEADERS  += \
    include/backend_factory.h \
    include/backend_implementation.h \
    include/backend_interface.h \
    include/camera_information.h \
    include/camera_interface.h \
    include/capability.h \
    include/frame.h \
    include/pixel_format_converter.h \
    include/pixel_format.h \
    include/unique_id.h \
    include/video_property_range.h \
    include/video_property.h \
    test_app/cameraform.h \
    test_app/mainwindow.h \
    test_app/videoform.h \
    src/capability_tree_builder.h \
    src/utils.h

FORMS    += \
    test_app/cameraform.ui \
    test_app/mainwindow.ui \
    test_app/videoform.ui

INCLUDEPATH += ./include \

QMAKE_CXXFLAGS += -std=c++11 \
                  -stdlib=libc++ \

DEFINES += WEBCAM_CAPTURE_BACKEND_AV_FOUNDATION

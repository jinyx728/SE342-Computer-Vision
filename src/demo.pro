#-------------------------------------------------
#
# Project created by QtCreator 2017-12-02T16:37:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lumir
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cvAlgorithms.cpp \
    hsldialog.cpp \
    ColorSpace.cpp \
    HSL.cpp \
    doublethresholddialog.cpp \
    zoomrotatedialog.cpp \
    arithmeticdialog.cpp \
    contrastdialog.cpp \
    filterdialog.cpp \
    edgedetectiondialog.cpp \
    houghdialog.cpp \
    cutdialog.cpp \
    binarymordialog.cpp \
    grayscalemordialog.cpp

HEADERS += \
        mainwindow.h \
    cvAlgorithms.h \
    hsldialog.h \
    ColorSpace.hpp \
    HSL.hpp \
    doublethresholddialog.h \
    zoomrotatedialog.h \
    arithmeticdialog.h \
    contrastdialog.h \
    filterdialog.h \
    edgedetectiondialog.h \
    houghdialog.h \
    cutdialog.h \
    binarymordialog.h \
    grayscalemordialog.h \
    WaterShedAlgorithm.h \
    WatershedPixel.h \
    WatershedStructure.h

FORMS += \
        mainwindow.ui \
    hsldialog.ui \
    doublethresholddialog.ui \
    zoomrotatedialog.ui \
    arithmeticdialog.ui \
    contrastdialog.ui \
    filterdialog.ui \
    edgedetectiondialog.ui \
    houghdialog.ui \
    cutdialog.ui \
    binarymordialog.ui \
    grayscalemordialog.ui

RESOURCES += \
    resource.qrc

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2

LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_highgui \
     -lopencv_imgproc \

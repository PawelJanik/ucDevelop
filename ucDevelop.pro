#-------------------------------------------------
#
# Project created by QtCreator 2018-05-23T17:22:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets KTextEditor

TARGET = ucDevelop
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
        ucdevelop.cpp \
    project.cpp \
    projectsettingsdialog.cpp \
    newprojectdialog.cpp \
    mcu.cpp \
    programmer.cpp

HEADERS += \
        ucdevelop.h \
    project.h \
    projectsettingsdialog.h \
    newprojectdialog.h \
    mcu.h \
    programmer.h \
    global.h

FORMS += \
        ucdevelop.ui \
    projectsettingsdialog.ui \
    newprojectdialog.ui

DISTFILES += \
    ucdevelop.png

RESOURCES +=

######################################################################
# Automatically generated by qmake (3.1) Tue Sep 12 16:58:17 2017
######################################################################

QT += widgets
TEMPLATE = app
TARGET = Klotski
INCLUDEPATH += .

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += mainwindow.h \
    gameview.h \
    gamecommon.h \
    graphicspiece.h \
    piece.h \
    move.h \
    layout.h \
    gameviewmodel.h \
    gameview.h \
    gamemodel.h
FORMS += mainwindow.ui
SOURCES += main.cpp mainwindow.cpp \
    gameview.cpp \
    graphicspiece.cpp \
    move.cpp \
    layout.cpp \
    gameviewmodel.cpp \
    gameview.cpp \
    gamemodel.cpp
RESOURCES += resources.qrc

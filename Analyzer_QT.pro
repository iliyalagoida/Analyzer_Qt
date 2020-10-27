QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
    backgroundthread.cpp \
    devices.cpp \
    dp5protocol.cpp \
    dp5status.cpp \
    dpputilities.cpp \
    elements.cpp \
    icon_tabs.cpp \
    main.cpp \
    mainwindow.cpp \
    parsepacket.cpp \
    sendcommand.cpp \
    stringex.cpp

HEADERS += \
    backgroundthread.h \
    devices.h \
    dp5protocol.h \
    dp5status.h \
    dppconst.h \
    dpputilities.h \
    elements.h \
    icon_tabs.h \
    mainwindow.h \
    parsepacket.h \
    sendcommand.h \
    stringex.h

FORMS += \
    mainwindow.ui

LIBS += -lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

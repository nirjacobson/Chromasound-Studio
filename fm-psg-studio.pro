QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    application.cpp \
    ganttwidget/gantteditorwidget.cpp \
    ganttwidget/ganttheaderwidget.cpp \
    ganttwidget/ganttitem.cpp \
    ganttwidget/ganttleftwidget.cpp \
    ganttwidget/ganttwidget.cpp \
    ganttwidget/scrollablewidget.cpp \
    playlistwidget/playlistwidget.cpp \
    project/channel.cpp \
    channelswidget.cpp \
    channelwidget.cpp \
    led.cpp \
    main.cpp \
    mainwindow.cpp \
    project/note.cpp \
    project/pattern.cpp \
    playlistwidget/playlistpatternswidget.cpp \
    prdisplaywidget.cpp \
    project/project.cpp \
    rectled.cpp \
    seekwidget.cpp \
    stepcursorwidget.cpp \
    stepsequencerwidget.cpp \
    timedisplaywidget.cpp \
    topwidget.cpp \
    project/track.cpp

HEADERS += \
    application.h \
    ganttwidget/gantteditorwidget.h \
    ganttwidget/ganttheaderwidget.h \
    ganttwidget/ganttitem.h \
    ganttwidget/ganttleftwidget.h \
    ganttwidget/ganttwidget.h \
    ganttwidget/scrollablewidget.h \
    playlistwidget/playlistwidget.h \
    project/channel.h \
    channelswidget.h \
    channelwidget.h \
    led.h \
    mainwindow.h \
    project/note.h \
    project/pattern.h \
    playlistwidget/playlistpatternswidget.h \
    prdisplaywidget.h \
    project/project.h \
    rectled.h \
    seekwidget.h \
    stepcursorwidget.h \
    stepsequencerwidget.h \
    timedisplaywidget.h \
    topwidget.h \
    project/track.h

FORMS += \
    channelswidget.ui \
    channelwidget.ui \
    ganttwidget/ganttwidget.ui \
    mainwindow.ui \
    playlistwidget/playlistwidget.ui \
    topwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

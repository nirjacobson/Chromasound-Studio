QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    project/channel.cpp \
    project/track.cpp \
    project/note.cpp \
    project/pattern.cpp \
    project/project.cpp \
    ganttwidget/gantteditorwidget.cpp \
    ganttwidget/ganttheaderwidget.cpp \
    ganttwidget/ganttitem.cpp \
    ganttwidget/ganttleftwidget.cpp \
    ganttwidget/ganttwidget.cpp \
    ganttwidget/scrollablewidget.cpp \
    playlistwidget/playlistwidget.cpp \
    channelswidget/channelswidget.cpp \
    channelswidget/channelwidget.cpp \
    channelswidget/led.cpp \
    channelswidget/stepcursorwidget.cpp \
    channelswidget/stepsequencerwidget.cpp \
    channelswidget/rectled.cpp \
    channelswidget/prdisplaywidget.cpp \
    playlistwidget/playlistpatternswidget.cpp \
    main.cpp \
    mainwindow.cpp \
    application.cpp \
    topwidget/seekwidget.cpp \
    topwidget/timedisplaywidget.cpp \
    topwidget/topwidget.cpp

HEADERS += \
    project/channel.h \
    project/note.h \
    project/pattern.h \
    project/project.h \
    project/track.h \
    ganttwidget/gantteditorwidget.h \
    ganttwidget/ganttheaderwidget.h \
    ganttwidget/ganttitem.h \
    ganttwidget/ganttleftwidget.h \
    ganttwidget/ganttwidget.h \
    ganttwidget/scrollablewidget.h \
    playlistwidget/playlistwidget.h \
    channelswidget/channelswidget.h \
    channelswidget/channelwidget.h \
    channelswidget/led.h \
    channelswidget/prdisplaywidget.h \
    channelswidget/rectled.h \
    channelswidget/stepcursorwidget.h \
    channelswidget/stepsequencerwidget.h \
    playlistwidget/playlistpatternswidget.h \
    mainwindow.h \
    application.h \
    topwidget/seekwidget.h \
    topwidget/timedisplaywidget.h \
    topwidget/topwidget.h

FORMS += \
    channelswidget/channelswidget.ui \
    channelswidget/channelwidget.ui \
    ganttwidget/ganttwidget.ui \
    playlistwidget/playlistwidget.ui \
    topwidget/topwidget.ui \
    mainwindow.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

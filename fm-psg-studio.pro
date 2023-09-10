QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bson.cpp \
    channelswidget/stepkeys.cpp \
    channelswidget/stepvelocities.cpp \
    ganttwidget/ganttbottomwidget.cpp \
    pianorollwidget/pianorollkeyswidget.cpp \
    pianorollwidget/pianorollvelocitieswidget.cpp \
    pianorollwidget/pianorollwidget.cpp \
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
    bson.h \
    channelswidget/stepkeys.h \
    channelswidget/stepvelocities.h \
    ganttwidget/ganttbottomwidget.h \
    pianorollwidget/pianorollkeyswidget.h \
    pianorollwidget/pianorollvelocitieswidget.h \
    pianorollwidget/pianorollwidget.h \
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
    pianorollwidget/pianorollwidget.ui \
    playlistwidget/playlistwidget.ui \
    topwidget/topwidget.ui \
    mainwindow.ui \

INCLUDEPATH += -I/usr/include/libbson-1.0

LIBS += -lbson-1.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

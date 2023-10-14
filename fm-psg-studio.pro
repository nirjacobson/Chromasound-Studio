QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bson.cpp \
    channelswidget/stepkeyswidget.cpp \
    channelswidget/stepvelocitieswidget.cpp \
    fm-psg/fm-psg_dummy.cpp \
    fm-psg/fm-psg_impl.cpp \
    fm-psg/vgmplayer.cpp \
    fmwidget/algorithmdisplaywidget.cpp \
    fmwidget/algorithmgraph.cpp \
    fmwidget/algorithmwidget.cpp \
    fmwidget/envelopedisplaywidget.cpp \
    fmwidget/fmwidget.cpp \
    fmwidget/operatorwidget.cpp \
    fmwidget/pianowidget.cpp \
    ganttwidget/ganttbottomwidget.cpp \
    midi/midiinput.cpp \
    midi/midipoller.cpp \
    noisewidget/noisewidget.cpp \
    pianorollwidget/pianorollkeyswidget.cpp \
    pianorollwidget/pianorollvelocitieswidget.cpp \
    pianorollwidget/pianorollwidget.cpp \
    project/channel/channel.cpp \
    project/channel/channelsettings.cpp \
    project/channel/envelopesettings.cpp \
    project/channel/fmchannelsettings.cpp \
    project/channel/noisechannelsettings.cpp \
    project/channel/operatorsettings.cpp \
    project/channel/settings.cpp \
    project/channel/tonechannelsettings.cpp \
    project/playlist.cpp \
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
    project/vgmstream.cpp \
    topwidget/seekwidget.cpp \
    topwidget/timedisplaywidget.cpp \
    topwidget/topwidget.cpp

HEADERS += \
    bson.h \
    channelswidget/stepkeyswidget.h \
    channelswidget/stepvelocitieswidget.h \
    fm-psg/fm-psg.h \
    fm-psg/fm-psg_dummy.h \
    fm-psg/fm-psg_impl.h \
    fm-psg/vgmplayer.h \
    fmwidget/algorithmdisplaywidget.h \
    fmwidget/algorithmgraph.h \
    fmwidget/algorithmwidget.h \
    fmwidget/envelopedisplaywidget.h \
    fmwidget/fmwidget.h \
    fmwidget/operatorwidget.h \
    fmwidget/pianowidget.h \
    ganttwidget/ganttbottomwidget.h \
    midi/midiinput.h \
    midi/midipoller.h \
    noisewidget/noisewidget.h \
    pianorollwidget/pianorollkeyswidget.h \
    pianorollwidget/pianorollvelocitieswidget.h \
    pianorollwidget/pianorollwidget.h \
    project/channel/channel.h \
    project/channel/channelsettings.h \
    project/channel/envelopesettings.h \
    project/channel/fmchannelsettings.h \
    project/channel/noisechannelsettings.h \
    project/channel/operatorsettings.h \
    project/channel/settings.h \
    project/channel/tonechannelsettings.h \
    project/note.h \
    project/pattern.h \
    project/playlist.h \
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
    project/vgmstream.h \
    topwidget/seekwidget.h \
    topwidget/timedisplaywidget.h \
    topwidget/topwidget.h

FORMS += \
    channelswidget/channelswidget.ui \
    channelswidget/channelwidget.ui \
    fmwidget/algorithmwidget.ui \
    fmwidget/fmwidget.ui \
    fmwidget/operatorwidget.ui \
    ganttwidget/ganttwidget.ui \
    noisewidget/noisewidget.ui \
    pianorollwidget/pianorollwidget.ui \
    playlistwidget/playlistwidget.ui \
    topwidget/topwidget.ui \
    mainwindow.ui \

INCLUDEPATH += -I/usr/include/libbson-1.0

LIBS += -lportmidi -lbson-1.0 -lpigpio

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

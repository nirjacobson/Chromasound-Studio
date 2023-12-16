cmake_minimum_required(VERSION 3.5)

project(FM-PSG-Studio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_SOURCES
    channelswidget/channelswidget.cpp
    channelswidget/channelswidget.h
    channelswidget/channelswidget.ui
    channelswidget/channelwidget/channelwidget.cpp
    channelswidget/channelwidget/channelwidget.h
    channelswidget/channelwidget/channelwidget.ui
    channelswidget/channelwidget/led.cpp
    channelswidget/channelwidget/led.h
    channelswidget/channelwidget/prdisplaywidget.cpp
    channelswidget/channelwidget/prdisplaywidget.h
    channelswidget/channelwidget/rectled.cpp
    channelswidget/channelwidget/rectled.h
    channelswidget/stepcursorwidget.cpp
    channelswidget/stepcursorwidget.h
    channelswidget/channelwidget/stepkeyswidget.cpp
    channelswidget/channelwidget/stepkeyswidget.h
    channelswidget/channelwidget/stepsequencerwidget.cpp
    channelswidget/channelwidget/stepsequencerwidget.h
    channelswidget/channelwidget/stepvelocitieswidget.cpp
    channelswidget/channelwidget/stepvelocitieswidget.h
    fm-psg/fm-psg.h
    fm-psg/fm-psg_dummy.cpp
    fm-psg/fm-psg_dummy.h
    fm-psg/fm-psg_impl.cpp
    fm-psg/fm-psg_impl.h
    fm-psg/vgmplayer.cpp
    fm-psg/vgmplayer.h
    fmwidget/algorithmdisplaywidget.cpp
    fmwidget/algorithmdisplaywidget.h
    fmwidget/algorithmgraph.cpp
    fmwidget/algorithmgraph.h
    fmwidget/algorithmwidget.cpp
    fmwidget/algorithmwidget.h
    fmwidget/algorithmwidget.ui
    fmwidget/envelopedisplaywidget.cpp
    fmwidget/envelopedisplaywidget.h
    fmwidget/operatorwidget.cpp
    fmwidget/operatorwidget.h
    fmwidget/operatorwidget.ui
    fmwidget/pianowidget.cpp
    fmwidget/pianowidget.h
    ganttwidget/ganttbottomwidget.cpp
    ganttwidget/ganttbottomwidget.h
    ganttwidget/gantteditorwidget.cpp
    ganttwidget/gantteditorwidget.h
    ganttwidget/ganttheaderwidget.cpp
    ganttwidget/ganttheaderwidget.h
    ganttwidget/ganttitem.cpp
    ganttwidget/ganttitem.h
    ganttwidget/ganttleftwidget.cpp
    ganttwidget/ganttleftwidget.h
    ganttwidget/ganttwidget.cpp
    ganttwidget/ganttwidget.h
    ganttwidget/ganttwidget.ui
    ganttwidget/scrollablewidget.cpp
    ganttwidget/scrollablewidget.h
    noisewidget/noisewidget.cpp
    noisewidget/noisewidget.h
    noisewidget/noisewidget.ui
    pianorollwidget/pianorollkeyswidget.cpp
    pianorollwidget/pianorollkeyswidget.h
    pianorollwidget/pianorollvelocitieswidget.cpp
    pianorollwidget/pianorollvelocitieswidget.h
    playlistwidget/playlistpatternswidget.cpp
    playlistwidget/playlistpatternswidget.h
    playlistwidget/playlistwidget.cpp
    playlistwidget/playlistwidget.h
    playlistwidget/playlistwidget.ui
    project/channel/channel.cpp
    project/channel/channel.h
    project/channel/channelsettings.cpp
    project/channel/channelsettings.h
    project/channel/envelopesettings.cpp
    project/channel/envelopesettings.h
    project/channel/fmchannelsettings.cpp
    project/channel/fmchannelsettings.h
    project/channel/noisechannelsettings.cpp
    project/channel/noisechannelsettings.h
    project/channel/operatorsettings.cpp
    project/channel/operatorsettings.h
    project/channel/settings.cpp
    project/channel/settings.h
    project/channel/tonechannelsettings.cpp
    project/channel/tonechannelsettings.h
    project/channel/algorithmsettings.h
    project/channel/algorithmsettings.cpp
    project/note.cpp
    project/note.h
    project/pattern.cpp
    project/pattern.h
    project/playlist.cpp
    project/playlist.h
    project/project.cpp
    project/project.h
    project/track.cpp
    project/track.h
    project/vgmstream.cpp
    project/vgmstream.h
    topwidget/seekwidget.cpp
    topwidget/seekwidget.h
    topwidget/timedisplaywidget.cpp
    topwidget/timedisplaywidget.h
    topwidget/topwidget.cpp
    topwidget/topwidget.h
    topwidget/topwidget.ui
    commands/addnotecommand.h
    commands/addnotecommand.cpp
    commands/removenotecommand.h
    commands/removenotecommand.cpp
    application.cpp
    application.h
    bson.cpp
    bson.h
    CMakeLists.txt
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    commands/editnotecommand.h
    commands/editnotecommand.cpp
    commands/movechanneldowncommand.h
    commands/movechanneldowncommand.cpp
    commands/movechannelupcommand.h
    commands/movechannelupcommand.cpp
    commands/deletechannelcommand.h
    commands/deletechannelcommand.cpp
    commands/addchannelcommand.h
    commands/addchannelcommand.cpp
    commands/editchannelvolumecommand.h
    commands/editchannelvolumecommand.cpp
    commands/editplaylistcommand.h
    commands/editplaylistcommand.cpp
    commands/addplaylistitemcommand.h
    commands/addplaylistitemcommand.cpp
    commands/removeplaylistitemcommand.h
    commands/removeplaylistitemcommand.cpp
    commands/editnoisechannelsettingscommand.h
    commands/editnoisechannelsettingscommand.cpp
    commands/editoperatorsettingscommand.h
    commands/editoperatorsettingscommand.cpp
    commands/editalgorithmsettingscommand.h
    commands/editalgorithmsettingscommand.cpp
    style/styledialog.h
    style/styledialog.cpp
    style/styledialog.ui
    mdiarea/mdiarea.h
    mdiarea/mdiarea.cpp
    mdiarea/mdisubwindow.h
    mdiarea/mdisubwindow.cpp
    commands/setfmchannelsettingscommand.h
    commands/setfmchannelsettingscommand.cpp
    pianorollwidget/pianorollwidget.h
    pianorollwidget/pianorollwidget.cpp
    pianorollwidget/pianorollwidget.ui
    commands/addtrackitemscommand.h
    commands/addtrackitemscommand.cpp
    commands/removetrackitemscommand.h
    commands/removetrackitemscommand.cpp
    commands/addplaylistitemscommand.h
    commands/addplaylistitemscommand.cpp
    commands/removeplaylistitemscommand.h
    commands/removeplaylistitemscommand.cpp
    midi/chunk/midichunk.cpp
    midi/chunk/midichunk.h
    midi/chunk/midiheader.cpp
    midi/chunk/midiheader.h
    midi/chunk/miditrack.cpp
    midi/chunk/miditrack.h
    midi/chunk/miditrackevent.cpp
    midi/chunk/miditrackevent.h
    midi/event/event.cpp
    midi/event/event.h
    midi/event/metaevent.cpp
    midi/event/metaevent.h
    midi/event/midievent.cpp
    midi/event/midievent.h
    midi/event/sysexevent.cpp
    midi/event/sysexevent.h
    midi/midifile.cpp
    midi/midifile.h
    midi/midiinput.cpp
    midi/midiinput.h
    midi/midipoller.cpp
    midi/midipoller.h
    midi/util.cpp
    midi/util.h
    midi/midi.h
    midi/midi.cpp
    project/channel/pcmchannelsettings.h
    project/channel/pcmchannelsettings.cpp
    pcmwidget/pcmwidget.h
    pcmwidget/pcmwidget.cpp
    pcmwidget/pcmwidget.ui
    commands/setchanneltypecommand.h
    commands/setchanneltypecommand.cpp
    commands/setpcmchannelsettingscommand.h
    commands/setpcmchannelsettingscommand.cpp
    fmimport/fmimportdialog.h
    fmimport/fmimportdialog.cpp
    fmimport/fmimportdialog.ui
    fmimport/patchtablemodel.h
    fmimport/patchtablemodel.cpp
    fmimport/tableview.h
    fmimport/tableview.cpp
    commands/setchannelnamecommand.h
    commands/setchannelnamecommand.cpp
    commands/composite/setfmchannelcommand.h
    commands/composite/setfmchannelcommand.cpp
    commands/composite/setpcmchannelcommand.h
    commands/composite/setpcmchannelcommand.cpp
    commands/composite/replacetrackitemscommand.h
    commands/composite/replacetrackitemscommand.cpp
    commands/composite/addfmchannelcommand.h
    commands/composite/addfmchannelcommand.cpp
    commands/composite/addpcmchannelcommand.h
    commands/composite/addpcmchannelcommand.cpp
    commands/composite/addtrackcommand.h
    commands/composite/addtrackcommand.cpp
    ganttwidget/ganttmarker.h
    ganttwidget/ganttmarker.cpp
    fmwidget/fmwidget.h
    fmwidget/fmwidget.cpp
    fmwidget/fmwidget.ui
    fmwidget/fmwidgetwindow.h
    fmwidget/fmwidgetwindow.cpp
    fmwidget/fmwidgetwindow.ui
    commands/addtracksettingschangecommand.h
    commands/addtracksettingschangecommand.cpp
    commands/addplaylistlfochangecommand.h
    commands/addplaylistlfochangecommand.cpp
    playlistwidget/lfowidget.h
    playlistwidget/lfowidget.cpp
    playlistwidget/lfowidget.ui
    commands/removetracksettingschangecommand.h
    commands/removetracksettingschangecommand.cpp
    commands/removeplaylistlfochangecommand.h
    commands/removeplaylistlfochangecommand.cpp
    project/channel/lfosettings.h
    project/channel/lfosettings.cpp
    fmwidget/fmlfowidget.h
    fmwidget/fmlfowidget.cpp
    fmwidget/fmlfowidget.ui
    commands/editlfosettingscommand.h
    commands/editlfosettingscommand.cpp
    ganttwidget/ganttscalewidget.h
    ganttwidget/ganttscalewidget.cpp
    pianorollwidget/settingschangefooterwidget.h
    pianorollwidget/settingschangefooterwidget.cpp
    pianorollwidget/settingschangefooterwidget.ui
    pianorollwidget/settingschangewidget.h
    pianorollwidget/settingschangewidget.cpp
    pianorollwidget/settingschangewidget.ui
    pcmusage/pcmusagedialog.h
    pcmusage/pcmusagedialog.cpp
    pcmusage/pcmusagedialog.ui
    pcmusage/pcmusagedisplaywidget.h
    pcmusage/pcmusagedisplaywidget.cpp
    pcmusage/pcmusagedisplayitem.h
    pcmusage/pcmusagedisplayitem.cpp
    pcmusage/pcmusagelegendwidget.h
    pcmusage/pcmusagelegendwidget.cpp
    fm-psg/soft/audio_output.h
    fm-psg/soft/blargg_common.h
    fm-psg/soft/blargg_endian.h
    fm-psg/soft/blargg_source.h
    fm-psg/soft/Blip_Buffer.cc
    fm-psg/soft/Blip_Buffer.h
    fm-psg/soft/Classic_Emu.cc
    fm-psg/soft/Classic_Emu.h
    fm-psg/soft/consumer.h
    fm-psg/soft/Data_Reader.cc
    fm-psg/soft/Data_Reader.h
    fm-psg/soft/Dual_Resampler.cc
    fm-psg/soft/Dual_Resampler.h
    fm-psg/soft/Effects_Buffer.cc
    fm-psg/soft/Effects_Buffer.h
    fm-psg/soft/Fir_Resampler.cc
    fm-psg/soft/Fir_Resampler.h
    fm-psg/soft/gme.cc fm-psg/soft/gme.h
    fm-psg/soft/Gme_File.cc
    fm-psg/soft/Gme_File.h
    fm-psg/soft/gme_types.h
    fm-psg/soft/M3u_Playlist.cc
    fm-psg/soft/M3u_Playlist.h
    fm-psg/soft/Multi_Buffer.cc
    fm-psg/soft/Multi_Buffer.h
    fm-psg/soft/Music_Emu.cc
    fm-psg/soft/Music_Emu.h
    fm-psg/soft/producer.h
    fm-psg/soft/Sms_Apu.cc
    fm-psg/soft/Sms_Apu.h
    fm-psg/soft/Sms_Oscs.h
    fm-psg/soft/Vgm_Emu.cc
    fm-psg/soft/Vgm_Emu.h
    fm-psg/soft/Vgm_Emu_Impl.cc
    fm-psg/soft/Vgm_Emu_Impl.h
    fm-psg/soft/Ym2413_Emu.cc
    fm-psg/soft/Ym2413_Emu.h
    fm-psg/soft/Ym2612_Emu.cc
    fm-psg/soft/Ym2612_Emu.h
    fm-psg/fm-psg_soft.cpp
    fm-psg/fm-psg_soft.h
    info/projectinfodialog.h
    info/projectinfodialog.cpp
    info/projectinfodialog.ui
    fm-psg/soft/audio.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FM-PSG-Studio
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FM-PSG-Studio APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FM-PSG-Studio SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FM-PSG-Studio
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(FM-PSG-Studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            /usr/lib64/libpigpio.so
                                            /usr/lib64/libportmidi.so
                                            /usr/lib64/libbson-1.0.so
                                            /usr/lib64/libz.so
                                            /usr/lib64/libportaudio.so)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FM-PSG-Studio)
endif()
set_target_properties(FM-PSG-Studio PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FM-PSG-Studio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FM-PSG-Studio)
endif()

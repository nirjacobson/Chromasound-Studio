cmake_minimum_required(VERSION 3.5)

project(Chromasound-Studio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(WIN32_ARM OFF)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    if (WIN32_ARM)
        include_directories("C:/Program Files (Arm)/mongo-c-driver/include/libbson-1.0"
                            "C:/Program Files (Arm)/portaudio/include"
                            "C:/Program Files (Arm)/portmidi/include"
                            "C:/Program Files (Arm)/zlib/include")
    else()
        include_directories("C:/Program Files/mongo-c-driver/include/libbson-1.0"
                            "C:/Program Files/portaudio/include"
                            "C:/Program Files/portmidi/include"
                            "C:/Program Files/zlib/include")
    endif()
elseif (APPLE)
    include_directories("/System/Volumes/Data/usr/local/include"
                        "/System/Volumes/Data/usr/local/include/libbson-1.0")
endif()


set(CMAKE_OSX_ARCHITECTURES x86_64 arm64)

set(PROJECT_SOURCES
    toplevel/channelswidget/channelswidget.cpp
    toplevel/channelswidget/channelswidget.h
    toplevel/channelswidget/channelswidget.ui
    toplevel/channelswidget/channelwidget/channelwidget.cpp
    toplevel/channelswidget/channelwidget/channelwidget.h
    toplevel/channelswidget/channelwidget/channelwidget.ui
    common/led/led.cpp
    common/led/led.h
    toplevel/channelswidget/channelwidget/prdisplaywidget.cpp
    toplevel/channelswidget/channelwidget/prdisplaywidget.h
    common/led/rectled.cpp
    common/led/rectled.h
    toplevel/channelswidget/stepcursorwidget.cpp
    toplevel/channelswidget/stepcursorwidget.h
    toplevel/channelswidget/channelwidget/stepkeyswidget.cpp
    toplevel/channelswidget/channelwidget/stepkeyswidget.h
    toplevel/channelswidget/channelwidget/stepsequencerwidget.cpp
    toplevel/channelswidget/channelwidget/stepsequencerwidget.h
    toplevel/channelswidget/channelwidget/stepvelocitieswidget.cpp
    toplevel/channelswidget/channelwidget/stepvelocitieswidget.h
    chromasound/chromasound.h
    chromasound/chromasound_standin.cpp
    chromasound/chromasound_standin.h
    opn/algorithmdisplaywidget.cpp
    opn/algorithmdisplaywidget.h
    opn/algorithmgraph.cpp
    opn/algorithmgraph.h
    opn/algorithmwidget.cpp
    opn/algorithmwidget.h
    opn/algorithmwidget.ui
    opn/opnenvelopedisplaywidget.cpp
    opn/opnenvelopedisplaywidget.h
    opn/opnoperatorwidget.cpp
    opn/opnoperatorwidget.h
    opn/opnoperatorwidget.ui
    common/pianowidget.cpp
    common/pianowidget.h
    common/ganttwidget/ganttbottomwidget.cpp
    common/ganttwidget/ganttbottomwidget.h
    common/ganttwidget/gantteditorwidget.cpp
    common/ganttwidget/gantteditorwidget.h
    common/ganttwidget/ganttheaderwidget.cpp
    common/ganttwidget/ganttheaderwidget.h
    common/ganttwidget/ganttitem.cpp
    common/ganttwidget/ganttitem.h
    common/ganttwidget/ganttleftwidget.cpp
    common/ganttwidget/ganttleftwidget.h
    common/ganttwidget/ganttwidget.cpp
    common/ganttwidget/ganttwidget.h
    common/ganttwidget/ganttwidget.ui
    common/ganttwidget/scrollablewidget.cpp
    common/ganttwidget/scrollablewidget.h
    psg/noisewidget.cpp
    psg/noisewidget.h
    psg/noisewidget.ui
    dialogs/pianorollwidget/pianorollkeyswidget.cpp
    dialogs/pianorollwidget/pianorollkeyswidget.h
    dialogs/pianorollwidget/pianorollvelocitieswidget.cpp
    dialogs/pianorollwidget/pianorollvelocitieswidget.h
    toplevel/playlistwidget/playlistpatternswidget.cpp
    toplevel/playlistwidget/playlistpatternswidget.h
    toplevel/playlistwidget/playlistwidget.cpp
    toplevel/playlistwidget/playlistwidget.h
    toplevel/playlistwidget/playlistwidget.ui
    project/channel/channel.cpp
    project/channel/channel.h
    project/channel/channelsettings.cpp
    project/channel/channelsettings.h
    project/channel/opnenvelopesettings.cpp
    project/channel/opnenvelopesettings.h
    project/channel/fmchannelsettings.cpp
    project/channel/fmchannelsettings.h
    project/channel/noisechannelsettings.cpp
    project/channel/noisechannelsettings.h
    project/channel/opnoperatorsettings.cpp
    project/channel/opnoperatorsettings.h
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
    toplevel/topwidget/seekwidget.cpp
    toplevel/topwidget/seekwidget.h
    toplevel/topwidget/timedisplaywidget.cpp
    toplevel/topwidget/timedisplaywidget.h
    toplevel/topwidget/topwidget.cpp
    toplevel/topwidget/topwidget.h
    toplevel/topwidget/topwidget.ui
    commands/addnotecommand.h
    commands/addnotecommand.cpp
    commands/removenotecommand.h
    commands/removenotecommand.cpp
    application.cpp
    application.h
    formats/bson.cpp
    formats/bson.h
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
    commands/editopnoperatorsettingscommand.h
    commands/editopnoperatorsettingscommand.cpp
    commands/editalgorithmsettingscommand.h
    commands/editalgorithmsettingscommand.cpp
    dialogs/style/styledialog.h
    dialogs/style/styledialog.cpp
    dialogs/style/styledialog.ui
    common/mdiarea/mdiarea.h
    common/mdiarea/mdiarea.cpp
    common/mdiarea/mdisubwindow.h
    common/mdiarea/mdisubwindow.cpp
    commands/setfmchannelsettingscommand.h
    commands/setfmchannelsettingscommand.cpp
    dialogs/pianorollwidget/pianorollwidget.h
    dialogs/pianorollwidget/pianorollwidget.cpp
    dialogs/pianorollwidget/pianorollwidget.ui
    commands/addtrackitemscommand.h
    commands/addtrackitemscommand.cpp
    commands/removetrackitemscommand.h
    commands/removetrackitemscommand.cpp
    commands/addplaylistitemscommand.h
    commands/addplaylistitemscommand.cpp
    commands/removeplaylistitemscommand.h
    commands/removeplaylistitemscommand.cpp
    formats/midi/chunk/midichunk.cpp
    formats/midi/chunk/midichunk.h
    formats/midi/chunk/midiheader.cpp
    formats/midi/chunk/midiheader.h
    formats/midi/chunk/miditrack.cpp
    formats/midi/chunk/miditrack.h
    formats/midi/chunk/miditrackevent.cpp
    formats/midi/chunk/miditrackevent.h
    formats/midi/event/event.cpp
    formats/midi/event/event.h
    formats/midi/event/metaevent.cpp
    formats/midi/event/metaevent.h
    formats/midi/event/midievent.cpp
    formats/midi/event/midievent.h
    formats/midi/event/sysexevent.cpp
    formats/midi/event/sysexevent.h
    formats/midi/midifile.cpp
    formats/midi/midifile.h
    formats/midi/midiinput.cpp
    formats/midi/midiinput.h
    formats/midi/midipoller.cpp
    formats/midi/midipoller.h
    formats/midi/util.cpp
    formats/midi/util.h
    formats/midi/midi.h
    formats/midi/midi.cpp
    commands/setchanneltypecommand.h
    commands/setchanneltypecommand.cpp
    commands/setpcmchannelsettingscommand.h
    commands/setpcmchannelsettingscommand.cpp
    tools/fmimport/opnimportdialog.h
    tools/fmimport/opnimportdialog.cpp
    tools/fmimport/opnimportdialog.ui
    tools/fmimport/patchtablemodel.h
    tools/fmimport/patchtablemodel.cpp
    tools/fmimport/tableview.h
    tools/fmimport/tableview.cpp
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
    common/ganttwidget/ganttmarker.h
    common/ganttwidget/ganttmarker.cpp
    opn/opnwidget.h
    opn/opnwidget.cpp
    opn/opnwidget.ui
    opn/opnwidgetwindow.h
    opn/opnwidgetwindow.cpp
    opn/opnwidgetwindow.ui
    commands/addtracksettingschangecommand.h
    commands/addtracksettingschangecommand.cpp
    commands/addplaylistlfochangecommand.h
    commands/addplaylistlfochangecommand.cpp
    opn/lfowidget.h
    opn/lfowidget.cpp
    opn/lfowidget.ui
    commands/removetracksettingschangecommand.h
    commands/removetracksettingschangecommand.cpp
    commands/removeplaylistlfochangecommand.h
    commands/removeplaylistlfochangecommand.cpp
    project/channel/lfosettings.h
    project/channel/lfosettings.cpp
    opn/fmlfowidget.h
    opn/fmlfowidget.cpp
    opn/fmlfowidget.ui
    commands/editlfosettingscommand.h
    commands/editlfosettingscommand.cpp
    common/ganttwidget/ganttscalewidget.h
    common/ganttwidget/ganttscalewidget.cpp
    dialogs/pianorollwidget/settingschangefooterwidget.h
    dialogs/pianorollwidget/settingschangefooterwidget.cpp
    dialogs/pianorollwidget/settingschangefooterwidget.ui
    dialogs/pianorollwidget/settingschangewidget.h
    dialogs/pianorollwidget/settingschangewidget.cpp
    dialogs/pianorollwidget/settingschangewidget.ui
    chromasound/emu/audio_output.h
    chromasound/emu/blargg_common.h
    chromasound/emu/blargg_endian.h
    chromasound/emu/blargg_source.h
    chromasound/emu/Blip_Buffer.cc
    chromasound/emu/Blip_Buffer.h
    chromasound/emu/Classic_Emu.cc
    chromasound/emu/Classic_Emu.h
    chromasound/emu/consumer.h
    chromasound/emu/Data_Reader.cc
    chromasound/emu/Data_Reader.h
    chromasound/emu/Dual_Resampler.cc
    chromasound/emu/Dual_Resampler.h
    chromasound/emu/Effects_Buffer.cc
    chromasound/emu/Effects_Buffer.h
    chromasound/emu/Fir_Resampler.cc
    chromasound/emu/Fir_Resampler.h
    chromasound/emu/gme.cc
    chromasound/emu/gme.h
    chromasound/emu/Gme_File.cc
    chromasound/emu/Gme_File.h
    chromasound/emu/gme_types.h
    chromasound/emu/M3u_Playlist.cc
    chromasound/emu/M3u_Playlist.h
    chromasound/emu/Multi_Buffer.cc
    chromasound/emu/Multi_Buffer.h
    chromasound/emu/Music_Emu.cc
    chromasound/emu/Music_Emu.h
    chromasound/emu/producer.h
    chromasound/emu/Sms_Apu.cc
    chromasound/emu/Sms_Apu.h
    chromasound/emu/Sms_Oscs.h
    chromasound/emu/Vgm_Emu.cc
    chromasound/emu/Vgm_Emu.h
    chromasound/emu/Vgm_Emu_Impl.cc
    chromasound/emu/Vgm_Emu_Impl.h
    chromasound/emu/Ym2413_Emu.cc
    chromasound/emu/Ym2413_Emu.h
    chromasound/emu/Ym2612_Emu.cc
    chromasound/emu/Ym2612_Emu.h
    chromasound/chromasound_emu.cpp
    chromasound/chromasound_emu.h
    dialogs/info/projectinfodialog.h
    dialogs/info/projectinfodialog.cpp
    dialogs/info/projectinfodialog.ui
    chromasound/emu/audio.h
    toplevel/browser/filesystemmodel.h
    toplevel/browser/filesystemmodel.cpp
    toplevel/browser/sortfilterproxymodel.h
    toplevel/browser/sortfilterproxymodel.cpp
    dialogs/settings/settingsdialog.h
    dialogs/settings/settingsdialog.cpp
    dialogs/settings/settingsdialog.ui
    Chromasound_Studio.h
    Chromasound_Studio.cpp
    tools/player/player.h
    tools/player/player.cpp
    tools/player/player.ui
    tools/player/playlisttablemodel.h
    tools/player/playlisttablemodel.cpp
    tools/player/playlistitem.h
    tools/player/playlistitem.cpp
    formats/gd3.h
    formats/gd3.cpp
    chromasound/emu/Ay_Apu.cc
    chromasound/emu/Ay_Apu.h
    ssg/ssgenvelopedisplaywidget.h
    ssg/ssgenvelopedisplaywidget.cpp
    ssg/ssgenvelopesettings.h
    ssg/ssgenvelopesettings.cpp
    ssg/ssgenvelopeshapewidget.h
    ssg/ssgenvelopeshapewidget.cpp
    ssg/ssgenvelopeshapewidget.ui
    ssg/ssgenvelopefreqwidget.h
    ssg/ssgenvelopefreqwidget.cpp
    ssg/ssgenvelopefreqwidget.ui
    ssg/ssgnoisefreqwidget.h
    ssg/ssgnoisefreqwidget.cpp
    ssg/ssgnoisefreqwidget.ui
    ssg/ssgwidget.h
    ssg/ssgwidget.cpp
    ssg/ssgwidget.ui
    project/channel/ssgchannelsettings.h
    project/channel/ssgchannelsettings.cpp
    commands/editssgchannelsettingscommand.h
    commands/editssgchannelsettingscommand.cpp
    commands/addplaylistnoisefrequencychangecommand.h
    commands/addplaylistnoisefrequencychangecommand.cpp
    commands/addplaylistenvelopefrequencychangecommand.h
    commands/addplaylistenvelopefrequencychangecommand.cpp
    commands/addplaylistenvelopeshapechangecommand.h
    commands/addplaylistenvelopeshapechangecommand.cpp
    toplevel/playlistwidget/playlistchangewidget.h
    toplevel/playlistwidget/playlistchangewidget.cpp
    toplevel/playlistwidget/playlistchangewidget.ui
    commands/removeplaylistnoisefrequencychangecommand.h
    commands/removeplaylistnoisefrequencychangecommand.cpp
    commands/removeplaylistenvelopefrequencychangecommand.h
    commands/removeplaylistenvelopefrequencychangecommand.cpp
    commands/removeplaylistenvelopeshapechangecommand.h
    commands/removeplaylistenvelopeshapechangecommand.cpp
    opl/usertone/oplenvelopesettings.h
    opl/usertone/oplenvelopesettings.cpp
    opl/usertone/oploperatorsettings.h
    opl/usertone/oploperatorsettings.cpp
    opl/usertone/oplenvelopedisplaywidget.h
    opl/usertone/oplenvelopedisplaywidget.cpp
    opl/usertone/oploperatorwidget.h
    opl/usertone/oploperatorwidget.cpp
    opl/usertone/oploperatorwidget.ui
    opl/usertone/oplwidget.h
    opl/usertone/oplwidget.cpp
    opl/usertone/oplwidget.ui
    opl/usertone/oplsettings.h
    opl/usertone/oplsettings.cpp
    commands/editoplsettingscommand.h
    commands/editoplsettingscommand.cpp
    commands/addplaylistusertonechangecommand.h
    commands/addplaylistusertonechangecommand.cpp
    commands/removeplaylistusertonechangecommand.h
    commands/removeplaylistusertonechangecommand.cpp
    commands/editplaylistlfochangecommand.h
    commands/editplaylistlfochangecommand.cpp
    commands/editplaylistnoisefrequencychangecommand.h
    commands/editplaylistnoisefrequencychangecommand.cpp
    commands/editplaylistenvelopefrequencychangecommand.h
    commands/editplaylistenvelopefrequencychangecommand.cpp
    commands/editssgenvelopesettingscommand.h
    commands/editssgenvelopesettingscommand.cpp
    project/channel/melodychannelsettings.h
    project/channel/melodychannelsettings.cpp
    project/channel/rhythmchannelsettings.h
    project/channel/rhythmchannelsettings.cpp
    opl/melodywidget.h opl/melodywidget.cpp
    opl/melodywidget.ui
    opl/rhythmwidget.h opl/rhythmwidget.cpp
    opl/rhythmwidget.ui
    commands/editmelodychannelsettingscommand.h
    commands/editmelodychannelsettingscommand.cpp
    commands/editrhythmchannelsettingscommand.h
    commands/editrhythmchannelsettingscommand.cpp
    tools/fmimport/oplimportdialog.h
    tools/fmimport/oplimportdialog.cpp
    tools/fmimport/oplimportdialog.ui
    globals/fmglobalswidget.h
    globals/fmglobalswidget.cpp
    globals/fmglobalswidget.ui
    globals/melodyglobalswidget.cpp
    globals/melodyglobalswidget.h
    globals/melodyglobalswidget.ui
    globals/fmglobalswidget.cpp
    globals/fmglobalswidget.h
    globals/fmglobalswidget.ui
    globals/ssgglobalswidget.cpp
    globals/ssgglobalswidget.h
    globals/ssgglobalswidget.ui
    commands/editprojectlfomodecommand.h
    commands/editprojectlfomodecommand.cpp
    commands/editprojectnoisefrequencycommand.h
    commands/editprojectnoisefrequencycommand.cpp
    commands/editprojectenvelopefrequencycommand.h
    commands/editprojectenvelopefrequencycommand.cpp
    chromasound/emu/2413tone.h
    chromasound/emu/2423tone.h
    chromasound/emu/281btone.h
    globals/melodypatchsetwidget.h
    globals/melodypatchsetwidget.cpp
    globals/melodypatchsetwidget.ui
    opl/opll.h
    opl/opll.cpp
    commands/editprojectoplltypecommand.h
    commands/editprojectoplltypecommand.cpp
    globals/unselectablestringlistmodel.h
    globals/unselectablestringlistmodel.cpp
    chromasound/chromasound_dual.h
    chromasound/chromasound_dual.cpp
    dialogs/settings/emulationsettingswidget.h
    dialogs/settings/emulationsettingswidget.cpp
    dialogs/settings/emulationsettingswidget.ui
    dialogs/settings/chromasoundlayoutwidget.h
    dialogs/settings/chromasoundlayoutwidget.cpp
    dialogs/settings/chromasoundlayoutwidget.ui
    commands/setpatternnamecommand.h
    commands/setpatternnamecommand.cpp
    commands/movepatternupcommand.h
    commands/movepatternupcommand.cpp
    commands/movepatterndowncommand.h
    commands/movepatterndowncommand.cpp
    commands/deletepatterncommand.h
    commands/deletepatterncommand.cpp
    commands/insertpatterncommand.h
    commands/insertpatterncommand.cpp
    commands/duplicatepatterncommand.h
    commands/duplicatepatterncommand.cpp
    commands/setprojectinfocommand.h
    commands/setprojectinfocommand.cpp
    tools/rombuilder/rombuilderdialog.h
    tools/rombuilder/rombuilderdialog.cpp
    tools/rombuilder/rombuilderdialog.ui
    tools/rombuilder/rombuildertablemodel.h
    tools/rombuilder/rombuildertablemodel.cpp
    formats/rom.h formats/rom.cpp
    globals/romtablemodel.h
    globals/romtablemodel.cpp
    project/channel/pcmchannelsettings.h
    project/channel/pcmchannelsettings.cpp
    pcm/pcmwidget.h
    pcm/pcmwidget.cpp
    pcm/pcmwidget.ui
    commands/editpcmchannelsettingscommand.h
    commands/editpcmchannelsettingscommand.cpp
    toplevel/browser/filesystemtreeview.h
    toplevel/browser/filesystemtreeview.cpp
    commands/composite/loadmultitrackmidicommand.h
    commands/composite/loadmultitrackmidicommand.cpp
    pcm/pcmwidgettablemodel.h
    pcm/pcmwidgettablemodel.cpp
    globals/pcmglobalswidget.h
    globals/pcmglobalswidget.cpp
    globals/pcmglobalswidget.ui
    globals/pcmglobalswindow.h
    globals/pcmglobalswindow.cpp
    globals/pcmglobalswindow.ui
    commands/setprojectpcmfilecommand.h
    commands/setprojectpcmfilecommand.cpp
    dialogs/info/projectinfoscreendialog.h
    dialogs/info/projectinfoscreendialog.cpp
    dialogs/info/projectinfoscreendialog.ui
    formats/midi/programs.h
    dialogs/pianorollwidget/pianorollvelocitydialog.h
    dialogs/pianorollwidget/pianorollvelocitydialog.cpp
    dialogs/pianorollwidget/pianorollvelocitydialog.ui
    pcm/pcmwidgetwindow.h
    pcm/pcmwidgetwindow.cpp
    pcm/pcmwidgetwindow.ui
    commands/setchannelenabledcommand.h
    commands/setchannelenabledcommand.cpp
    common/damagewidget/damagewidget.h
    common/damagewidget/damagewidget.cpp
    dialogs/settings/emulatoroutputdevicesettingswidget.h
    dialogs/settings/emulatoroutputdevicesettingswidget.cpp
    dialogs/settings/emulatoroutputdevicesettingswidget.ui
    dialogs/settings/profilesettingswidget.h
    dialogs/settings/profilesettingswidget.cpp
    dialogs/settings/profilesettingswidget.ui
    common/recordiconengine.h
    common/recordiconengine.cpp
    dialogs/pianorollwidget/pianorollquantizationdivisiondialog.h
    dialogs/pianorollwidget/pianorollquantizationdivisiondialog.cpp
    dialogs/pianorollwidget/pianorollquantizationdivisiondialog.ui
    toplevel/topwidget/midipadsdialog.h
    toplevel/topwidget/midipadsdialog.cpp
    toplevel/topwidget/midipadsdialog.ui
    dialogs/pianorollwidget/pianorollpitchwidget.h
    dialogs/pianorollwidget/pianorollpitchwidget.cpp
    commands/addpitchchangecommand.h
    commands/addpitchchangecommand.cpp
    commands/editpitchchangecommand.h
    commands/editpitchchangecommand.cpp
    commands/removepitchchangecommand.h
    commands/removepitchchangecommand.cpp
    commands/setchannelpitchrangecommand.h
    commands/setchannelpitchrangecommand.cpp
)

if (LINUX)
    set (PROJECT_SOURCES
            ${PROJECT_SOURCES}
            chromasound/chromasound_direct.cpp
            chromasound/chromasound_direct.h
            chromasound/direct/vgmplayer.cpp
            chromasound/direct/vgmplayer.h
            chromasound/direct/gpio.h
            chromasound/direct/gpio.cpp
            chromasound/direct/spi.h
            chromasound/direct/spi.cpp
    )
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if (WIN32)
        set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/resources/chromasound-studio.rc")
        qt_add_executable(Chromasound-Studio
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            resources.qrc
            ${app_icon_resource_windows}
        )
    elseif (LINUX)
        qt_add_executable(Chromasound-Studio
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            resources.qrc
        )
    elseif (APPLE)
        set(MACOSX_BUNDLE_ICON_FILE chromasound-studio.icns)
        set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/chromasound-studio.icns")
        set_source_files_properties(${app_icon_macos} PROPERTIES
                   MACOSX_PACKAGE_LOCATION "Resources")
        qt_add_executable(Chromasound-Studio
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            resources.qrc
            ${app_icon_macos}
        )
    endif()
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Chromasound-Studio APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Chromasound-Studio SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Chromasound-Studio
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if (LINUX)
    target_link_libraries(Chromasound-Studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                    bson-1.0
                                                    portaudio
                                                    portmidi
                                                    z)
elseif (WIN32)
    if (WIN32_ARM)
        target_link_libraries(Chromasound-Studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                    "C:/Program Files (Arm)/mongo-c-driver/lib/bson-1.0.lib"
                                                    "C:/Program Files (Arm)/portaudio/lib/portaudio_x64.lib"
                                                    "C:/Program Files (Arm)/portmidi/lib/portmidi.lib"
                                                    "C:/Program Files (Arm)/zlib/lib/zlib.lib")
    else()
        target_link_libraries(Chromasound-Studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                    "C:/Program Files/mongo-c-driver/lib/bson-1.0.lib"
                                                    "C:/Program Files/portaudio/lib/portaudio_x64.lib"
                                                    "C:/Program Files/portmidi/lib/portmidi.lib"
                                                    "C:/Program Files/zlib/lib/zlib.lib")
    endif()
elseif (APPLE)
    target_link_libraries(Chromasound-Studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                "/System/Volumes/Data/usr/local/lib/libbson-1.0.dylib"
                                                "/System/Volumes/Data/usr/local/lib/libportaudio.dylib"
                                                "/System/Volumes/Data/usr/local/lib/libportmidi.dylib"
                                                "/System/Volumes/Data/usr/local/lib/libz.dylib")
endif()


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Chromasound-Studio)
endif()
set_target_properties(Chromasound-Studio PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Chromasound-Studio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (LINUX)
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/unix_install/chromasound-studio.svg DESTINATION /usr/share/icons)
  if (NOT EXISTS /usr/share/desktop-directories/chromasound.directory)
    install(FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/unix_install/chromasound.directory DESTINATION /usr/share/desktop-directories)
  endif()
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/unix_install/ChromasoundStudio.desktop DESTINATION /usr/share/applications)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Chromasound-Studio)
endif()

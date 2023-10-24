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
    fmwidget/fmwidget.cpp
    fmwidget/fmwidget.h
    fmwidget/fmwidget.ui
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
    midi/midiinput.cpp
    midi/midiinput.h
    midi/midipoller.cpp
    midi/midipoller.h
    noisewidget/noisewidget.cpp
    noisewidget/noisewidget.h
    noisewidget/noisewidget.ui
    pianorollwidget/pianorollkeyswidget.cpp
    pianorollwidget/pianorollkeyswidget.h
    pianorollwidget/pianorollvelocitieswidget.cpp
    pianorollwidget/pianorollvelocitieswidget.h
    pianorollwidget/pianorollwidget.cpp
    pianorollwidget/pianorollwidget.h
    pianorollwidget/pianorollwidget.ui
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
                                            /usr/lib64/libbson-1.0.so)

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

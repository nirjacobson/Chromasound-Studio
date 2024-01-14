#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRect>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>

#include "application.h"
#include "bson.h"
#include "channelswidget/channelswidget.h"
#include "noisewidget/noisewidget.h"
#include "playlistwidget/playlistwidget.h"
#include "pianorollwidget/pianorollwidget.h"
#include "fmwidget/fmwidgetwindow.h"
#include "pcmwidget/pcmwidget.h"
#include "ssg/ssgwidget.h"
#include "project/vgmstream.h"
#include "midi/midiinput.h"
#include "midi/midipoller.h"
#include "style/styledialog.h"
#include "mdiarea/mdisubwindow.h"
#include "fmimport/opnimportdialog.h"
#include "fmimport/oplimportdialog.h"
#include "pcmusage/pcmusagedialog.h"
#include "info/projectinfodialog.h"
#include "settings/settingsdialog.h"
#include "player/player.h"
#include "globals/melodyglobalswidget.h"
#include "globals/ssgglobalswidget.h"
#include "globals/fmglobalswidget.h"
#include "opl/melodywidget.h"
#include "opl/rhythmwidget.h"
#include "playlistwidget/lfowidget.h"

#include "commands/movechanneldowncommand.h"
#include "commands/movechannelupcommand.h"
#include "commands/deletechannelcommand.h"
#include "commands/addchannelcommand.h"
#include "commands/setchanneltypecommand.h"

#include "browser/filesystemmodel.h"
#include "browser/sortfilterproxymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr, Application* app = nullptr);
        ~MainWindow();

        void moveChannelUp(const int index);
        void moveChannelDown(const int index);
        Channel deleteChannel(const int index);
        void addChannel(const int index, const Channel& channel);
        void addChannel();
        int getChannelVolume(const int index);
        void setChannelVolume(const int index, const int volume);

        int channels() const;

        void doUpdate();
        void channelSettingsUpdated();

        void setChannelType(Channel& channel, const Channel::Type type);

        Application* app();

    private slots:
        void play();
        void pause();
        void stop();

        void patternChanged(int num);
        void beatsPerBarChanged(int beatsPerBar);

        void frame();

        void pianoRollTriggered(const int index, const bool on);
        void channelSelected(const int index);
        void channelNameChanged(const int index);

        void newTriggered();
        void openTriggered();
        void saveTriggered();
        void renderForFMPSGTriggered();
        void renderFor3rdPartyTriggered();

        void keyOn(const int key, const int velocity);
        void keyOff(const int key);
        void handleMIDIMessage(const long msg);
        void setMIDIDevice(const int device);

        void projectInfoTriggered();
        void settingsTriggered();
        void stylesTriggered();
        void pcmUsageTriggered();
        void opnImportTriggered();
        void oplImportTriggered();
        void playerTriggered();
        void fmGlobalsTriggered();
        void ssgGlobalsTriggered();
        void melodyGlobalsTriggered();

        void showChannelsWindow();
        void showPlaylistWindow();

        void mdiViewModeChanged(const QString& viewMode);

        void pcmUploadStarted();
        void pcmUploadFinished();
        void compileStarted();
        void compileFinished();

    private:
        Ui::MainWindow* ui;

        Application* _app;

        MIDIInput* _midiInput;
        MIDIPoller _midiPoller;

        ChannelsWidget* _channelsWidget;
        PlaylistWidget* _playlistWidget;

        MdiSubWindow* _channelsWindow;
        MdiSubWindow* _playlistWindow;

        QMap<int, QList<MdiSubWindow*>> _channelWindows;

        SettingsDialog* _settingsDialog;
        ProjectInfoDialog* _infoDialog;
        StyleDialog* _styleDialog;
        OPNImportDialog* _opnImportDialog;
        OPLImportDialog* _oplImportDialog;
        PCMUsageDialog* _pcmUsageDialog;
        Player* _player;
        FMGlobalsWidget* _fmGlobalsWidget;
        SSGGlobalsWidget* _ssgGlobalsWidget;
        MelodyGlobalsWidget* _melodyGlobalsWidget;

        MdiSubWindow* _settingsDialogWindow;
        MdiSubWindow* _infoDialogWindow;
        MdiSubWindow* _styleDialogWindow;
        MdiSubWindow* _opnImportDialogWindow;
        MdiSubWindow* _oplImportDialogWindow;
        MdiSubWindow* _pcmUsageDialogWindow;
        MdiSubWindow* _playerDialogWindow;
        MdiSubWindow* _fmGlobalsWindow;
        MdiSubWindow* _ssgGlobalsWindow;
        MdiSubWindow* _melodyGlobalsWindow;

        FilesystemModel _filesystemModel;
        SortFilterProxyModel _proxyModel;

        int _selectedChannel;

        QTimer _timer;

        void windowClosed(MdiSubWindow* window);

        // QWidget interface
    protected:
        void showEvent(QShowEvent*);
        void resizeEvent(QResizeEvent*);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};
#endif // MAINWINDOW_H

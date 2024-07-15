#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRect>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTreeView>
#include <QSplitter>

#include "application.h"
#include "formats/bson.h"
#include "toplevel/channelswidget/channelswidget.h"
#include "psg/noisewidget.h"
#include "toplevel/playlistwidget/playlistwidget.h"
#include "toplevel/browser/filesystemtreeview.h"
#include "dialogs/pianorollwidget/pianorollwidget.h"
#include "opn/opnwidgetwindow.h"
#include "rom/romwidget.h"
#include "ssg/ssgwidget.h"
#include "project/vgmstream.h"
#include "formats/midi/midiinput.h"
#include "formats/midi/midipoller.h"
#include "dialogs/style/styledialog.h"
#include "common/mdiarea/mdisubwindow.h"
#include "tools/fmimport/opnimportdialog.h"
#include "tools/fmimport/oplimportdialog.h"
#include "tools/rombuilder/rombuilderdialog.h"
#include "dialogs/info/projectinfodialog.h"
#include "dialogs/info/projectinfoscreendialog.h"
#include "dialogs/settings/settingsdialog.h"
#include "tools/player/player.h"
#include "globals/melodyglobalswidget.h"
#include "globals/ssgglobalswidget.h"
#include "globals/fmglobalswidget.h"
#include "globals/romglobalswindow.h"
#include "opl/melodywidget.h"
#include "opl/rhythmwidget.h"
#include "opn/lfowidget.h"

#include "commands/movechanneldowncommand.h"
#include "commands/movechannelupcommand.h"
#include "commands/deletechannelcommand.h"
#include "commands/addchannelcommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/composite/loadmultitrackmidicommand.h"

#include "toplevel/browser/filesystemmodel.h"
#include "toplevel/browser/sortfilterproxymodel.h"

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

        void movePatternUp(const int index);
        void movePatternDown(const int index);
        QList<Playlist::Item*> deletePattern(const int index);
        void insertPattern(const int index, Pattern* pattern = nullptr, const QList<Playlist::Item*>& items = QList<Playlist::Item*>());

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
        void saveAsTriggered();
        void publishTriggered();
        void renderForChromasoundTriggered();
        void renderFor3rdPartyTriggered();

        void keyOn(const int key, const int velocity);
        void keyOff(const int key);
        void handleMIDIMessage(const long msg);
        void setMIDIDevice(const int device);

        void projectInfoTriggered();
        void settingsTriggered();
        void stylesTriggered();
        void opnImportTriggered();
        void oplImportTriggered();
        void playerTriggered();
        void romBuilderTriggered();
        void fmGlobalsTriggered();
        void ssgGlobalsTriggered();
        void melodyGlobalsTriggered();
        void romGlobalsTriggered();

        void showChannelsWindow();
        void showPlaylistWindow();

        void mdiViewModeChanged(const QString& viewMode);

        void pcmUploadStarted();
        void pcmUploadFinished();
        void compileStarted();
        void compileFinished();

        void splitterMoved(int,int);

        void loadEmptyTemplate();
        void loadPSGTemplate();
        void loadFM4Template();
        void loadFM4PSGTemplate();
        void loadSSGTemplate();
        void loadFM2Template();
        void loadFM2SSGTemplate();

    private:
        Ui::MainWindow* ui;
        QTreeView* _treeView;
        MdiArea* _mdiArea;
        QSplitter* _splitter;

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
        ROMBuilderDialog* _romBuilderDialog;
        Player* _player;
        FMGlobalsWidget* _fmGlobalsWidget;
        SSGGlobalsWidget* _ssgGlobalsWidget;
        MelodyGlobalsWidget* _melodyGlobalsWidget;
        ROMGlobalsWindow* _romGlobalsWindow;

        MdiSubWindow* _settingsDialogWindow;
        MdiSubWindow* _infoDialogWindow;
        MdiSubWindow* _infoScreenDialogWindow;
        MdiSubWindow* _styleDialogWindow;
        MdiSubWindow* _opnImportDialogWindow;
        MdiSubWindow* _oplImportDialogWindow;
        MdiSubWindow* _playerDialogWindow;
        MdiSubWindow* _romBuilderDialogWindow;
        MdiSubWindow* _fmGlobalsWindow;
        MdiSubWindow* _ssgGlobalsWindow;
        MdiSubWindow* _melodyGlobalsWindow;
        MdiSubWindow* _romGlobalsWindowWindow;

        FilesystemModel _filesystemModel;
        SortFilterProxyModel _proxyModel;

        int _selectedChannel;

        QAction _actionPSG;
        QAction _actionFM4;
        QAction _actionFM4PSG;
        QAction _actionSSG;
        QAction _actionFM2;
        QAction _actionFM2SSG;

        QTimer _timer;

        void windowClosed(MdiSubWindow* window);

        void preLoad();
        void load(const QString& path);
        void postLoad();

        // QWidget interface
    protected:
        void showEvent(QShowEvent*);
        void resizeEvent(QResizeEvent*);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QTimer>
#include <QRect>
#include <QFile>
#include <QFileDialog>

#include "application.h"
#include "bson.h"
#include "channelswidget/channelswidget.h"
#include "noisewidget/noisewidget.h"
#include "playlistwidget/playlistwidget.h"
#include "pianorollwidget/pianorollwidget.h"
#include "fmwidget/fmwidget.h"
#include "project/vgmstream.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Application* app = nullptr);
    ~MainWindow();

private slots:
    void play();
    void pause();
    void stop();

    void patternChanged(int num);
    void beatsPerBarChanged(int);

    void frame();

    void pianoRollTriggered(const int index);
    void deleteChannelTriggered(const int index);
    void channelAdded();
    void moveChannelUpTriggered(const int index);
    void moveChannelDownTriggered(const int index);

    void channelSelected(const int index);

    void toneTriggered(const int index);
    void noiseTriggered(const int index);
    void fmTriggered(const int index);

    void channelNameChanged(const int index);

    void openTriggered();
    void saveTriggered();
    void renderTriggered();

private:
    Ui::MainWindow* ui;

    Application* _app;

    ChannelsWidget* _channelsWidget;
    PlaylistWidget* _playlistWidget;
    PianoRollWidget* _pianoRollWidget;

    NoiseWidget* _noiseWidget;
    FMWidget* _fmWidget;

    QMdiSubWindow* _channelsWindow;
    QMdiSubWindow* _playlistWindow;
    QMdiSubWindow* _pianoRollWindow;
    QMdiSubWindow* _channelWindow;

    int _selectedChannel;


    QTimer _timer;

    void doUpdate();

    // QWidget interface
    protected:
    void showEvent(QShowEvent*);
};
#endif // MAINWINDOW_H

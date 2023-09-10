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
#include "playlistwidget/playlistwidget.h"
#include "pianorollwidget/pianorollwidget.h"

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

    void openTriggered();
    void saveTriggered();

private:
    Ui::MainWindow* ui;

    Application* _app;

    ChannelsWidget* _channelsWidget;
    PlaylistWidget* _playlistWidget;
    PianoRollWidget* _pianoRollWidget;

    QMdiSubWindow* _channelsWindow;
    QMdiSubWindow* _playlistWindow;
    QMdiSubWindow* _pianoRollWindow;

    QTimer _timer;

    void doUpdate();

    // QWidget interface
    protected:
    void showEvent(QShowEvent*);
};
#endif // MAINWINDOW_H

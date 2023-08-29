#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _channelsWidget(new ChannelsWidget(this, app))
    , _playlistWidget(new PlaylistWidget(this, app))
{
    ui->setupUi(this);

    ui->topWidget->setApplication(app);

   _channelsWindow = new QMdiSubWindow(ui->mdiArea);
   _channelsWindow->setWidget(_channelsWidget);
   ui->mdiArea->addSubWindow(_channelsWindow);
   _channelsWindow->show();

   _playlistWindow = new QMdiSubWindow(ui->mdiArea);
   _playlistWindow->setWidget(_playlistWidget);
   ui->mdiArea->addSubWindow(_playlistWindow);
   _playlistWindow->show();

   _pianoRollWindow = new QMdiSubWindow(ui->mdiArea);
   ui->mdiArea->addSubWindow(_pianoRollWindow);
   _pianoRollWindow->hide();

   _timer.setInterval(1000 / 30);
   connect(&_timer, &QTimer::timeout, this, &MainWindow::frame);

   connect(ui->topWidget, &TopWidget::play, this, &MainWindow::play);
   connect(ui->topWidget, &TopWidget::pause, this, &MainWindow::pause);
   connect(ui->topWidget, &TopWidget::stop, this, &MainWindow::stop);
   connect(ui->topWidget, &TopWidget::patternChanged, this, &MainWindow::patternChanged);
   connect(ui->topWidget, &TopWidget::beatsPerBarChanged, this, &MainWindow::beatsPerBarChanged);

   connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
}

MainWindow::~MainWindow()
{
    delete _channelsWindow;
    delete _playlistWindow;
    delete ui;
}

void MainWindow::play()
{
    _timer.start();
    _app->play();
}

void MainWindow::pause()
{
    _app->pause();
    _timer.stop();
}

void MainWindow::stop()
{
    _app->stop();
    _timer.stop();
    doUpdate();
}

void MainWindow::patternChanged(int num)
{
    _app->setActivePattern(num - 1);
    _channelsWidget->update();
}

void MainWindow::beatsPerBarChanged(int beats)
{
    _channelsWidget->update();
    _playlistWidget->update();
}

void MainWindow::frame()
{
    doUpdate();
}

void MainWindow::pianoRollTriggered(const int index)
{
    _app->project().getPattern(_app->activePattern()).getTrack(index).usePianoRoll();
    _channelsWidget->update();

    _pianoRollWindow->show();
    _pianoRollWindow->setFocus();
}

void MainWindow::doUpdate()
{
    update();
    _channelsWidget->update();
    _playlistWidget->update();
}

void MainWindow::showEvent(QShowEvent* event)
{
    _playlistWindow->resize(ui->mdiArea->frameGeometry().width()/2, ui->mdiArea->frameGeometry().height());
    _playlistWindow->move(ui->mdiArea->frameGeometry().width()/2, 0);
}


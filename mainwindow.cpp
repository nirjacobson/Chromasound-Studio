#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _channelsWidget(new ChannelsWidget(this, app))
    , _playlistWidget(new PlaylistWidget(this, app))
    , _pianoRollWidget(nullptr)
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

   connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openTriggered);
   connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveTriggered);
}

MainWindow::~MainWindow()
{
    if (_pianoRollWidget) delete _pianoRollWidget;
    delete _playlistWindow;
    delete _channelsWindow;
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
    _app->project().setFrontPattern(num - 1);
    _channelsWidget->update();
}

void MainWindow::beatsPerBarChanged(int)
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
    _app->project().getFrontPattern().getTrack(index).usePianoRoll();
    _channelsWidget->update();

    _pianoRollWidget = new PianoRollWidget(this, _app);
    _pianoRollWidget->setTrack(_app->project().frontPattern(), index);

    _pianoRollWindow->setWidget(_pianoRollWidget);

    _pianoRollWindow->show();
    _pianoRollWindow->setFocus();
}

void MainWindow::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "FM-PSG Studio Projects (*.fsp)");
    Project p = BSON::decode(path);
    fprintf(stderr, "%d\n", p.channels());
}

void MainWindow::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "FM-PSG Studio Projects (*.fsp)");
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(BSON::encode(_app->project()));
    file.close();

}

void MainWindow::doUpdate()
{
    update();
    _channelsWidget->update();
    _playlistWidget->update();
    if (_pianoRollWidget) _pianoRollWidget->update();
}

void MainWindow::showEvent(QShowEvent*)
{
    int width = ui->mdiArea->frameGeometry().width()/2;
    int height = ui->mdiArea->frameGeometry().height();

    _playlistWindow->resize(width, height);
    _playlistWindow->move(width, 0);

    _pianoRollWindow->resize(width, height);
    _pianoRollWindow->move(width, 0);
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _channelsWidget(new ChannelsWidget(this, app))
    , _playlistWidget(new PlaylistWidget(this, app))
    , _pianoRollWidget(nullptr)
    , _noiseWidget(nullptr)
{
    ui->setupUi(this);

    ui->topWidget->setApplication(app);

   _channelsWindow = new QMdiSubWindow(ui->mdiArea);
   _channelsWindow->setWidget(_channelsWidget);
   _channelsWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
   ui->mdiArea->addSubWindow(_channelsWindow);
   _channelsWindow->show();

   _playlistWindow = new QMdiSubWindow(ui->mdiArea);
   _playlistWindow->setWidget(_playlistWidget);
   ui->mdiArea->addSubWindow(_playlistWindow);
   _playlistWindow->show();

   _pianoRollWindow = new QMdiSubWindow(ui->mdiArea);
   ui->mdiArea->addSubWindow(_pianoRollWindow);
   _pianoRollWindow->hide();

   _channelWindow = new QMdiSubWindow(ui->mdiArea);
   ui->mdiArea->addSubWindow(_channelWindow);
   _channelWindow->hide();

   _timer.setInterval(1000 / 30);
   connect(&_timer, &QTimer::timeout, this, &MainWindow::frame);

   connect(ui->topWidget, &TopWidget::play, this, &MainWindow::play);
   connect(ui->topWidget, &TopWidget::pause, this, &MainWindow::pause);
   connect(ui->topWidget, &TopWidget::stop, this, &MainWindow::stop);
   connect(ui->topWidget, &TopWidget::patternChanged, this, &MainWindow::patternChanged);
   connect(ui->topWidget, &TopWidget::beatsPerBarChanged, this, &MainWindow::beatsPerBarChanged);

   connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
   connect(_channelsWidget, &ChannelsWidget::deleteTriggered, this, &MainWindow::deleteChannelTriggered);
   connect(_channelsWidget, &ChannelsWidget::channelAdded, this, &MainWindow::channelAdded);
   connect(_channelsWidget, &ChannelsWidget::moveUpTriggered, this, &MainWindow::moveChannelUpTriggered);
   connect(_channelsWidget, &ChannelsWidget::moveDownTriggered, this, &MainWindow::moveChannelDownTriggered);
   connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);

   connect(_channelsWidget, &ChannelsWidget::toneTriggered, this, &MainWindow::toneTriggered);
   connect(_channelsWidget, &ChannelsWidget::noiseTriggered, this, &MainWindow::noiseTriggered);

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

    PianoRollWidget* oldWidget = _pianoRollWidget;
    _pianoRollWidget = new PianoRollWidget(this, _app);
    _pianoRollWidget->setTrack(_app->project().frontPattern(), index);

    _pianoRollWindow->setWidget(_pianoRollWidget);
    delete oldWidget;

    _pianoRollWindow->show();
    _pianoRollWindow->setFocus();
}

void MainWindow::deleteChannelTriggered(const int index)
{
    _app->project().removeChannel(index);
    _channelsWidget->remove(index);
}

void MainWindow::channelAdded()
{
    _app->project().addChannel();
    _channelsWidget->add(_app->project().channels() - 1);
}

void MainWindow::moveChannelUpTriggered(const int index)
{
    if (index != 0) {
        _app->project().moveChannelUp(index);

        _channelsWidget->update(index);
        _channelsWidget->update(index-1);
    }

    _channelsWidget->update();
}

void MainWindow::moveChannelDownTriggered(const int index)
{
    if (index != _app->project().channels()-1) {
        _app->project().moveChannelDown(index);

        _channelsWidget->update(index);
        _channelsWidget->update(index+1);
    }

    _channelsWidget->update();
}

void MainWindow::channelSelected(const int index)
{
    _channelWindow->hide();

    if (_app->project().getChannel(index).type() == Channel::Type::NOISE) {
        NoiseWidget* oldWidget = _noiseWidget;
        _noiseWidget = new NoiseWidget(this);
        _noiseWidget->setWindowTitle(QString("%1: Noise").arg(_app->project().getChannel(index).name()));
        _noiseWidget->setData(dynamic_cast<NoiseChannelSettings*>(&_app->project().getChannel(index).data()));

        _channelWindow->setWidget(_noiseWidget);
        _channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        delete oldWidget;

        _channelWindow->show();
        _channelWindow->setFocus();
    } else {

    }
}

void MainWindow::toneTriggered(const int index)
{
    _app->project().getChannel(index).setType(Channel::Type::TONE);
}

void MainWindow::noiseTriggered(const int index)
{
    _app->project().getChannel(index).setType(Channel::Type::NOISE);
}

void MainWindow::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "FM-PSG Studio Projects (*.fsp)");
    _app->project() = BSON::decode(path);

    ui->topWidget->setPattern(_app->project().frontPattern());
    ui->topWidget->setTempo(_app->project().tempo());
    ui->topWidget->setBeatsPerBar(_app->project().beatsPerBar());

    _channelsWidget->rebuild();
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


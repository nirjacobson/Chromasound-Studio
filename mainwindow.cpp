#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _midiInput(MIDIInput::instance())
    , _channelsWidget(new ChannelsWidget(this, app))
    , _playlistWidget(new PlaylistWidget(this, app))
    , _pianoRollWidget(nullptr)
    , _noiseWidget(nullptr)
    , _fmWidget(nullptr)
{
    ui->setupUi(this);

    _midiInput->init();
    connect(&_midiPoller, &MIDIPoller::receivedMessage, this, &MainWindow::handleMIDIMessage);
    _midiPoller.start();

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
   connect(ui->topWidget, &TopWidget::midiDeviceSet, this, &MainWindow::setMIDIDevice);

   connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
   connect(_channelsWidget, &ChannelsWidget::deleteTriggered, this, &MainWindow::deleteChannelTriggered);
   connect(_channelsWidget, &ChannelsWidget::channelAdded, this, &MainWindow::channelAdded);
   connect(_channelsWidget, &ChannelsWidget::moveUpTriggered, this, &MainWindow::moveChannelUpTriggered);
   connect(_channelsWidget, &ChannelsWidget::moveDownTriggered, this, &MainWindow::moveChannelDownTriggered);
   connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);
   connect(_channelsWidget, &ChannelsWidget::nameChanged, this, &MainWindow::channelNameChanged);

   connect(_channelsWidget, &ChannelsWidget::toneTriggered, this, &MainWindow::toneTriggered);
   connect(_channelsWidget, &ChannelsWidget::noiseTriggered, this, &MainWindow::noiseTriggered);
   connect(_channelsWidget, &ChannelsWidget::fmTriggered, this, &MainWindow::fmTriggered);

   connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newTriggered);
   connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openTriggered);
   connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveTriggered);
   connect(ui->actionRender, &QAction::triggered, this, &MainWindow::renderTriggered);
}

MainWindow::~MainWindow()
{
    if (_pianoRollWidget) delete _pianoRollWidget;
    delete _playlistWindow;
    delete _channelsWindow;
    delete _channelWindow;
    delete _pianoRollWindow;

    _midiPoller.stop();
    _midiPoller.quit();
    _midiPoller.wait();

    _midiInput->destroy();

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
    connect(_pianoRollWidget, &PianoRollWidget::keyOn, this, &MainWindow::keyOn);
    connect(_pianoRollWidget, &PianoRollWidget::keyOff, this, &MainWindow::keyOff);

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
        bool isSelected = _channelsWidget->selected() == index;
        bool isOtherSelected = _channelsWidget->selected() == index - 1;

        _app->project().moveChannelUp(index);

        _channelsWidget->update(index);
        _channelsWidget->update(index-1);

        if (isSelected)
            _channelsWidget->select(index - 1);
        if (isOtherSelected)
            _channelsWidget->select(index);
    }
}

void MainWindow::moveChannelDownTriggered(const int index)
{
    if (index != _app->project().channels()-1) {
        bool isSelected = _channelsWidget->selected() == index;
        bool isOtherSelected = _channelsWidget->selected() == index + 1;

        _app->project().moveChannelDown(index);

        _channelsWidget->update(index);
        _channelsWidget->update(index+1);

        if (isSelected)
            _channelsWidget->select(index + 1);
        if (isOtherSelected)
            _channelsWidget->select(index);
    }
}

void MainWindow::channelSelected(const int index)
{
    _selectedChannel = index;

    _channelWindow->hide();
    if (_pianoRollWidget)
        _pianoRollWidget->setTrack(_app->project().frontPattern(), index);

    QWidget* oldWidget = nullptr;
    switch (_app->project().getChannel(index).type()) {
        case Channel::Type::NOISE:
            oldWidget = _noiseWidget;
            _noiseWidget = new NoiseWidget(this);
            _noiseWidget->setSettings(dynamic_cast<NoiseChannelSettings*>(&_app->project().getChannel(index).settings()));
            _noiseWidget->setWindowTitle(QString("%1: Noise").arg(_app->project().getChannel(index).name()));

            _channelWindow->setWidget(_noiseWidget);
            break;
        case Channel::Type::FM:
            oldWidget = _fmWidget;
            _fmWidget = new FMWidget(this);
            _fmWidget->setSettings(dynamic_cast<FMChannelSettings*>(&_app->project().getChannel(index).settings()));
            _fmWidget->setWindowTitle(QString("%1: FM").arg(_app->project().getChannel(index).name()));

            _channelWindow->setWidget(_fmWidget);
            break;
        case Channel::TONE:
            break;
    }

    if (_app->project().getChannel(index).type() > Channel::Type::TONE) {
        delete oldWidget;

        _channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);

        _channelWindow->show();
        _channelWindow->setFocus();
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

void MainWindow::fmTriggered(const int index)
{
    _app->project().getChannel(index).setType(Channel::Type::FM);
}

void MainWindow::channelNameChanged(const int index)
{
    if (index == _selectedChannel) {
        switch(_app->project().getChannel(index).type()) {
            case Channel::TONE:
                break;
            case Channel::NOISE:
                _noiseWidget->setWindowTitle(QString("%1: Noise").arg(_app->project().getChannel(index).name()));
                break;
            case Channel::FM:
                _fmWidget->setWindowTitle(QString("%1: FM").arg(_app->project().getChannel(index).name()));
                break;
        }
    }
}

void MainWindow::newTriggered()
{
    _app->project() = Project();

    ui->topWidget->updateFromProject(_app->project());

    _channelsWidget->rebuild();

    _playlistWidget->update();
}

void MainWindow::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "FM-PSG Studio Projects (*.fsp)");

    if (!path.isNull()) {
        _app->project() = BSON::decode(path);

        ui->topWidget->updateFromProject(_app->project());

        _channelsWidget->rebuild();

        _playlistWidget->update();
    }
}

void MainWindow::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "FM-PSG Studio Projects (*.fsp)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encode(_app->project()));
        file.close();
    }
}

void MainWindow::renderTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "VGM files (*.vgm)");
    QFile file(path);
    file.open(QIODevice::WriteOnly);

    VGMStream vgmStream;
    QByteArray data = vgmStream.compile(_app->project(), true);
    file.write(data);
    file.close();
}

void MainWindow::keyOn(const int key, const int velocity)
{
    int activeChannel = _channelsWidget->activeChannel();
    if (activeChannel >= 0) {
        Channel& channel = _app->project().getChannel(activeChannel);
        _app->keyOn(channel.type(), channel.settings(), key, velocity);
        if (_pianoRollWidget) _pianoRollWidget->pressKey(key);
    }
}

void MainWindow::keyOff(const int key)
{
    _app->keyOff(key);
    if (_pianoRollWidget) _pianoRollWidget->releaseKey(key);
}

void MainWindow::handleMIDIMessage(const long message)
{
    const char status = ((message >> 0) & 0xFF);
    const char data1 = ((message >> 8) & 0xFF);
    const char data2 = ((message >> 16) & 0xFF);

    if (status == 0x90) {
        keyOn(data1, qMin((int)data2, 100));
    } else if (status == 0x80) {
        keyOff(data1);
    }
}

void MainWindow::setMIDIDevice(const int device)
{
    _midiPoller.stop();
    _midiPoller.quit();
    _midiPoller.wait();

    _midiInput->setDevice(device);

    _midiPoller.start();
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

    _channelsWindow->move(0, 0);

    _playlistWindow->resize(width, height);
    _playlistWindow->move(width, 0);

    _pianoRollWindow->resize(width, height);
    _pianoRollWindow->move(width, 0);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    int width = ui->mdiArea->frameGeometry().width()/2;
    int height = ui->mdiArea->frameGeometry().height();

    _channelsWindow->move(0, 0);

    _playlistWindow->resize(width, height);
    _playlistWindow->move(width, 0);

    _pianoRollWindow->resize(width, height);
    _pianoRollWindow->move(width, 0);
}



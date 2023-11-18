#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _midiInput(MIDIInput::instance())
    , _channelsWindow(nullptr)
    , _playlistWindow(nullptr)
{
    _midiInput->init();

    ui->setupUi(this);

    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionRender->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    ui->actionExit->setShortcuts(QKeySequence::Quit);

    QAction* menuEditFirstAction = ui->menuEdit->actions()[0];

    QAction* undoAction = _app->undoStack().createUndoAction(this, "&Undo");
    undoAction->setShortcuts(QKeySequence::Undo);
    ui->menuEdit->insertAction(menuEditFirstAction, undoAction);

    QAction* redoAction = _app->undoStack().createRedoAction(this, "&Redo");
    redoAction->setShortcuts(QKeySequence::Redo);
    ui->menuEdit->insertAction(menuEditFirstAction, redoAction);

    connect(&_midiPoller, &MIDIPoller::receivedMessage, this, &MainWindow::handleMIDIMessage);
    _midiPoller.start();

    ui->topWidget->setApplication(app);

   _timer.setInterval(1000 / 30);
   connect(&_timer, &QTimer::timeout, this, &MainWindow::frame);

   connect(ui->topWidget, &TopWidget::play, this, &MainWindow::play);
   connect(ui->topWidget, &TopWidget::pause, this, &MainWindow::pause);
   connect(ui->topWidget, &TopWidget::stop, this, &MainWindow::stop);
   connect(ui->topWidget, &TopWidget::patternChanged, this, &MainWindow::patternChanged);
   connect(ui->topWidget, &TopWidget::beatsPerBarChanged, this, &MainWindow::beatsPerBarChanged);
   connect(ui->topWidget, &TopWidget::midiDeviceSet, this, &MainWindow::setMIDIDevice);

   connect(ui->mdiArea, &MdiArea::viewModeChanged, this, &MainWindow::mdiViewModeChanged);

   connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newTriggered);
   connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openTriggered);
   connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveTriggered);
   connect(ui->actionRender, &QAction::triggered, this, &MainWindow::renderTriggered);
   connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
   connect(ui->actionStyles, &QAction::triggered, this, &MainWindow::stylesTriggered);
   connect(ui->actionChannels, &QAction::triggered, this, &MainWindow::showChannelsWindow);
   connect(ui->actionPlaylist, &QAction::triggered, this, &MainWindow::showPlaylistWindow);
   connect(ui->actionImportFMPatches, &QAction::triggered, this, &MainWindow::fmImportTriggered);

   _styleDialog.setApplication(_app);
   _fmImportDialog.setApplication(_app);

   showChannelsWindow();
   showPlaylistWindow();
}

MainWindow::~MainWindow()
{
    _midiPoller.stop();
    _midiPoller.quit();
    _midiPoller.wait();

    _midiInput->destroy();

    delete ui;
}

void MainWindow::moveChannelUp(const int index)
{
    if (index != 0) {
        bool isSelected = _channelsWidget->selected() == index;
        bool isOtherSelected = _channelsWidget->selected() == index - 1;

        _app->project().moveChannelUp(index);

        QList<MdiSubWindow*> channelWindows = _channelWindows[index];
        QList<MdiSubWindow*> otherChannelWindows = _channelWindows[index - 1];

        _channelWindows.remove(index);
        _channelWindows.remove(index - 1);

        _channelWindows[index - 1] = channelWindows;
        _channelWindows[index] = otherChannelWindows;

        _channelsWidget->update(index);
        _channelsWidget->update(index-1);

        if (isSelected)
            _channelsWidget->select(index - 1);
        if (isOtherSelected)
            _channelsWidget->select(index);
    }
}

void MainWindow::moveChannelDown(const int index)
{
    if (index != _app->project().channels()-1) {
        bool isSelected = _channelsWidget->selected() == index;
        bool isOtherSelected = _channelsWidget->selected() == index + 1;

        _app->project().moveChannelDown(index);

        QList<MdiSubWindow*> channelWindows = _channelWindows[index];
        QList<MdiSubWindow*> otherChannelWindows = _channelWindows[index + 1];

        _channelWindows.remove(index);
        _channelWindows.remove(index + 1);

        _channelWindows[index + 1] = channelWindows;
        _channelWindows[index] = otherChannelWindows;

        _channelsWidget->update(index);
        _channelsWidget->update(index+1);

        if (isSelected)
            _channelsWidget->select(index + 1);
        if (isOtherSelected)
            _channelsWidget->select(index);
    }
}

Channel MainWindow::deleteChannel(const int index)
{
    Channel chan = _app->project().getChannel(index);

    _app->project().removeChannel(index);
    _channelsWidget->remove(index);

    if (_channelsWidget->activeChannel() == index) {
        if (_app->project().channels() > index) {
            _channelsWidget->select(index);
        }
    }

    for (MdiSubWindow* window : _channelWindows[index]) {
        window->close();
    }

    _channelWindows.remove(index);

    QList<int> toDecrement;

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        if (it.key() > index) {
            toDecrement.append(it.key());
        }
    }

    std::sort(toDecrement.begin(), toDecrement.end());

    for (const int i : toDecrement) {
        _channelWindows[i-1] = _channelWindows[i];
        _channelWindows.remove(i);
    }

    return chan;
}

void MainWindow::addChannel(const int index, const Channel& channel)
{
    _app->project().addChannel(index, channel);
    _channelsWidget->add(index);
}

void MainWindow::addChannel()
{
    _app->project().addChannel();
    _channelsWidget->add(_app->project().channels() - 1);
}

int MainWindow::getChannelVolume(const int index)
{
    return _app->project().getChannel(index).settings().volume();
}

void MainWindow::setChannelVolume(const int index, const int volume)
{
    _channelsWidget->setVolume(index, volume);
}

int MainWindow::channels() const
{
    return _app->project().channels();
}

void MainWindow::play()
{
    _timer.start();

    if (!_app->paused()) {
        PianoRollWidget* prw;
        PlaylistWidget* pw;
        if ((prw = dynamic_cast<PianoRollWidget*>(ui->mdiArea->activeSubWindow()->widget()))) {
            if (prw->hasLoop()) {
                _app->play(prw->pattern(), prw->loopStart(), prw->loopEnd());
                return;
            }
        } else if ((pw = dynamic_cast<PlaylistWidget*>(ui->mdiArea->activeSubWindow()->widget()))) {
            if (_app->project().playMode() == Project::PlayMode::SONG) {
                if (pw->hasLoop()) {
                    _app->play(pw->loopStart(), pw->loopEnd());
                    return;
                }
            }
        }
    }

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

void MainWindow::pianoRollTriggered(const int index, const bool on)
{
    if (on) {
        _app->project().getFrontPattern().getTrack(index).usePianoRoll();
    } else {
        _app->project().getFrontPattern().getTrack(index).useStepSequencer();
    }

    _channelsWidget->update();

    auto it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [&](MdiSubWindow* window){
        PianoRollWidget* prw;
        if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
            if (&prw->pattern() == &_app->project().getFrontPattern()) {
                return true;
            }
        }
        return false;
    });
    if (it != _channelWindows[index].end()) {
        if (on) {
            ui->mdiArea->setActiveSubWindow(*it);
        } else {
            (*it)->close();
            _channelWindows[index].removeAll(*it);
        }
    } else if (on) {
        PianoRollWidget* pianoRollWidget = new PianoRollWidget(this, _app);
        pianoRollWidget->setWindowTitle(QString("%1: Piano Roll").arg(_app->project().getChannel(index).name()));
        connect(pianoRollWidget, &PianoRollWidget::keyOn, this, &MainWindow::keyOn);
        connect(pianoRollWidget, &PianoRollWidget::keyOff, this, &MainWindow::keyOff);

        pianoRollWidget->setTrack(_app->project().frontPattern(), index);

        MdiSubWindow* pianoRollWindow = new MdiSubWindow(ui->mdiArea);
        pianoRollWindow->resize(pianoRollWidget->size());

        pianoRollWindow->setAttribute(Qt::WA_DeleteOnClose);
        ui->mdiArea->addSubWindow(pianoRollWindow);

        pianoRollWindow->setWidget(pianoRollWidget);
        connect(pianoRollWindow, &MdiSubWindow::closed, this, [=](){ windowClosed(pianoRollWindow); });

        if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
            pianoRollWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        }

        pianoRollWindow->show();
        ui->mdiArea->setActiveSubWindow(pianoRollWindow);

        _channelWindows[index].append(pianoRollWindow);
    }
}

void MainWindow::channelSelected(const int index)
{
    _selectedChannel = index;

    MdiSubWindow* channelWindow;

    if (_app->project().getChannel(index).type() != Channel::Type::TONE) {
        channelWindow = new MdiSubWindow(ui->mdiArea);
        channelWindow->setAttribute(Qt::WA_DeleteOnClose);
        ui->mdiArea->addSubWindow(channelWindow);
    }

    NoiseWidget* noiseWidget;
    FMWidgetWindow* fmWidget;
    PCMWidget* pcmWidget;
    QList<MdiSubWindow*>::Iterator it;

    for (MdiSubWindow* window : _channelWindows[index]) {
        window->close();
    }

    switch (_app->project().getChannel(index).type()) {
        case Channel::Type::NOISE:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window){ return dynamic_cast<NoiseWidget*>(window->widget()); });
            if (it != _channelWindows[index].end()) {
                ui->mdiArea->setActiveSubWindow(*it);
            } else {
                noiseWidget = new NoiseWidget(this, _app);
                noiseWidget->setSettings(dynamic_cast<NoiseChannelSettings*>(&_app->project().getChannel(index).settings()));
                noiseWidget->setWindowTitle(QString("%1: Noise").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(noiseWidget);
                if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=](){ windowClosed(channelWindow); });
            }
            break;
        case Channel::Type::FM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window){ return dynamic_cast<FMWidgetWindow*>(window->widget()); });
            if (it != _channelWindows[index].end()) {
                ui->mdiArea->setActiveSubWindow(*it);
            } else {
                fmWidget = new FMWidgetWindow(this, _app);
                connect(fmWidget, &FMWidgetWindow::keyPressed, this, &MainWindow::keyOn);
                connect(fmWidget, &FMWidgetWindow::keyReleased, this, &MainWindow::keyOff);
                fmWidget->setSettings(dynamic_cast<FMChannelSettings*>(&_app->project().getChannel(index).settings()));
                fmWidget->setWindowTitle(QString("%1: FM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(fmWidget);
                if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=](){ windowClosed(channelWindow); });
            }
            break;
        case Channel::TONE:
            break;
        case Channel::PCM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window){ return dynamic_cast<PCMWidget*>(window->widget()); });
            if (it != _channelWindows[index].end()) {
                ui->mdiArea->setActiveSubWindow(*it);
            } else {
                pcmWidget = new PCMWidget(this, _app);
                pcmWidget->setSettings(dynamic_cast<PCMChannelSettings*>(&_app->project().getChannel(index).settings()));
                pcmWidget->setWindowTitle(QString("%1: PCM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(pcmWidget);
                if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=](){ windowClosed(channelWindow); });
            }
            break;
    }

    if (_app->project().getChannel(index).type() != Channel::Type::TONE) {
        channelWindow->show();
        ui->mdiArea->setActiveSubWindow(channelWindow);
    }
}

void MainWindow::channelNameChanged(const int index)
{
    QString tail = "Window";
    for (MdiSubWindow* window : _channelWindows[index]) {
        if (dynamic_cast<PianoRollWidget*>(window->widget())) {
            tail = "Piano Roll";
        } else if (dynamic_cast<NoiseWidget*>(window->widget())) {
            tail = "Noise";
        } else if (dynamic_cast<FMWidgetWindow*>(window->widget())) {
            tail = "FM";
        }

        window->setWindowTitle(QString("%1: %2").arg(_app->project().getChannel(index).name()).arg(tail));
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

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);

        VGMStream vgmStream;
        QByteArray data = vgmStream.compile(_app->project(), true);
        file.write(data);
        file.close();
    }
}

void MainWindow::keyOn(const int key, const int velocity)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(ui->mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = _channelsWidget->activeChannel();
    }

    if (activeChannel >= 0) {
        Channel& channel = _app->project().getChannel(activeChannel);
        _app->keyOn(channel.type(), channel.settings(), key, velocity);

        for (MdiSubWindow* window : _channelWindows[activeChannel]) {
            PianoRollWidget* prw;
            FMWidgetWindow* fmw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->pressKey(key);
            } else if ((fmw = dynamic_cast<FMWidgetWindow*>(window->widget()))) {
                fmw->pressKey(key);
            }
        }
    }
}

void MainWindow::keyOff(const int key)
{
    int activeChannel = _channelsWidget->activeChannel();
    _app->keyOff(key);
    for (MdiSubWindow* window : _channelWindows[activeChannel]) {
        PianoRollWidget* prw;
        FMWidgetWindow* fmw;
        if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
            prw->releaseKey(key);
        } else if ((fmw = dynamic_cast<FMWidgetWindow*>(window->widget()))) {
            fmw->releaseKey(key);
        }
    }
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

void MainWindow::stylesTriggered()
{
    _styleDialog.show();
}

void MainWindow::fmImportTriggered()
{
    _fmImportDialog.show();
}

void MainWindow::showChannelsWindow()
{
    if (_channelsWindow == nullptr) {
        _channelsWidget = new ChannelsWidget(this, _app);

        connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
        connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);
        connect(_channelsWidget, &ChannelsWidget::nameChanged, this, &MainWindow::channelNameChanged);

        MdiSubWindow* channelsWindow = new MdiSubWindow(ui->mdiArea);
        connect(channelsWindow, &MdiSubWindow::closed, this, [&](){ _channelsWindow = nullptr; });
        channelsWindow->setAttribute(Qt::WA_DeleteOnClose);
        channelsWindow->setWidget(_channelsWidget);
        if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
            channelsWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        }
        ui->mdiArea->addSubWindow(channelsWindow);
        channelsWindow->show();
        _channelsWindow = channelsWindow;
    } else {
        ui->mdiArea->setActiveSubWindow(_channelsWindow);
    }
}

void MainWindow::showPlaylistWindow()
{
    if (_playlistWindow == nullptr) {
        _playlistWidget = new PlaylistWidget(this, _app);

        MdiSubWindow* playlistWindow = new MdiSubWindow(ui->mdiArea);
        connect(playlistWindow, &MdiSubWindow::closed, this, [&](){ _playlistWindow = nullptr; });
        playlistWindow->setAttribute(Qt::WA_DeleteOnClose);
        playlistWindow->setWidget(_playlistWidget);

        if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
            int width = ui->mdiArea->frameGeometry().width()/2;
            int height = ui->mdiArea->frameGeometry().height();

            playlistWindow->resize(width, height);
            playlistWindow->move(width, 0);
        }

        ui->mdiArea->addSubWindow(playlistWindow);
        playlistWindow->show();
        _playlistWindow = playlistWindow;
    } else {
        ui->mdiArea->setActiveSubWindow(_playlistWindow);
    }
}

void MainWindow::mdiViewModeChanged(const QString& viewMode)
{
    ui->mdiArea->closeAllSubWindows();

    if (viewMode == "windows") {
        MdiArea* mdiArea = new MdiArea(this);
        ui->centralwidget->layout()->replaceWidget(ui->mdiArea, mdiArea);
        ui->mdiArea = mdiArea;
    }
}

void MainWindow::windowClosed(MdiSubWindow* window)
{
    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        (*it).removeAll(window);
    }
}

void MainWindow::doUpdate()
{
    update();
    _channelsWidget->update();
    _playlistWidget->update();

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            PianoRollWidget* prw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->update();
            }
        }
    }
}

void MainWindow::channelSettingsUpdated()
{
    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            NoiseWidget* nw;
            FMWidgetWindow* fmw;
            PCMWidget* pw;
            PianoRollWidget* prw;

            if ((nw = dynamic_cast<NoiseWidget*>(window->widget()))) {
                nw->doUpdate();
            } else if ((fmw = dynamic_cast<FMWidgetWindow*>(window->widget()))) {
                fmw->doUpdate();
            } else if ((pw = dynamic_cast<PCMWidget*>(window->widget()))) {
                pw->doUpdate();
            } else if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate();
            }
        }
    }
}

void MainWindow::setChannelType(Channel& channel, const Channel::Type type)
{
    int index = _app->project().indexOfChannel(channel);

    for (MdiSubWindow* window : _channelWindows[index]) {
        window->close();
    }

    channel.setType(type);

    doUpdate();
}

Application* MainWindow::app()
{
    return _app;
}

void MainWindow::showEvent(QShowEvent*)
{
    if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
        int width = ui->mdiArea->frameGeometry().width()/2;
        int height = ui->mdiArea->frameGeometry().height();

        if (_channelsWindow) {
            _channelsWindow->move(0, 0);
        }

        if (_playlistWindow) {
            _playlistWindow->resize(width, height);
            _playlistWindow->move(width, 0);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (ui->mdiArea->viewMode() == QMdiArea::SubWindowView) {
        int width = ui->mdiArea->frameGeometry().width()/2;
        int height = ui->mdiArea->frameGeometry().height();

        if (_channelsWindow) {
            _channelsWindow->move(0, 0);
        }

        if (_playlistWindow) {
            _playlistWindow->resize(width, height);
            _playlistWindow->move(width, 0);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    _styleDialog.hide();
    _fmImportDialog.hide();
}



#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _midiInput(MIDIInput::instance())
    , _channelsWindow(nullptr)
    , _playlistWindow(nullptr)
    , _styleDialogWindow(nullptr)
    , _fmImportDialogWindow(nullptr)
    , _pcmUsageDialogWindow(nullptr)
    , _infoDialogWindow(nullptr)
{
    _midiInput->init();

    ui->setupUi(this);

    setAcceptDrops(true);

    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
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

    QStringList filters;
    filters << "*.fsp";
    filters << "*.fm";
    filters << "*.mid";
    filters << "*.pcm";
    filters << "*.vgm";
    _filesystemModel.setNameFilters(filters);
    _filesystemModel.setNameFilterDisables(false);
    _filesystemModel.setRootPath(QDir::currentPath());

    _proxyModel.setSourceModel(&_filesystemModel);

    ui->treeView->setModel(&_proxyModel);
    ui->treeView->setRootIndex(_proxyModel.mapFromSource(_filesystemModel.index(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())));
    ui->treeView->setColumnHidden(2, true);
    ui->treeView->setColumnHidden(3, true);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->treeView->setDragEnabled(true);
    ui->treeView->setColumnWidth(0, 192);

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
    connect(ui->actionForFMPSG, &QAction::triggered, this, &MainWindow::renderForFMPSGTriggered);
    connect(ui->actionFor3rdPartyPlayers, &QAction::triggered, this, &MainWindow::renderFor3rdPartyTriggered);
    connect(ui->actionInfo, &QAction::triggered, this, &MainWindow::projectInfoTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionStyles, &QAction::triggered, this, &MainWindow::stylesTriggered);
    connect(ui->actionChannels, &QAction::triggered, this, &MainWindow::showChannelsWindow);
    connect(ui->actionPlaylist, &QAction::triggered, this, &MainWindow::showPlaylistWindow);
    connect(ui->actionImportFMPatches, &QAction::triggered, this, &MainWindow::fmImportTriggered);
    connect(ui->actionPCMUsage, &QAction::triggered, this, &MainWindow::pcmUsageTriggered);

    connect(_app, &Application::pcmUploadStarted, this, &MainWindow::pcmUploadStarted);
    connect(_app, &Application::pcmUploadFinished, this, &MainWindow::pcmUploadFinished);

    showChannelsWindow();
    showPlaylistWindow();
}

MainWindow::~MainWindow()
{
    _midiPoller.stop();
    _midiPoller.quit();
    _midiPoller.wait();

    _midiInput->destroy();

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : *it) {
            window->close();
            delete window;
        }
    }

    if (_channelsWindow) {
        _channelsWindow->close();
        delete _channelsWindow;
    }

    if (_playlistWindow) {
        _playlistWindow->close();
        delete _playlistWindow;
    }

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
    if (index != _app->project().channelCount()-1) {
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
        if (_app->project().channelCount() > index) {
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
    _channelsWidget->add(_app->project().channelCount() - 1);
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
    return _app->project().channelCount();
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
    ui->topWidget->setStatusMessage("Ready.");
    doUpdate();
}

void MainWindow::patternChanged(int num)
{
    _app->project().setFrontPattern(num - 1);
    if (_channelsWindow) _channelsWidget->update();
    if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();
    ui->topWidget->updateFromProject(_app->project());
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

    auto it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [&](MdiSubWindow* window) {
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
        connect(pianoRollWindow, &MdiSubWindow::closed, this, [=]() {
            windowClosed(pianoRollWindow);
        });

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
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<NoiseWidget*>(window->widget());
            });
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
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::Type::FM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<FMWidgetWindow*>(window->widget());
            });
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
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::TONE:
            break;
        case Channel::PCM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<PCMWidget*>(window->widget());
            });
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
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
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
    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : *it) {
            window->close();
            delete window;
        }
    }

    _app->project() = Project();

    ui->topWidget->updateFromProject(_app->project());

    if (_channelsWindow) _channelsWidget->rebuild();
    if (_playlistWindow) _playlistWidget->update();
    if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();
}

void MainWindow::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(nullptr, tr("Open file"), "", "FM-PSG Studio Projects (*.fsp)");

    if (!path.isNull()) {
        _app->project() = BSON::decode(path);

        ui->topWidget->updateFromProject(_app->project());

        _channelsWidget->rebuild();

        _playlistWidget->update();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(nullptr, tr("Save file"), "", "FM-PSG Studio Projects (*.fsp)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encode(_app->project()));
        file.close();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::renderForFMPSGTriggered()
{
    const QString path = QFileDialog::getSaveFileName(nullptr, tr("Save file"), "", "VGM files (*.vgm)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);

        VGMStream vgmStream;
        QByteArray data = _app->project().playMode() == Project::PlayMode::PATTERN
                          ? vgmStream.compile(_app->project(), _app->project().getFrontPattern(), true, true)
                          : vgmStream.compile(_app->project(), true, true);
        file.write(data);
        file.close();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::renderFor3rdPartyTriggered()
{
    const QString path = QFileDialog::getSaveFileName(nullptr, tr("Save file"), "", "VGM files (*.vgm)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);

        VGMStream vgmStream(VGMStream::Format::STANDARD);
        QByteArray data = _app->project().playMode() == Project::PlayMode::PATTERN
                          ? vgmStream.compile(_app->project(), _app->project().getFrontPattern(), true, true)
                          : vgmStream.compile(_app->project(), true, true);
        file.write(data);
        file.close();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::keyOn(const int key, const int velocity)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(ui->mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    FMWidgetWindow* fmw;
    if (_channelWindows[activeChannel].contains(ui->mdiArea->activeSubWindow())) {
        if (prw) {
            prw->pressKey(key);
            _app->keyOn(channel.type(), prw->currentSettings(), key, velocity);
            return;
        } else if ((fmw = dynamic_cast<FMWidgetWindow*>(ui->mdiArea->activeSubWindow()->widget()))) {
            fmw->pressKey(key);
        }
    }
    _app->keyOn(channel.type(), channel.settings(), key, velocity);
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

void MainWindow::projectInfoTriggered()
{
    if (_infoDialogWindow == nullptr) {
        _infoDialog = new ProjectInfoDialog(this, _app);

        MdiSubWindow* window = new MdiSubWindow(ui->mdiArea);
        connect(_infoDialog, &QDialog::finished, window, &MdiSubWindow::close);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _infoDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_infoDialog);
        _infoDialogWindow = window;
        ui->mdiArea->addSubWindow(window);
        window->show();
    } else {
        ui->mdiArea->setActiveSubWindow(_infoDialogWindow);
    }
}

void MainWindow::stylesTriggered()
{
    if (_styleDialogWindow == nullptr) {
        _styleDialog = new StyleDialog(this);
        _styleDialog->setApplication(_app);

        MdiSubWindow* window = new MdiSubWindow(ui->mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _styleDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_styleDialog);
        _styleDialogWindow = window;
        ui->mdiArea->addSubWindow(window);
        window->show();
    } else {
        ui->mdiArea->setActiveSubWindow(_styleDialogWindow);
    }
}

void MainWindow::pcmUsageTriggered()
{
    if (_pcmUsageDialogWindow == nullptr) {
        _pcmUsageDialog = new PCMUsageDialog(this, _app);

        MdiSubWindow* window = new MdiSubWindow(ui->mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _fmImportDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_pcmUsageDialog);
        _pcmUsageDialogWindow = window;
        ui->mdiArea->addSubWindow(window);
        window->show();
    } else {
        ui->mdiArea->setActiveSubWindow(_pcmUsageDialogWindow);
    }
}

void MainWindow::fmImportTriggered()
{
    if (_fmImportDialogWindow == nullptr) {
        _fmImportDialog = new FMImportDialog(this);
        _fmImportDialog->setApplication(_app);

        MdiSubWindow* window = new MdiSubWindow(ui->mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _fmImportDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_fmImportDialog);
        _fmImportDialogWindow = window;
        ui->mdiArea->addSubWindow(window);
        window->show();
    } else {
        ui->mdiArea->setActiveSubWindow(_fmImportDialogWindow);
    }
}

void MainWindow::showChannelsWindow()
{
    if (_channelsWindow == nullptr) {
        _channelsWidget = new ChannelsWidget(this, _app);

        connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
        connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);
        connect(_channelsWidget, &ChannelsWidget::nameChanged, this, &MainWindow::channelNameChanged);

        MdiSubWindow* channelsWindow = new MdiSubWindow(ui->mdiArea);
        connect(channelsWindow, &MdiSubWindow::closed, this, [&]() {
            _channelsWindow = nullptr;
        });
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
        connect(_playlistWidget, &PlaylistWidget::patternClicked, this, &MainWindow::patternChanged);

        MdiSubWindow* playlistWindow = new MdiSubWindow(ui->mdiArea);
        connect(playlistWindow, &MdiSubWindow::closed, this, [&]() {
            _playlistWindow = nullptr;
        });
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
        delete ui->centralwidget->layout()->replaceWidget(ui->mdiArea, mdiArea);
        ui->mdiArea = mdiArea;
    }

    showChannelsWindow();
    showPlaylistWindow();
}

void MainWindow::pcmUploadStarted()
{
    ui->topWidget->setStatusMessage("Uploading PCM...");
}

void MainWindow::pcmUploadFinished()
{
    ui->topWidget->setStatusMessage("Done.");
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
    if (_channelsWindow) _channelsWidget->update();
    if (_playlistWindow) _playlistWidget->update();
    if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();


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

    for (Pattern* pattern : _app->project().patterns()) {
        for (Track* track : pattern->tracks()) {
            for (Track::SettingsChange* sc : track->settingsChanges()) {
                track->removeSettingsChange(sc);
            }
        }
    }

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

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "fsp") {
        _app->project() = BSON::decode(path);

        ui->topWidget->updateFromProject(_app->project());

        if (_channelsWindow) _channelsWidget->rebuild();
        if (_playlistWindow) _playlistWidget->update();
        if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();

        _playlistWidget->update();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(fileInfo.fileName()));
    }
}



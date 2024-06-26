#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _app(app)
    , _midiInput(MIDIInput::instance())
    , _channelsWindow(nullptr)
    , _playlistWindow(nullptr)
    , _settingsDialogWindow(nullptr)
    , _styleDialogWindow(nullptr)
    , _opnImportDialogWindow(nullptr)
    , _oplImportDialogWindow(nullptr)
    , _pcmUsageDialogWindow(nullptr)
    , _infoDialogWindow(nullptr)
    , _playerDialogWindow(nullptr)
    , _romBuilderDialogWindow(nullptr)
    , _fmGlobalsWindow(nullptr)
    , _ssgGlobalsWindow(nullptr)
    , _melodyGlobalsWindow(nullptr)
    , _romGlobalsWindow(nullptr)
    , _treeView(new FilesystemTreeView(this))
    , _mdiArea(new MdiArea(this))
    , _splitter(new QSplitter(this))
{
    _midiInput->init();

    ui->setupUi(this);
    ui->centralwidget->layout()->replaceWidget(ui->centerWidget, _splitter);

    _splitter->addWidget(_treeView);
    _splitter->addWidget(_mdiArea);
    _splitter->setHandleWidth(8);

    _treeView->setMinimumWidth(320);

    setAcceptDrops(true);

    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
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
    filters << "*.csp";
    filters << "*.opn";
    filters << "*.opl";
    filters << "*.mid";
    filters << "*.pcm";
    filters << "*.vgm";
    filters << "*.rom";
    _filesystemModel.setNameFilters(filters);
    _filesystemModel.setNameFilterDisables(false);
    _filesystemModel.setRootPath(QDir::currentPath());

    _proxyModel.setSourceModel(&_filesystemModel);

    _treeView->setModel(&_proxyModel);
    _treeView->setRootIndex(_proxyModel.mapFromSource(_filesystemModel.index(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())));
    _treeView->setColumnHidden(2, true);
    _treeView->setColumnHidden(3, true);
    _treeView->setSortingEnabled(true);
    _treeView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    _treeView->setDragEnabled(true);
    _treeView->setColumnWidth(0, 192);

    connect(ui->topWidget, &TopWidget::play, this, &MainWindow::play);
    connect(ui->topWidget, &TopWidget::pause, this, &MainWindow::pause);
    connect(ui->topWidget, &TopWidget::stop, this, &MainWindow::stop);
    connect(ui->topWidget, &TopWidget::patternChanged, this, &MainWindow::patternChanged);
    connect(ui->topWidget, &TopWidget::beatsPerBarChanged, this, &MainWindow::beatsPerBarChanged);
    connect(ui->topWidget, &TopWidget::midiDeviceSet, this, &MainWindow::setMIDIDevice);

    connect(_mdiArea, &MdiArea::viewModeChanged, this, &MainWindow::mdiViewModeChanged);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveTriggered);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsTriggered);
    connect(ui->actionPublish, &QAction::triggered, this, &MainWindow::publishTriggered);
    connect(ui->actionForChromasound, &QAction::triggered, this, &MainWindow::renderForChromasoundTriggered);
    connect(ui->actionFor3rdPartyPlayers, &QAction::triggered, this, &MainWindow::renderFor3rdPartyTriggered);
    connect(ui->actionInfo, &QAction::triggered, this, &MainWindow::projectInfoTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settingsTriggered);
    connect(ui->actionStyles, &QAction::triggered, this, &MainWindow::stylesTriggered);
    connect(ui->actionChannels, &QAction::triggered, this, &MainWindow::showChannelsWindow);
    connect(ui->actionPlaylist, &QAction::triggered, this, &MainWindow::showPlaylistWindow);
    connect(ui->actionOPN, &QAction::triggered, this, &MainWindow::opnImportTriggered);
    connect(ui->actionOPL, &QAction::triggered, this, &MainWindow::oplImportTriggered);
    connect(ui->actionPCMUsage, &QAction::triggered, this, &MainWindow::pcmUsageTriggered);
    connect(ui->actionMediaPlayer, &QAction::triggered, this, &MainWindow::playerTriggered);
    connect(ui->actionROMBuilder, &QAction::triggered, this, &MainWindow::romBuilderTriggered);

    connect(_app, &Application::pcmUploadStarted, this, &MainWindow::pcmUploadStarted);
    connect(_app, &Application::pcmUploadFinished, this, &MainWindow::pcmUploadFinished);
    connect(_app, &Application::compileStarted, this, &MainWindow::compileStarted);
    connect(_app, &Application::compileFinished, this, &MainWindow::compileFinished);

    connect(&_app->undoStack(), &QUndoStack::cleanChanged, this, &MainWindow::undoStackCleanChanged);

    connect(ui->actionFM, &QAction::triggered, this, &MainWindow::fmGlobalsTriggered);
    connect(ui->actionSSG, &QAction::triggered, this, &MainWindow::ssgGlobalsTriggered);
    connect(ui->actionMelody, &QAction::triggered, this, &MainWindow::melodyGlobalsTriggered);
    connect(ui->actionROM, &QAction::triggered, this, &MainWindow::romGlobalsTriggered);

    connect(_splitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

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

    if (_settingsDialogWindow) {
        _settingsDialogWindow->close();
        delete _settingsDialogWindow;
    }

    if (_infoDialogWindow) {
        _infoDialogWindow->close();
        delete _infoDialogWindow;
    }

    if (_styleDialogWindow) {
        _styleDialogWindow->close();
        delete _styleDialogWindow;
    }

    if (_opnImportDialogWindow) {
        _opnImportDialogWindow->close();
        delete _opnImportDialogWindow;
    }

    if (_oplImportDialogWindow) {
        _oplImportDialogWindow->close();
        delete _oplImportDialogWindow;
    }

    if (_pcmUsageDialogWindow) {
        _pcmUsageDialogWindow->close();
        delete _pcmUsageDialogWindow;
    }

    if (_playerDialogWindow) {
        _playerDialogWindow->close();
        delete _playerDialogWindow;
    }

    if (_fmGlobalsWindow) {
        _fmGlobalsWindow->close();
        delete _fmGlobalsWindow;
    }

    if (_ssgGlobalsWindow) {
        _ssgGlobalsWindow->close();
        delete _ssgGlobalsWindow;
    }

    if (_melodyGlobalsWindow) {
        _melodyGlobalsWindow->close();
        delete _melodyGlobalsWindow;
    }

    delete _treeView;
    delete _mdiArea;
    delete _splitter;

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

void MainWindow::movePatternUp(const int index)
{
    _app->project().movePatternUp(index);
    doUpdate();
}

void MainWindow::movePatternDown(const int index)
{
    _app->project().movePatternDown(index);
    doUpdate();
}

QList<Playlist::Item*> MainWindow::deletePattern(const int index)
{
    QList<Playlist::Item*> items = _app->project().deletePattern(index);

    doUpdate();

    return items;
}

void MainWindow::insertPattern(const int index, Pattern* pattern, const QList<Playlist::Item*>& items)
{
    _app->project().insertPattern(index, pattern);
    if (pattern) {
        _app->project().playlist().addItems(items);
    }
    doUpdate();
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
        if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
            if (prw->hasLoop()) {
                _app->play(prw->pattern(), prw->loopStart(), prw->loopEnd());
                return;
            }
        } else if ((pw = dynamic_cast<PlaylistWidget*>(_mdiArea->activeSubWindow()->widget()))) {
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

void MainWindow::beatsPerBarChanged(int beatsPerBar)
{
    if (_channelsWindow) {
        _channelsWidget->update();
    }

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            PianoRollWidget* prw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->setCellMajors({ beatsPerBar });
                prw->update();
            }
        }
    }

    if (_playlistWindow) {
        _playlistWidget->setCellMajors({ beatsPerBar });
        _playlistWidget->update();
    }
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
            _mdiArea->setActiveSubWindow(*it);
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

        MdiSubWindow* pianoRollWindow = new MdiSubWindow(_mdiArea);
        pianoRollWindow->setWidget(pianoRollWidget);

        pianoRollWindow->resize(pianoRollWidget->size());

        pianoRollWindow->setAttribute(Qt::WA_DeleteOnClose);
        _mdiArea->addSubWindow(pianoRollWindow);

        connect(pianoRollWindow, &MdiSubWindow::closed, this, [=]() {
            windowClosed(pianoRollWindow);
        });

        if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
            pianoRollWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        }

        pianoRollWindow->show();
        _mdiArea->setActiveSubWindow(pianoRollWindow);

        _channelWindows[index].append(pianoRollWindow);
    }
}

void MainWindow::channelSelected(const int index)
{
    _selectedChannel = index;

    MdiSubWindow* channelWindow;

    if (_app->project().getChannel(index).type() == Channel::Type::TONE) {
        return;
    }

    channelWindow = new MdiSubWindow(_mdiArea);
    channelWindow->setAttribute(Qt::WA_DeleteOnClose);

    NoiseWidget* noiseWidget;
    FMWidgetWindow* fmWidget;
    PCMWidget* pcmWidget;
    SSGWidget* ssgWidget;
    MelodyWidget* melodyWidget;
    RhythmWidget* rhythmWidget;
    ROMWidget* romWidget;
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
                _mdiArea->setActiveSubWindow(*it);
            } else {
                noiseWidget = new NoiseWidget(this, _app);
                noiseWidget->setSettings(dynamic_cast<NoiseChannelSettings*>(&_app->project().getChannel(index).settings()));
                noiseWidget->setWindowTitle(QString("%1: Noise").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(noiseWidget);
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
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
                _mdiArea->setActiveSubWindow(*it);
            } else {
                fmWidget = new FMWidgetWindow(this, _app);
                connect(fmWidget, &FMWidgetWindow::keyPressed, this, &MainWindow::keyOn);
                connect(fmWidget, &FMWidgetWindow::keyReleased, this, &MainWindow::keyOff);
                fmWidget->setSettings(dynamic_cast<FMChannelSettings*>(&_app->project().getChannel(index).settings()));
                fmWidget->setWindowTitle(QString("%1: FM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(fmWidget);
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
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
                _mdiArea->setActiveSubWindow(*it);
            } else {
                pcmWidget = new PCMWidget(this, _app);
                pcmWidget->setSettings(dynamic_cast<PCMChannelSettings*>(&_app->project().getChannel(index).settings()));
                pcmWidget->setWindowTitle(QString("%1: PCM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(pcmWidget);
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::ROM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<ROMWidget*>(window->widget());
            });
            if (it != _channelWindows[index].end()) {
                _mdiArea->setActiveSubWindow(*it);
            } else {
                romWidget = new ROMWidget(this, _app);
                romWidget->setSettings(dynamic_cast<ROMChannelSettings*>(&_app->project().getChannel(index).settings()));
                romWidget->setWindowTitle(QString("%1: ROM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(romWidget);
                channelWindow->resize(channelWindow->minimumSizeHint());
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::SSG:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<SSGWidget*>(window->widget());
            });
            if (it != _channelWindows[index].end()) {
                _mdiArea->setActiveSubWindow(*it);
            } else {
                ssgWidget = new SSGWidget(this, _app);
                ssgWidget->setSettings(dynamic_cast<SSGChannelSettings*>(&_app->project().getChannel(index).settings()));
                ssgWidget->setWindowTitle(QString("%1: SSG").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(ssgWidget);
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::MELODY:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<MelodyWidget*>(window->widget());
            });
            if (it != _channelWindows[index].end()) {
                _mdiArea->setActiveSubWindow(*it);
            } else {
                melodyWidget = new MelodyWidget(this, _app);
                melodyWidget->setSettings(dynamic_cast<MelodyChannelSettings*>(&_app->project().getChannel(index).settings()));
                melodyWidget->setWindowTitle(QString("%1: Melody").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(melodyWidget);
                channelWindow->resize(channelWindow->minimumSizeHint());
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
        case Channel::RHYTHM:
            it = std::find_if(_channelWindows[index].begin(), _channelWindows[index].end(), [](MdiSubWindow* window) {
                return dynamic_cast<RhythmWidget*>(window->widget());
            });
            if (it != _channelWindows[index].end()) {
                _mdiArea->setActiveSubWindow(*it);
            } else {
                rhythmWidget = new RhythmWidget(this, _app);
                rhythmWidget->setSettings(dynamic_cast<RhythmChannelSettings*>(&_app->project().getChannel(index).settings()));
                rhythmWidget->setWindowTitle(QString("%1: Rhythm").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(rhythmWidget);
                channelWindow->resize(channelWindow->minimumSizeHint());
                if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
                    channelWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
                }
                _channelWindows[index].append(channelWindow);
                connect(channelWindow, &MdiSubWindow::closed, this, [=]() {
                    windowClosed(channelWindow);
                });
            }
            break;
    }

    _mdiArea->addSubWindow(channelWindow);
    channelWindow->show();
    _mdiArea->setActiveSubWindow(channelWindow);
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
        } else if (dynamic_cast<SSGWidget*>(window->widget())) {
            tail = "SSG";
        } else if (dynamic_cast<MelodyWidget*>(window->widget())) {
            tail = "Melody";
        } else if (dynamic_cast<RhythmWidget*>(window->widget())) {
            tail = "Rhythm";
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

    _app->undoStack().clear();
    _app->project() = Project();
    _app->setupChromasound();

    ui->topWidget->updateFromProject(_app->project());

    if (_channelsWindow) _channelsWidget->rebuild();
    if (_playlistWindow) _playlistWidget->update();
    if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();
}

void MainWindow::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound Studio Projects (*.csp)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
            for (MdiSubWindow* window : *it) {
                window->close();
                delete window;
            }
        }

        _app->undoStack().clear();
        _app->project() = BSON::decode(path);
        _app->setupChromasound();

        ui->topWidget->updateFromProject(_app->project());

        _channelsWidget->rebuild();

        _playlistWidget->update();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::saveTriggered()
{
    if (_app->project().path().isEmpty()) {
        saveAsTriggered();
    } else {
        QFile file(_app->project().path());
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encode(_app->project()));
        file.close();

        _app->undoStack().setClean();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(_app->project().path()).fileName()));
    }
}

void MainWindow::saveAsTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Chromasound Studio Projects (*.csp)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encode(_app->project()));
        file.close();

        _app->undoStack().setClean();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
    }
}

void MainWindow::publishTriggered()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Publish project"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QDir dir(path);

        for (int i = 0; i < _app->project().channelCount(); i++) {
            Channel& chan = _app->project().getChannel(i);
            if (chan.type() == Channel::Type::PCM) {
                PCMChannelSettings& pcmSettings = dynamic_cast<PCMChannelSettings&>(chan.settings());

                if (pcmSettings.path().isEmpty()) {
                    continue;
                }

                dir.mkdir("PCM");
                QDir pcmDir(dir.absoluteFilePath("PCM"));

                QString newPath = pcmDir.absoluteFilePath(QFileInfo(pcmSettings.path()).fileName());
                QFile::copy(_app->project().resolve(pcmSettings.path()), newPath);

                pcmSettings.setPath(dir.relativeFilePath(newPath));
            }
        }

        if (!_app->project().romFile().isEmpty()) {
            QString newPath = dir.absoluteFilePath(QFileInfo(_app->project().romFile()).fileName());
            QFile::copy(_app->project().resolve(_app->project().romFile()), newPath);

            _app->project().setROMFile(dir.relativeFilePath(newPath));
        }

        QString projectFileName("project.csp");
        if (!_app->project().path().isEmpty()) {
            projectFileName = QFileInfo(_app->project().path()).fileName();
        }

        QFile file(dir.absoluteFilePath(projectFileName));
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encode(_app->project()));
        file.close();

        ui->topWidget->setStatusMessage(QString("Published %1.").arg(projectFileName));
    }
}

void MainWindow::renderForChromasoundTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "VGM files (*.vgm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QThread* thread = QThread::create([&](const QString path) {
            QFile file(path);
            file.open(QIODevice::WriteOnly);

            VGMStream vgmStream;
            compileStarted();
            QByteArray data = _app->project().playMode() == Project::PlayMode::PATTERN
                              ? vgmStream.compile(_app->project(), _app->project().getFrontPattern(), true)
                              : vgmStream.compile(_app->project(), true);
            file.write(data);
            file.close();

            ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
        }, path);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}

void MainWindow::renderFor3rdPartyTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "VGM files (*.vgm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QThread* thread = QThread::create([&](const QString path) {
            QFile file(path);
            file.open(QIODevice::WriteOnly);

            VGMStream vgmStream(VGMStream::Format::STANDARD);
            compileStarted();
            QByteArray data = _app->project().playMode() == Project::PlayMode::PATTERN
                              ? vgmStream.compile(_app->project(), _app->project().getFrontPattern(), true)
                              : vgmStream.compile(_app->project(), true);
            file.write(data);
            file.close();

            ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
        }, path);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}

void MainWindow::keyOn(const int key, const int velocity)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    FMWidgetWindow* fmw;
    if (_channelWindows[activeChannel].contains(_mdiArea->activeSubWindow())) {
        if (prw) {
            prw->pressKey(key);
            _app->keyOn(channel.type(), prw->currentSettings(), key, velocity);
            return;
        } else if ((fmw = dynamic_cast<FMWidgetWindow*>(_mdiArea->activeSubWindow()->widget()))) {
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

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(_infoDialog, &QDialog::finished, window, &MdiSubWindow::close);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _infoDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_infoDialog);
        _infoDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_infoDialogWindow);
    }
}

void MainWindow::settingsTriggered()
{
    if (_settingsDialogWindow == nullptr) {
        _settingsDialog = new SettingsDialog(this);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(_settingsDialog, &QDialog::finished, window, &MdiSubWindow::close);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _settingsDialogWindow = nullptr;
        });
        connect(_settingsDialog, &SettingsDialog::done, this, [&]() {
            _app->setupChromasound();
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_settingsDialog);
        _settingsDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_settingsDialogWindow);
    }
}

void MainWindow::stylesTriggered()
{
    if (_styleDialogWindow == nullptr) {
        _styleDialog = new StyleDialog(this);
        _styleDialog->setApplication(_app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _styleDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_styleDialog);
        _styleDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_styleDialogWindow);
    }
}

void MainWindow::pcmUsageTriggered()
{
    if (_pcmUsageDialogWindow == nullptr) {
        _pcmUsageDialog = new PCMUsageDialog(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _pcmUsageDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_pcmUsageDialog);
        _pcmUsageDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_pcmUsageDialogWindow);
    }
}

void MainWindow::opnImportTriggered()
{
    if (_opnImportDialogWindow == nullptr) {
        _opnImportDialog = new OPNImportDialog(this);
        _opnImportDialog->setApplication(_app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _opnImportDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_opnImportDialog);
        _opnImportDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_opnImportDialogWindow);
    }
}

void MainWindow::oplImportTriggered()
{
    if (_oplImportDialogWindow == nullptr) {
        _oplImportDialog = new OPLImportDialog(this);
        _oplImportDialog->setApplication(_app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _oplImportDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_oplImportDialog);
        _oplImportDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_oplImportDialogWindow);
    }
}

void MainWindow::playerTriggered()
{
    if (_playerDialogWindow == nullptr) {
        _player = new Player(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _playerDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_player);
        _playerDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_playerDialogWindow);
    }
}

void MainWindow::romBuilderTriggered()
{
    if (_romBuilderDialogWindow == nullptr) {
        _romBuilderDialog = new ROMBuilderDialog(this);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&](){
            _romBuilderDialogWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_romBuilderDialog);
        _romBuilderDialogWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_romBuilderDialogWindow);
    }
}

void MainWindow::fmGlobalsTriggered()
{
    if (_fmGlobalsWindow == nullptr) {
        _fmGlobalsWidget = new FMGlobalsWidget(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _fmGlobalsWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_fmGlobalsWidget);
        window->resize(window->minimumSizeHint());
        window->setWindowTitle("FM Globals");
        _fmGlobalsWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_fmGlobalsWindow);
    }
}

void MainWindow::ssgGlobalsTriggered()
{
    if (_ssgGlobalsWindow == nullptr) {
        _ssgGlobalsWidget = new SSGGlobalsWidget(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _ssgGlobalsWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_ssgGlobalsWidget);
        window->resize(window->minimumSizeHint());
        window->setWindowTitle("SSG Globals");
        _ssgGlobalsWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_ssgGlobalsWindow);
    }
}

void MainWindow::melodyGlobalsTriggered()
{
    if (_melodyGlobalsWindow == nullptr) {
        _melodyGlobalsWidget = new MelodyGlobalsWidget(this, _app);
        _melodyGlobalsWidget->setSettings(&_app->project().userTone());

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _melodyGlobalsWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_melodyGlobalsWidget);
        window->resize(window->minimumSizeHint());
        window->setWindowTitle("Melody Globals");
        _melodyGlobalsWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_melodyGlobalsWindow);
    }
}

void MainWindow::romGlobalsTriggered()
{
    if (_romGlobalsWindow == nullptr) {
        _romGlobalsWidget = new ROMGlobalsWidget(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _romGlobalsWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_romGlobalsWidget);
        window->resize(window->minimumSizeHint());
        window->setWindowTitle("Project Sample ROM");
        _romGlobalsWindow = window;
        _mdiArea->addSubWindow(window);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_romGlobalsWindow);
    }
}

void MainWindow::showChannelsWindow()
{
    if (_channelsWindow == nullptr) {
        _channelsWidget = new ChannelsWidget(this, _app);

        connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
        connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);
        connect(_channelsWidget, &ChannelsWidget::nameChanged, this, &MainWindow::channelNameChanged);

        MdiSubWindow* channelsWindow = new MdiSubWindow(_mdiArea);
        connect(channelsWindow, &MdiSubWindow::closed, this, [&]() {
            _channelsWindow = nullptr;
        });
        channelsWindow->setAttribute(Qt::WA_DeleteOnClose);
        channelsWindow->setWidget(_channelsWidget);
        if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
            channelsWindow->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        }
        _mdiArea->addSubWindow(channelsWindow);
        channelsWindow->show();
        _channelsWindow = channelsWindow;
    } else {
        _mdiArea->setActiveSubWindow(_channelsWindow);
    }
}

void MainWindow::showPlaylistWindow()
{
    if (_playlistWindow == nullptr) {
        _playlistWidget = new PlaylistWidget(this, _app);
        connect(_playlistWidget, &PlaylistWidget::patternClicked, this, &MainWindow::patternChanged);

        MdiSubWindow* playlistWindow = new MdiSubWindow(_mdiArea);
        connect(playlistWindow, &MdiSubWindow::closed, this, [&]() {
            _playlistWindow = nullptr;
        });
        playlistWindow->setAttribute(Qt::WA_DeleteOnClose);
        playlistWindow->setWidget(_playlistWidget);

        if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
            int width = _mdiArea->frameGeometry().width()/2;
            int height = _mdiArea->frameGeometry().height();

            playlistWindow->resize(width, height);
            playlistWindow->move(width, 0);
        }

        _mdiArea->addSubWindow(playlistWindow);
        playlistWindow->show();
        _playlistWindow = playlistWindow;
    } else {
        _mdiArea->setActiveSubWindow(_playlistWindow);
    }
}

void MainWindow::mdiViewModeChanged(const QString& viewMode)
{
    _mdiArea->closeAllSubWindows();

    if (viewMode == "windows") {
        MdiArea* mdiArea = new MdiArea(this);
        delete ui->centralwidget->layout()->replaceWidget(_mdiArea, mdiArea);
        _mdiArea = mdiArea;
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
    ui->topWidget->setStatusMessage("Ready.");
}

void MainWindow::compileStarted()
{
    ui->topWidget->setStatusMessage("Compiling song...");
}

void MainWindow::compileFinished()
{
    ui->topWidget->setStatusMessage("Ready.");
}

void MainWindow::undoStackCleanChanged(bool clean)
{
    ui->actionSave->setEnabled(!clean);
}

void MainWindow::splitterMoved(int, int)
{
    if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
        int width = _mdiArea->frameGeometry().width()/2;
        int height = _mdiArea->frameGeometry().height();

        if (_channelsWindow) {
            _channelsWindow->move(0, 0);
        }

        if (_playlistWindow) {
            _playlistWindow->resize(width, height);
            _playlistWindow->move(width, 0);
        }
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
    float position = _app->position();

    ui->topWidget->doUpdate(position);
    if (_channelsWindow) _channelsWidget->doUpdate(position);
    if (_playlistWindow) _playlistWidget->doUpdate(position);
    if (_fmGlobalsWindow) _fmGlobalsWidget->doUpdate();
    if (_ssgGlobalsWindow) _ssgGlobalsWidget->doUpdate();
    if (_melodyGlobalsWindow) _melodyGlobalsWidget->doUpdate();
    if (_romGlobalsWindow) _romGlobalsWidget->doUpdate();

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            PianoRollWidget* prw;
            MelodyWidget* mw;
            ROMWidget* rw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate(position);
            }
            if ((mw = dynamic_cast<MelodyWidget*>(window->widget()))) {
                mw->doUpdate();
            }
            if ((rw = dynamic_cast<ROMWidget*>(window->widget()))) {
                rw->doUpdate();
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
            SSGWidget* sw;
            PianoRollWidget* prw;
            MelodyWidget* mw;
            RhythmWidget* rw;

            if ((nw = dynamic_cast<NoiseWidget*>(window->widget()))) {
                nw->doUpdate();
            } else if ((fmw = dynamic_cast<FMWidgetWindow*>(window->widget()))) {
                fmw->doUpdate();
            } else if ((pw = dynamic_cast<PCMWidget*>(window->widget()))) {
                pw->doUpdate();
            } else if ((sw = dynamic_cast<SSGWidget*>(window->widget()))) {
                sw->doUpdate();
            } else if ((mw = dynamic_cast<MelodyWidget*>(window->widget()))) {
                mw->doUpdate();
            } else if ((rw = dynamic_cast<RhythmWidget*>(window->widget()))) {
                rw->doUpdate();
            } else if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate(0);
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
    if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
        int width = _mdiArea->frameGeometry().width()/2;
        int height = _mdiArea->frameGeometry().height();

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
    if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
        int width = _mdiArea->frameGeometry().width()/2;
        int height = _mdiArea->frameGeometry().height();

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
    QStringList paths = QString(data).split("\r\n");
    QString path = paths.first().mid(QString("file://").length()).replace("%20", " ");

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "csp") {
        for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
            for (MdiSubWindow* window : *it) {
                window->close();
                delete window;
            }
        }

        _app->undoStack().clear();
        _app->project() = BSON::decode(path);
        _app->setupChromasound();

        ui->topWidget->updateFromProject(_app->project());

        if (_channelsWindow) _channelsWidget->rebuild();
        if (_playlistWindow) _playlistWidget->update();
        if (_pcmUsageDialogWindow) _pcmUsageDialog->doUpdate();

        _playlistWidget->update();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(fileInfo.fileName()));
    }
}



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
    , _infoDialog(nullptr)
    , _infoDialogWindow(nullptr)
    , _infoScreenDialogWindow(nullptr)
    , _playerDialogWindow(nullptr)
    , _romBuilderDialogWindow(nullptr)
    , _fmGlobalsWindow(nullptr)
    , _ssgGlobalsWindow(nullptr)
    , _melodyGlobalsWindow(nullptr)
    , _romGlobalsWindow(nullptr)
    , _midiPadsDialog(nullptr)
    , _treeView(new FilesystemTreeView(this))
    , _mdiArea(new MdiArea(this))
    , _splitter(new QSplitter(this))
    , _actionPSG(tr("PSG (Tone + Noise)"))
    , _actionFM4(tr("FM"))
    , _actionFM4PSG(tr("FM + PSG"))
    , _actionSSG(tr("SSG (Tone + Noise)"))
    , _actionFM2(tr("FM"))
    , _actionFM2SSG(tr("FM + SSG"))
    , _actionFM2Rhythm(tr("FM + Rhythm"))
    , _actionFM2RhythmSSG(tr("FM + Rhythm + SSG"))
    , _shownEver(false)
    , _selectedChannel(-1)
{
    _midiInput->init();

    ui->setupUi(this);
    ui->centralwidget->layout()->replaceWidget(ui->centerWidget, _splitter);

    _splitter->addWidget(_treeView);
    _splitter->addWidget(_mdiArea);
    _splitter->setHandleWidth(8);

    _treeView->setMinimumWidth(320);

    setAcceptDrops(true);

    _actionPSG.setShortcuts(QKeySequence::New);
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
    connect(&_midiPoller, &MIDIPoller::sync, this, &MainWindow::handleMIDISync);
    _midiPoller.start();

    ui->topWidget->setApplication(app);

    _timer.setInterval(1000 / 30);
    connect(&_timer, &QTimer::timeout, this, &MainWindow::frame);

    QStringList filters;
    filters << "*.csp";
    filters << "*.opn";
    filters << "*.opl";
    filters << "*.opm";
    filters << "*.mid";
    filters << "*.pcm";
    filters << "*.vgm";
    filters << "*.rom";
    filters << "*.lay";
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

    ui->menuFromTemplate->addSection("OPN (Chromasound Pro, Prodigy)");
    ui->menuFromTemplate->addAction(&_actionPSG);
    ui->menuFromTemplate->addAction(&_actionFM4);
    ui->menuFromTemplate->addAction(&_actionFM4PSG);
    ui->menuFromTemplate->addSection("OPL (Chromasound Nova)");
    ui->menuFromTemplate->addAction(&_actionSSG);
    ui->menuFromTemplate->addAction(&_actionFM2);
    ui->menuFromTemplate->addAction(&_actionFM2SSG);
    ui->menuFromTemplate->addAction(&_actionFM2Rhythm);
    ui->menuFromTemplate->addAction(&_actionFM2RhythmSSG);

    connect(ui->topWidget, &TopWidget::play, this, &MainWindow::play);
    connect(ui->topWidget, &TopWidget::pause, this, &MainWindow::pause);
    connect(ui->topWidget, &TopWidget::stop, this, &MainWindow::stop);
    connect(ui->topWidget, &TopWidget::patternChanged, this, &MainWindow::patternChanged);
    connect(ui->topWidget, &TopWidget::beatsPerBarChanged, this, &MainWindow::beatsPerBarChanged);
    connect(ui->topWidget, &TopWidget::midiDeviceSet, this, &MainWindow::setMIDIDevice);
    connect(ui->topWidget, &TopWidget::seekClicked, this, &MainWindow::seekClicked);
    connect(ui->topWidget, &TopWidget::uploadClicked, this, &MainWindow::uploadPCM);
    connect(ui->topWidget, &TopWidget::padsClicked, this, &MainWindow::showPadsDialog);

    connect(_mdiArea, &MdiArea::viewModeChanged, this, &MainWindow::mdiViewModeChanged);

    connect(ui->actionEmptyProject, &QAction::triggered, this, &MainWindow::loadEmptyTemplate);
    connect(&_actionPSG, &QAction::triggered, this, &MainWindow::loadPSGTemplate);
    connect(&_actionFM4, &QAction::triggered, this, &MainWindow::loadFM4Template);
    connect(&_actionFM4PSG, &QAction::triggered, this, &MainWindow::loadFM4PSGTemplate);
    connect(&_actionSSG, &QAction::triggered, this, &MainWindow::loadSSGTemplate);
    connect(&_actionFM2, &QAction::triggered, this, &MainWindow::loadFM2Template);
    connect(&_actionFM2SSG, &QAction::triggered, this, &MainWindow::loadFM2SSGTemplate);
    connect(&_actionFM2Rhythm, &QAction::triggered, this, &MainWindow::loadFM2RhythmTemplate);
    connect(&_actionFM2RhythmSSG, &QAction::triggered, this, &MainWindow::loadFM2RhythmSSGTemplate);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveTriggered);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsTriggered);
    connect(ui->actionPublish, &QAction::triggered, this, &MainWindow::publishTriggered);
    connect(ui->actionRender, &QAction::triggered, this, &MainWindow::renderTriggered);
    connect(ui->actionInfo, &QAction::triggered, this, &MainWindow::projectInfoTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settingsTriggered);
    connect(ui->actionStyles, &QAction::triggered, this, &MainWindow::stylesTriggered);
    connect(ui->actionChannels, &QAction::triggered, this, &MainWindow::showChannelsWindow);
    connect(ui->actionPlaylist, &QAction::triggered, this, &MainWindow::showPlaylistWindow);
    connect(ui->actionOPN, &QAction::triggered, this, &MainWindow::opnImportTriggered);
    connect(ui->actionOPL, &QAction::triggered, this, &MainWindow::oplImportTriggered);
    connect(ui->actionMediaPlayer, &QAction::triggered, this, &MainWindow::playerTriggered);
    connect(ui->actionROMBuilder, &QAction::triggered, this, &MainWindow::romBuilderTriggered);

    connect(_app, &Application::pcmUploadStarted, this, &MainWindow::pcmUploadStarted);
    connect(_app, &Application::pcmUploadFinished, this, &MainWindow::pcmUploadFinished);
    connect(_app, &Application::compileStarted, this, &MainWindow::compileStarted);
    connect(_app, &Application::compileFinished, this, &MainWindow::compileFinished);

    connect(&_app->undoStack(), &QUndoStack::cleanChanged, this, &MainWindow::cleanChanged);

    connect(ui->actionFMGlobals, &QAction::triggered, this, &MainWindow::fmGlobalsTriggered);
    connect(ui->actionSSGGlobals, &QAction::triggered, this, &MainWindow::ssgGlobalsTriggered);
    connect(ui->actionMelodyGlobals, &QAction::triggered, this, &MainWindow::melodyGlobalsTriggered);
    connect(ui->actionPCMGlobals, &QAction::triggered, this, &MainWindow::romGlobalsTriggered);

    _countoffTimer.setSingleShot(true);
    connect(&_countoffTimer, &QTimer::timeout, this, &MainWindow::countoffTimeout);

    connect(_splitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    ui->menubar->setNativeMenuBar(false);
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

void MainWindow::play(bool record)
{
    _timer.start();

    if (record) {
        _countoffTimer.start(0);
    } else {
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

    if (!_app->recording().empty()) {
        if (_selectedChannel < 0) {
            _app->undoStack().push(new AddTrackCommand(this, _app->recording().items(), _app->recording().pitchChanges(), "Recording"));
            _app->project().getFrontPattern().getTrack(_app->project().channelCount() - 1).usePianoRoll();
        } else {
            _app->undoStack().push(new AddTrackItemsCommand(this, _app->project().getFrontPattern().getTrack(_selectedChannel), 0, _app->recording().items(), _app->recording().pitchChanges(), false));
            _app->project().getFrontPattern().getTrack(_selectedChannel).usePianoRoll();
        }
        _app->clearRecording();
    }

    ui->topWidget->setStatusMessage("Ready.");
    doUpdate();
}

void MainWindow::patternChanged(int num)
{
    _app->project().setFrontPattern(num - 1);
    if (_channelsWindow) _channelsWidget->doUpdate(_app->position(), true);
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
    float position = _app->position();

    ui->topWidget->doUpdate(position);
    if (_channelsWindow) _channelsWidget->doUpdate(position);
    if (_playlistWindow) _playlistWidget->doUpdate(position);

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            PianoRollWidget* prw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate(position);
            }
        }
    }
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
        connect(pianoRollWidget, &PianoRollWidget::keyOn, this, &MainWindow::keyOnWithSync);
        connect(pianoRollWidget, &PianoRollWidget::keyOff, this, &MainWindow::keyOffWithSync);

        pianoRollWidget->setTrack(_app->project().frontPattern(), index);

        MdiSubWindow* pianoRollWindow = new MdiSubWindow(_mdiArea);
        pianoRollWindow->setWidget(pianoRollWidget);

        pianoRollWindow->resize(pianoRollWidget->size());

        pianoRollWindow->setAttribute(Qt::WA_DeleteOnClose);
        _mdiArea->addSubWindow(pianoRollWindow);

        connect(pianoRollWindow, &MdiSubWindow::closed, this, [=]() {
            windowClosed(pianoRollWindow);
        });

        connect(pianoRollWidget, &PianoRollWidget::sizeUpNeeded, this, [=]() {
            QSize size;
            size.setWidth(qMax(pianoRollWindow->size().width(), pianoRollWindow->minimumSizeHint().width()));
            size.setHeight(qMax(pianoRollWindow->size().height(), pianoRollWindow->minimumSizeHint().height()));

            _pianoRollSize = pianoRollWindow->size();

            pianoRollWindow->resize(size);
        });

        connect(pianoRollWidget, &PianoRollWidget::sizeDownNeeded, this, [=]() {
            pianoRollWindow->resize(_pianoRollSize);
        });

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
    SSGWidget* ssgWidget;
    MelodyWidget* melodyWidget;
    RhythmWidget* rhythmWidget;
    PCMWidgetWindow* pcmWidget;
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
                return dynamic_cast<PCMWidgetWindow*>(window->widget());
            });
            if (it != _channelWindows[index].end()) {
                _mdiArea->setActiveSubWindow(*it);
            } else {
                pcmWidget = new PCMWidgetWindow(this, _app);
                pcmWidget->setSettings(dynamic_cast<PCMChannelSettings*>(&_app->project().getChannel(index).settings()));
                pcmWidget->setWindowTitle(QString("%1: PCM").arg(_app->project().getChannel(index).name()));

                channelWindow->setWidget(pcmWidget);
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

        preLoad();
        load(path);
        postLoad();

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

        _app->project().setPath(path);

        _app->undoStack().setClean();

        ui->topWidget->setStatusMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));

        updateWindowTitle();
    }
}

void MainWindow::publishTriggered()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Publish project"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QDir dir(path);

        if (!_app->project().pcmFile().isEmpty()) {
            QString newPath = dir.absoluteFilePath(QFileInfo(_app->project().pcmFile()).fileName());
            QFile::copy(_app->project().pcmFile(), newPath);

            _app->project().setPCMFile(QFileInfo(_app->project().pcmFile()).fileName());
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

void MainWindow::renderTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "VGM files (*.vgm)", nullptr, QFileDialog::DontUseNativeDialog);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
    Chromasound_Studio::Profile profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    if (!path.isNull()) {
        QThread* thread = QThread::create([&](const QString path) {
            QFile file(path);
            file.open(QIODevice::WriteOnly);

            VGMStream vgmStream(profile);
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
            _app->keyOn(channel, prw->currentSettings(), key, velocity);
            return;
        } else if ((fmw = dynamic_cast<FMWidgetWindow*>(_mdiArea->activeSubWindow()->widget()))) {
            fmw->pressKey(key);
        }
    }
    _app->keyOn(channel, key, velocity);
}

void MainWindow::keyOff(const int key)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    _app->keyOff(channel, key);
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

void MainWindow::keyOnWithSync(const int key, const int velocity)
{
    keyOn(key, velocity);
    handleMIDISync();
}

void MainWindow::keyOffWithSync(const int key)
{
    keyOff(key);
    handleMIDISync();
}

void MainWindow::padOn(const int pad, const int velocity)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    if (channel.type() == Channel::Type::PCM) {
        PCMChannelSettings& settings = dynamic_cast<PCMChannelSettings&>(channel.settings());
        if (pad < settings.keySampleMappings().count()) {
            int key = settings.keySampleMappings().keys().at(pad);
            _app->keyOn(channel, key, velocity);
        }
    }
}

void MainWindow::padOff(const int pad)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    if (channel.type() == Channel::Type::PCM) {
        PCMChannelSettings& settings = dynamic_cast<PCMChannelSettings&>(channel.settings());
        if (pad < settings.keySampleMappings().count()) {
            int key = settings.keySampleMappings().keys().at(pad);
            _app->keyOff(channel, key);
        }
    }
}

void MainWindow::pitchChange(const int pitch)
{
    int activeChannel;
    PianoRollWidget* prw;
    if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
        activeChannel = prw->channel();
    } else {
        activeChannel = qMax(0, _channelsWidget->activeChannel());
    }

    Channel& channel = _app->project().getChannel(activeChannel);

    float normalizedPitch = -1.0 + 2.0 * (pitch / (std::pow(2, 14) - 1));

    if (channel.type() != Channel::Type::PCM) {
        _app->pitchChange(activeChannel, normalizedPitch, channel.pitchRange());
    }
}

void MainWindow::handleMIDIMessage(const long message)
{
    const quint8 status = ((message >> 0) & 0xFF);
    const quint8 data1 = ((message >> 8) & 0xFF);
    const quint8 data2 = ((message >> 16) & 0xFF);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    QString setting = settings.value(Chromasound_Studio::MIDIPadsKey, "").toString();
    QStringList pads = setting.split(",");
    QList<int> padsInt;

    int pitchVal;
    int modVal;
    for (QString& pad : pads) {
        padsInt.append(pad.toInt(nullptr, 16));
    }

    switch(status) {
    case 0x90:
        keyOn(data1, qMin((int)data2, 100));
        break;
    case 0x80:
        keyOff(data1);
        break;
    case 0x99:
        if (_midiPadsDialog) {
            _midiPadsDialog->pad(data1);
        } else if (padsInt.contains(data1)){
            padOn(padsInt.indexOf(data1), qMin((int)data2, 100));
        }
        break;
    case 0x89:
        if (!_midiPadsDialog && padsInt.contains(data1)) {
            padOff(padsInt.indexOf(data1));
        }
        break;
    case 0xE0:
        pitchVal = ((int)data2 << 7) | data1;
        pitchChange(pitchVal);
        break;
    case 0xB0:
        modVal = data2;
        break;
    default:
        break;
    }
}

void MainWindow::handleMIDISync()
{
    _app->midiSync();
}

void MainWindow::setMIDIDevice(const int device)
{
    _midiPoller.stop();
    _midiPoller.quit();
    _midiPoller.wait();

    _midiInput->setDevice(device);

    _midiPoller.start();
}

void MainWindow::showPadsDialog()
{
    _midiPadsDialog = new MIDIPadsDialog;

    connect(_midiPadsDialog, &MIDIPadsDialog::accepted, this, [&](){
#ifdef Q_OS_WIN
        QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
        QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
        settings.setValue(Chromasound_Studio::MIDIPadsKey, _midiPadsDialog->setting());
    });

    _midiPadsDialog->exec();

    delete _midiPadsDialog;
    _midiPadsDialog = nullptr;
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

#ifdef Q_OS_WIN
            QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
            QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
            int quantity = settings.value(Chromasound_Studio::NumberOfChromasoundsKey, 1).toInt();
            QString chromasound1 = settings.value(Chromasound_Studio::Chromasound1Key, Chromasound_Studio::Emulator).toString();
            QString chromasound2 = settings.value(Chromasound_Studio::Chromasound2Key, Chromasound_Studio::Emulator).toString();
            QString device = settings.value(Chromasound_Studio::DeviceKey, Chromasound_Studio::ChromasoundPro).toString();

            ui->topWidget->showUploadButton((chromasound1 == Chromasound_Studio::ChromasoundDirect ||
                                             (quantity == 2 && chromasound2 == Chromasound_Studio::ChromasoundDirect)) &&
                                            device == Chromasound_Studio::ChromasoundProDirect);
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_settingsDialog);
        _settingsDialogWindow = window;
        _mdiArea->addSubWindow(window);

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_styleDialogWindow);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
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

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_melodyGlobalsWindow);
    }
}

void MainWindow::romGlobalsTriggered()
{
    if (_romGlobalsWindow == nullptr) {
        _romGlobalsWindow = new PCMGlobalsWindow(this, _app);

        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _romGlobalsWindow = nullptr;
        });
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidget(_romGlobalsWindow);
        window->resize(window->minimumSizeHint());
        window->setWindowTitle("PCM Sample ROM");
        _romGlobalsWindowWindow = window;
        _mdiArea->addSubWindow(window);

        window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        window->show();
    } else {
        _mdiArea->setActiveSubWindow(_romGlobalsWindowWindow);
    }
}

void MainWindow::showChannelsWindow()
{
    if (_channelsWindow == nullptr) {
        _channelsWidget = new ChannelsWidget(this, _app);

        connect(_channelsWidget, &ChannelsWidget::pianoRollTriggered, this, &MainWindow::pianoRollTriggered);
        connect(_channelsWidget, &ChannelsWidget::channelSelected, this, &MainWindow::channelSelected);

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

void MainWindow::seekClicked(const float percent)
{
    if (_app->chromasound().isPlaying() || _app->chromasound().isPaused()) {
        return;
    }

    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        float patternLength = _app->project().patterns().empty() ? _app->project().beatsPerBar() : _app->project().getPatternBarLength(_app->project().frontPattern());
        float newPos = percent * (float)patternLength;

        _app->setPosition(newPos);
    } else {
        float songLength = qCeil(_app->project().getLength()/_app->project().beatsPerBar()) * _app->project().beatsPerBar();
        float newPos = percent * (float)songLength;

        _app->setPosition(newPos);
    }

    doUpdate();
}

void MainWindow::loadEmptyTemplate()
{
    loadTemplate(":/templates/empty.csp");
}

void MainWindow::loadPSGTemplate()
{
    loadTemplate(":/templates/opn-psg.csp");
}

void MainWindow::loadFM4Template()
{
    loadTemplate(":/templates/opn-fm.csp");
}

void MainWindow::loadFM4PSGTemplate()
{
    loadTemplate(":/templates/opn-full.csp");
}

void MainWindow::loadSSGTemplate()
{
    loadTemplate(":/templates/opl-ssg.csp");
}

void MainWindow::loadFM2Template()
{
    loadTemplate(":/templates/opl-fm.csp");
}

void MainWindow::loadFM2SSGTemplate()
{
    loadTemplate(":/templates/opl-fm-ssg.csp");
}

void MainWindow::loadFM2RhythmTemplate()
{
    loadTemplate(":/templates/opl-fm-rhythm.csp");
}

void MainWindow::loadFM2RhythmSSGTemplate()
{
    loadTemplate(":/templates/opl-full.csp");
}

void MainWindow::cleanChanged(bool clean)
{
    updateWindowTitle();
}

void MainWindow::countoffTimeout()
{
    static int count = 3;
    if (count == 0) {
        count = 3;

        _app->record();

        ui->topWidget->setStatusMessage("Ready.");

#ifdef Q_OS_WIN
        QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
        QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
        bool isDual = settings.value(Chromasound_Studio::NumberOfChromasoundsKey, 1).toInt() == 2;

        if (!isDual) return;

        if (!_app->paused()) {
            if (_app->project().playMode() == Project::PlayMode::PATTERN) {
                PianoRollWidget* prw;
                if ((prw = dynamic_cast<PianoRollWidget*>(_mdiArea->activeSubWindow()->widget()))) {
                    if (prw->pattern().tracks().empty()) {
                        return;
                    }
                    if (prw->hasLoop()) {
                        _app->play(prw->pattern(), prw->loopStart(), prw->loopEnd());
                        return;
                    }
                }
            } else {
                PlaylistWidget* pw;
                if ((pw = dynamic_cast<PlaylistWidget*>(_mdiArea->activeSubWindow()->widget()))) {
                    if (_app->project().patterns().empty()) {
                        return;
                    }
                    if (pw->hasLoop()) {
                        _app->play(pw->loopStart(), pw->loopEnd());
                        return;
                    }
                }
            }
        }

        _app->play();
    } else {
        ui->topWidget->setStatusMessage(QString::number(count--));
        _countoffTimer.start(1000);
    }
}

void MainWindow::uploadPCM()
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
    Chromasound_Studio::Profile profile = Chromasound_Studio::Profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    if (_app->project().playMode() == Project::PlayMode::PATTERN) {
        QThread* thread = QThread::create([&]() {
            int currentOffsetData;
            int currentOffsetSamples = _app->position() / _app->project().tempo() * 60 * 44100;
            VGMStream vgmStream(profile);

            emit compileStarted();
            QByteArray vgm = vgmStream.compile(_app->project(), _app->project().getFrontPattern(), false, nullptr, -1, -1, _app->position(), &currentOffsetData);
            emit compileFinished();

            quint32 dataOffset = *(quint32*)&vgm.constData()[0x34] + 0x34;

            if (vgm[dataOffset] == 0x67) {
                quint32 size = *(quint32*)&vgm.constData()[dataOffset + 3];
                QByteArray pcmBlock = vgm.mid(dataOffset, 7 + size);

                _app->uploadPCM(pcmBlock);
            }
        });

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    } else {
        QThread* thread = QThread::create([&]() {
            int loopOffsetData;
            int currentOffsetData;
            int currentOffsetSamples = _app->position() / _app->project().tempo() * 60 * 44100;
            VGMStream vgmStream(profile);


            emit compileStarted();
            QByteArray vgm = vgmStream.compile(_app->project(), false, &loopOffsetData, -1, -1, _app->position(), &currentOffsetData);
            emit compileFinished();

            quint32 dataOffset = *(quint32*)&vgm.constData()[0x34] + 0x34;

            if (vgm[dataOffset] == 0x67) {
                quint32 size = *(quint32*)&vgm.constData()[dataOffset + 3];
                QByteArray pcmBlock = vgm.mid(dataOffset, 7 + size);

                _app->uploadPCM(pcmBlock);
            }
        });

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}

void MainWindow::windowClosed(MdiSubWindow* window)
{
    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        (*it).removeAll(window);
    }
}

void MainWindow::preLoad()
{
    if (_infoScreenDialogWindow) {
        _infoScreenDialogWindow->close();
        _infoScreenDialogWindow = nullptr;
    }

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : *it) {
            window->close();
            delete window;
        }
    }

    _app->undoStack().clear();
}

void MainWindow::load(const QString& path)
{
    _app->project() = BSON::decode(path);

    QString fileName = QFileInfo(path).fileName();

    updateWindowTitle();

    if (_app->project().showInfoOnOpen()) {
        MdiSubWindow* window = new MdiSubWindow(_mdiArea);
        window->setWidget(new ProjectInfoScreenDialog(this, _app->project().info()));
        window->resize(window->minimumSizeHint());
        if (_mdiArea->viewMode() == QMdiArea::SubWindowView) {
            window->layout()->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        }
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
        window->move(_mdiArea->rect().center() - window->rect().center());
        connect(window, &MdiSubWindow::closed, this, [&]() {
            _infoScreenDialogWindow = nullptr;
        });

        _infoScreenDialogWindow = window;
    }
}

void MainWindow::loadTemplate(const QString& path)
{
    preLoad();

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QByteArray bytes(file.readAll());

    QFile tempFile(QDir::tempPath() + QDir::separator() + "file.tmp");
    tempFile.open(QIODevice::WriteOnly);
    tempFile.write(bytes);
    tempFile.flush();
    tempFile.close();

    load(tempFile.fileName());
    postLoad();

    tempFile.remove();
}

void MainWindow::postLoad()
{
    _app->setupChromasound();

    ui->topWidget->updateFromProject(_app->project());

    _channelsWidget->rebuild();

    _playlistWidget->doUpdate(_app->position(), true);

    ui->topWidget->setStatusMessage("Template loaded.");
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle(QString("Chromasound Studio [%1%2]")
                       .arg((_app->project().path().isNull() || _app->project().path().startsWith(".")) ? "untitled" : QFileInfo(_app->project().path()).fileName())
                       .arg(_app->undoStack().isClean() ? "" : "*"));
}

void MainWindow::doUpdate()
{
    float position = _app->position();

    ui->topWidget->doUpdate(position);
    if (_channelsWindow) _channelsWidget->doUpdate(position, true);
    if (_playlistWindow) _playlistWidget->doUpdate(position, true);
    if (_fmGlobalsWindow) _fmGlobalsWidget->doUpdate();
    if (_ssgGlobalsWindow) _ssgGlobalsWidget->doUpdate();
    if (_melodyGlobalsWindow) _melodyGlobalsWidget->doUpdate();
    if (_romGlobalsWindow) _romGlobalsWindow->doUpdate();
    if (_infoDialog) _infoDialog->doUpdate();

    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            PianoRollWidget* prw;
            MelodyWidget* mw;
            PCMWidgetWindow* rw;
            if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate(position, true);
            }
            if ((mw = dynamic_cast<MelodyWidget*>(window->widget()))) {
                mw->doUpdate();
            }
            if ((rw = dynamic_cast<PCMWidgetWindow*>(window->widget()))) {
                rw->doUpdate();
            }

            QString tail = "Window";
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
            } else if (dynamic_cast<PCMWidgetWindow*>(window->widget())) {
                tail = "PCM";
            }

            window->setWindowTitle(QString("%1: %2").arg(_app->project().getChannel(it.key()).name()).arg(tail));
        }
    }
}

void MainWindow::channelSettingsUpdated()
{
    for (auto it = _channelWindows.begin(); it != _channelWindows.end(); ++it) {
        for (MdiSubWindow* window : (*it)) {
            NoiseWidget* nw;
            FMWidgetWindow* fmw;
            SSGWidget* sw;
            PianoRollWidget* prw;
            MelodyWidget* mw;
            RhythmWidget* rw;
            PCMWidgetWindow* romw;

            if ((nw = dynamic_cast<NoiseWidget*>(window->widget()))) {
                nw->doUpdate();
            } else if ((fmw = dynamic_cast<FMWidgetWindow*>(window->widget()))) {
                fmw->doUpdate();
            } else if ((romw = dynamic_cast<PCMWidgetWindow*>(window->widget()))) {
                romw->doUpdate();
            } else if ((sw = dynamic_cast<SSGWidget*>(window->widget()))) {
                sw->doUpdate();
            } else if ((mw = dynamic_cast<MelodyWidget*>(window->widget()))) {
                mw->doUpdate();
            } else if ((rw = dynamic_cast<RhythmWidget*>(window->widget()))) {
                rw->doUpdate();
            } else if ((prw = dynamic_cast<PianoRollWidget*>(window->widget()))) {
                prw->doUpdate(_app->position(), true);
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
    if (!_shownEver) {
        showChannelsWindow();
        showPlaylistWindow();

        loadPSGTemplate();

        _shownEver = true;
    }

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

        preLoad();
        load(path);
        postLoad();

        if (_channelsWindow) _channelsWidget->rebuild();
        if (_playlistWindow) _playlistWidget->update();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(fileInfo.fileName()));
    } else if (fileInfo.suffix() == "mid") {
        _app->undoStack().push(new LoadMultiTrackMIDICommand(this, path));

        if (_channelsWindow) _channelsWidget->rebuild();
        doUpdate();

        ui->topWidget->setStatusMessage(QString("Opened %1.").arg(fileInfo.fileName()));
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!_app->undoStack().isClean()) {
        QString message;
        if (_app->project().path().isNull() || _app->project().path().startsWith(".")) {
            message = "Save your project?";
        } else {
            message = QString("Save changes to %1?").arg(QFileInfo(_app->project().path()).fileName());
        }

        QMessageBox::StandardButton reply = QMessageBox::question(this, "Save changes", message);

        if (reply == QMessageBox::Yes) {
            saveTriggered();
        }
    }
}



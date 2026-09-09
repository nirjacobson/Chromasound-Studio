#include "songmakerdialog.h"
#include "tools/songmaker/ui_songmakerdialog.h"
#include "ui_songmakerdialog.h"

#include "mainwindow.h"

SongMakerDialog::SongMakerDialog(QWidget *parent)
    : QMainWindow(parent)
    , _mainWindow(dynamic_cast<MainWindow*>(parent))
    , _backend(Backend::CPU)
    , ui(new Ui::SongMakerDialog)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    connect(ui->actionOpen, &QAction::triggered, this, &SongMakerDialog::openModels);
    connect(ui->actionSave, &QAction::triggered, this, &SongMakerDialog::saveModels);
    connect(ui->actionClear, &QAction::triggered, this, &SongMakerDialog::clearModels);
    connect(ui->actionRandom, &QAction::triggered, this,  &SongMakerDialog::random);
    ui->actionRandom->setShortcut({QKeySequence("Alt+R")});
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
    ui->menubar->setNativeMenuBar(false);

    connect(ui->octavesComboBox, &QComboBox::currentIndexChanged, this, &SongMakerDialog::chordsSelectionChanged);

    connect(ui->backendButton, &QPushButton::clicked, this, &SongMakerDialog::backendButtonClicked);

    QDirListing octavesDirList(":/unison/progressions/");

    for (const auto &entry : octavesDirList) {
        QString name = entry.fileName().mid(5);
        _chordsPaths.insert(name, entry.filePath());
        ui->octavesComboBox->addItem(name);
    }

    _melodySetsPaths.insert("Default", ":/unison/melodies/");
    ui->melodySetComboBox->addItem("Default");

    QDirListing drumkitsDirList(":/unison/drumkits/");

    for (const auto &entry : drumkitsDirList) {
        QString name = entry.fileName().mid(0, entry.fileName().indexOf("."));
        _drumkitsPaths.insert(name, entry.filePath());;
        ui->drumTrackComboBox->addItem(name);
    }

    connect(ui->okPushButton, &QPushButton::clicked, this, &SongMakerDialog::okButtonClicked);

    std::vector<QLCDNumber*> lossWidgets = {
        ui->loss1LcdNumber,
        ui->loss2LcdNumber,
        ui->loss3LcdNumber,
        ui->loss4LcdNumber,
        ui->loss5LcdNumber
    };
    for(int i = 0; i < 5; i++) {
        lossWidgets[i]->display("-----");
    }

    _backends.push_back(Backend::CPU);
    _backendIcons.push_back(QIcon(":/icons/cpu.png"));
    if (at::is_vulkan_available()) {
        _backends.push_back(Backend::Vulkan);
        _backendIcons.push_back(QIcon(":/icons/vulkan.svg"));
    }
    if (torch::cuda::is_available()) {
        _backends.push_back(Backend::CUDA);
        _backendIcons.push_back(QIcon(":/icons/cuda.svg"));
    }

    ui->backendButton->setIcon(QIcon(":/icons/cpu.png"));
    ui->backendButton->setIconSize(QSize(48, 48));
}

SongMakerDialog::~SongMakerDialog()
{
    delete ui;
}

void SongMakerDialog::chordsSelectionChanged(const int index)
{
    ui->progressionSetComboBox->clear();

    QString chords = ui->octavesComboBox->currentText();
    QDirListing octavesDirList(":/unison/progressions/", {QString("*%1").arg(chords)});

    QString progressionsDirPath = octavesDirList.begin()->filePath();

    QDirListing progressionsDirList(progressionsDirPath);
    for (const auto &entry : progressionsDirList) {
        QString name = entry.fileName().at(0).toUpper() + entry.fileName().mid(1);
        ui->progressionSetComboBox->addItem(name);
    }
}

void SongMakerDialog::okButtonClicked()
{
    ui->taskProgressBar->setValue(0);
    ui->totalProgressBar->setValue(0);

    QThread* workerThread = new QThread(this);

    int trainingIters = 0;
    switch (ui->trainingLevelSlider->value()) {
    case 0:
        trainingIters = 10;
        break;
    case 1:
        trainingIters = 50;
        break;
    case 2:
        trainingIters = 100;
        break;
    case 3:
        trainingIters = 500;
        break;
    case 4:
        trainingIters = 1000;
        break;
    }

    auto lamb = [this,workerThread](Model chordsModel, Model phrasesModel, Model timesModel) {
        auto generatingWorkerChords = new ChordGenerationWorker(this);
        connect(generatingWorkerChords, &ChordGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
        connect(generatingWorkerChords, &ChordGenerationWorker::finished, this, [this, workerThread, generatingWorkerChords, phrasesModel, timesModel](std::vector<torch::Tensor> chords){
            delete generatingWorkerChords;

            QList<Track::Item*> items;
            int time = 0;
            torch::Tensor negOne = torch::tensor({-1});
            std::vector<torch::Tensor> last_cni = torch::unbind(moved_tensor(negOne), 0);
            for (const torch::Tensor chord : chords) {
                std::vector<torch::Tensor> chord_notes;
                chord_notes.push_back(chord[Chord::Root]);
                int add = (chord[Chord::MajorMinor].item().toBool() ? 4 : 3);
                chord_notes.push_back(chord[Chord::Root] + add);
                chord_notes.push_back(chord[Chord::Root] + add + (7 - add));
                std::vector<torch::Tensor> chord_notes_inverted;
                for (int i = chord[Chord::Inversion].item().toInt(); i < 3; i++) {
                    chord_notes_inverted.push_back(chord_notes[i]);
                }
                for (int i = 0; i < chord[Chord::Inversion].item().toInt(); i++) {
                    torch::Tensor note = chord_notes[i] + 12;

                    chord_notes_inverted.push_back(note);
                }
                chord_notes_inverted.push_back(chord_notes_inverted[0] + 12);

                if (last_cni.size() > 1) {
                    if (torch::abs(chord_notes_inverted[0] - last_cni[0]).item().toInt() > 6) {
                        for (int i = 0; i < chord_notes_inverted.size(); i++) {
                            bool tooHigh = chord_notes_inverted[i].item().toInt() > last_cni[i].item().toInt();
                            while (torch::abs(chord_notes_inverted[i] - last_cni[i]).item().toInt() > 6) {
                                if (tooHigh) {
                                    chord_notes_inverted[i] -= 12;
                                } else {
                                    chord_notes_inverted[i] += 12;
                                }
                            }
                        }
                    }
                }

                for (int i = 0; i < 4; i++) {
                    Track::Item* item = new Track::Item(time, Note(chord_notes_inverted[i].item().toInt(), chord[Chord::ChordDuration].item().toFloat() / 480));
                    items.append(item);
                }
                time += chord[Chord::ChordDuration].item().toFloat() / 480;

                last_cni = chord_notes_inverted;
            }

            _mainWindow->loadEmptyTemplate();

            AddChannelCommand* channelCommand = new AddChannelCommand(_mainWindow);
            _mainWindow->_app->undoStack().push(channelCommand);

            _mainWindow->setChannelVolume(0, 40);

            SetChannelNameCommand* channelNameCommand = new SetChannelNameCommand(_mainWindow, _mainWindow->_app->project().getChannel(0), "Chords");
            _mainWindow->_app->undoStack().push(channelNameCommand);

            SetChannelTypeCommand* channelTypeCommand = new SetChannelTypeCommand(_mainWindow, _mainWindow->_app->project().getChannel(0), Channel::Type::FM);
            _mainWindow->_app->undoStack().push(channelTypeCommand);

            FMChannelSettings ivoryEbondy = OPM::parseOPM(":/factory/dx100_1_1.opm")["IvoryEbony"];
            SetFMChannelSettingsCommand* fmSettingsCommand = new SetFMChannelSettingsCommand(_mainWindow, dynamic_cast<FMChannelSettings&>(_mainWindow->_app->project().getChannel(0).settings()), ivoryEbondy);
            _mainWindow->_app->undoStack().push(fmSettingsCommand);

            Track& track = _mainWindow->app()->project().getPattern(0).getTrack(0);
            ReplaceTrackItemsCommand* itemsCommand = new ReplaceTrackItemsCommand(_mainWindow, track, items);
            _mainWindow->_app->undoStack().push(itemsCommand);

            _mainWindow->_app->project().getPattern(0).getTrack(0).usePianoRoll();

            auto generatingWorkerPhrases = new PhraseGenerationWorker(this);
            connect(generatingWorkerPhrases, &PhraseGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
            connect(generatingWorkerPhrases, &PhraseGenerationWorker::finished, this, [this, workerThread, generatingWorkerPhrases, chords, phrasesModel, timesModel](torch::Tensor phrases){
                delete generatingWorkerPhrases;
                auto generatingWorkerTimes = new TimeGenerationWorker(this);

                connect(generatingWorkerTimes, &TimeGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                connect(generatingWorkerTimes, &TimeGenerationWorker::finished, this, [this, workerThread, generatingWorkerTimes, chords, phrasesModel, phrases](torch::Tensor times) {
                    delete generatingWorkerTimes;
                    auto chords_phrases = phrases_by_chord(chords, phrases);
                    auto generatingWorkerNotes = new NoteGenerationWorker(this);
                    connect(generatingWorkerNotes, &NoteGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                    connect(generatingWorkerNotes, &NoteGenerationWorker::finished, this, [this, workerThread, generatingWorkerNotes, chords, phrasesModel](torch::Tensor melody) {
                        delete generatingWorkerNotes;
                        delete workerThread;
                        ui->totalProgressBar->setValue(100);

                        QList<Track::Item*> items;
                        float time = 0;
                        for (const torch::Tensor note : melody.unbind(0)) {
                            Track::Item* item = new Track::Item(time, Note(note[0].item().toInt(), note[1].item().toFloat() / 480.0f));
                            items.append(item);
                            time += note[1].item().toFloat() / 480.0f;
                        }

                        AddChannelCommand* channelCommand = new AddChannelCommand(_mainWindow);
                        _mainWindow->_app->undoStack().push(channelCommand);

                        _mainWindow->setChannelVolume(1, 60);

                        SetChannelNameCommand* channelNameCommand = new SetChannelNameCommand(_mainWindow, _mainWindow->_app->project().getChannel(1), "Melody");
                        _mainWindow->_app->undoStack().push(channelNameCommand);

                        SetChannelTypeCommand* channelTypeCommand = new SetChannelTypeCommand(_mainWindow, _mainWindow->_app->project().getChannel(1), Channel::Type::FM);
                        _mainWindow->_app->undoStack().push(channelTypeCommand);

                        FMChannelSettings panFloot = OPM::parseOPM(":/factory/dx100_3_2.opm")["Pan Floot"];
                        SetFMChannelSettingsCommand* fmSettingsCommand = new SetFMChannelSettingsCommand(_mainWindow, dynamic_cast<FMChannelSettings&>(_mainWindow->_app->project().getChannel(1).settings()), panFloot);
                        _mainWindow->_app->undoStack().push(fmSettingsCommand);

                        Track& track = _mainWindow->app()->project().getPattern(1).getTrack(1);
                        ReplaceTrackItemsCommand* itemsCommand = new ReplaceTrackItemsCommand(_mainWindow, track, items);
                        _mainWindow->_app->undoStack().push(itemsCommand);
                        _mainWindow->_app->project().getPattern(1).getTrack(1).usePianoRoll();

                        QString path = QFile(ui->drumTrackComboBox->currentText()).exists()
                                           ? ui->drumTrackComboBox->currentText()
                                           : _drumkitsPaths[ui->drumTrackComboBox->currentText()];

                        channelCommand = new AddChannelCommand(_mainWindow);
                        _mainWindow->_app->undoStack().push(channelCommand);

                        channelNameCommand = new SetChannelNameCommand(_mainWindow, _mainWindow->_app->project().getChannel(2), "Drums");
                        _mainWindow->_app->undoStack().push(channelNameCommand);

                        channelTypeCommand = new SetChannelTypeCommand(_mainWindow, _mainWindow->_app->project().getChannel(2), Channel::Type::PCM);
                        _mainWindow->_app->undoStack().push(channelTypeCommand);

                        SetProjectPCMFileCommand* pcmFileCommand = new SetProjectPCMFileCommand(_mainWindow, _mainWindow->_app->project(), ":/factory/drums.rom");
                        _mainWindow->_app->undoStack().push(pcmFileCommand);

                        QString tempFilePath = QDir::homePath() + "/" + ".tmp";
                        QFile (":/factory/drums.lay").copy(tempFilePath);
                        SetPCMChannelCommand* pcmChannelCommand = new SetPCMChannelCommand(_mainWindow, _mainWindow->_app->project().getChannel(2), *BSON::decodePCMLayout(tempFilePath));
                        QFile(tempFilePath).remove();
                        _mainWindow->_app->undoStack().push(pcmChannelCommand);

                        QFile midFile = QFile(path);
                        MIDIFile mfile(midFile);

                        auto it1 = std::find_if(mfile.chunks().begin(), mfile.chunks().end(), [](const MIDIChunk* chunk) {
                            return dynamic_cast<const MIDIHeader*>(chunk);
                        });

                        QList<Track::Item*> drumItems;
                        if (it1 != mfile.chunks().end()) {
                            const MIDIHeader* header = dynamic_cast<const MIDIHeader*>(*it1);

                            auto trackIt = std::find_if(mfile.chunks().begin(), mfile.chunks().end(), [](const MIDIChunk* chunk) {
                                return dynamic_cast<const MIDITrack*>(chunk);
                            });

                            const MIDITrack* midTrack = dynamic_cast<const MIDITrack*>(*trackIt);

                            drumItems = MIDI::toTrackItems(*midTrack, header->division());

                            Track& track = _mainWindow->app()->project().getPattern(2).getTrack(2);
                            ReplaceTrackItemsCommand* itemsCommand = new ReplaceTrackItemsCommand(_mainWindow, track, drumItems);
                            _mainWindow->_app->undoStack().push(itemsCommand);
                            _mainWindow->_app->project().getPattern(2).getTrack(2).usePianoRoll();
                        }

                        int chordsMelodySum =  items.last()->time() + items.last()->duration();
                        int drumsSum = drumItems.last()->time() + drumItems.last()->duration();


                        if (chordsMelodySum < drumsSum) {
                            for (int i = 0; i < drumsSum / chordsMelodySum; i++) {
                                AddPlaylistItemCommand* playlistCommand1 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), i * chordsMelodySum, 0);
                                AddPlaylistItemCommand* playlistCommand2 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), i * chordsMelodySum, 1);
                                _mainWindow->_app->undoStack().push(playlistCommand1);
                                _mainWindow->_app->undoStack().push(playlistCommand2);
                            }
                            AddPlaylistItemCommand* playlistCommand3 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 2);
                            _mainWindow->_app->undoStack().push(playlistCommand3);
                        } else if (chordsMelodySum == drumsSum) {
                            AddPlaylistItemCommand* playlistCommand1 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 0);
                            AddPlaylistItemCommand* playlistCommand2 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 1);
                            AddPlaylistItemCommand* playlistCommand3 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 2);
                            _mainWindow->_app->undoStack().push(playlistCommand1);
                            _mainWindow->_app->undoStack().push(playlistCommand2);
                            _mainWindow->_app->undoStack().push(playlistCommand3);
                        } else {
                            AddPlaylistItemCommand* playlistCommand1 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 0);
                            AddPlaylistItemCommand* playlistCommand2 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), 0, 1);
                            _mainWindow->_app->undoStack().push(playlistCommand1);
                            _mainWindow->_app->undoStack().push(playlistCommand2);
                            for (int i = 0; i < chordsMelodySum / drumsSum; i++) {
                                AddPlaylistItemCommand* playlistCommand3 = new AddPlaylistItemCommand(_mainWindow, _mainWindow->app()->project().playlist(), i * drumsSum, 2);
                                _mainWindow->_app->undoStack().push(playlistCommand3);
                            }
                        }

                        _mainWindow->_app->project().playlist().setLoopOffset(0);
                        _mainWindow->setPlayMode(Project::PlayMode::SONG);


                        ui->statusbar->showMessage("Done.");
                        _mainWindow->setStatusMessage("Enjoy!");
                        enable_fields();
                    });

                    ui->totalProgressBar->setValue(84);
                    generatingWorkerNotes->moveToThread(workerThread);
                    ui->statusbar->showMessage("Generating melody...");
                    generatingWorkerNotes->doGenerateNotes(chords_phrases, times);
                });
                ui->totalProgressBar->setValue(70);
                generatingWorkerTimes->moveToThread(workerThread);
                ui->statusbar->showMessage("Generating durations...");
                generatingWorkerTimes->doGenerateTimes(timesModel, ui->barsSpinBox->value());
            });
            ui->totalProgressBar->setValue(56);
            generatingWorkerPhrases->moveToThread(workerThread);
            ui->statusbar->showMessage("Generating phrases...");
            generatingWorkerPhrases->doGeneratePhrases(phrasesModel, ui->barsSpinBox->value());
        });
        ui->totalProgressBar->setValue(42);
        generatingWorkerChords->moveToThread(workerThread);
        ui->statusbar->showMessage("Generating chords...");
        generatingWorkerChords->doGenerateChords(chordsModel, ui->barsSpinBox->value());
    };

    QString path = QFile(ui->octavesComboBox->currentText()).exists()
        ? ui->octavesComboBox->currentText()
        : _chordsPaths[ui->octavesComboBox->currentText()] + "/" +
              ui->progressionSetComboBox->currentText().toLower() + "/" +
              (ui->majorRadioButton->isChecked() ? "major" : "minor");
    QFile folder = QFile(path);
    if (folder.exists()) {
        QFileInfo folderInfo = QFileInfo(folder);
        if (folderInfo.isDir()) {
            disable_fields();
            if (ui->retrainButton->isChecked()) {
                clearModels();
            }
            if (std::get<0>(_chordsModel).empty()) {
                auto XY = build_chords_dataset(folderInfo.filePath().toStdString());

                auto trainingWorkerChords = new TrainingWorker(this);
                connect(trainingWorkerChords, &TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                connect(trainingWorkerChords, &TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                connect(trainingWorkerChords, &TrainingWorker::finished, this, [this, lamb, workerThread, trainingWorkerChords, trainingIters](Model chordsModel){
                    _chordsModel = chordsModel;
                    delete trainingWorkerChords;
                    QString melodiesPath = QFile(ui->melodySetComboBox->currentText()).exists()
                                               ? ui->melodySetComboBox->currentText()
                                               : _melodySetsPaths[ui->melodySetComboBox->currentText()];
                    QFile melodiesFolder = QFile(melodiesPath);
                    QFileInfo melodiesFolderInfo = QFileInfo(melodiesFolder);
                    if (melodiesFolderInfo.isDir()) {
                        auto XY = build_phrases_dataset(melodiesFolderInfo.filePath().toStdString());
                        auto trainingWorkerPhrases = new TrainingWorker(this);
                        connect(trainingWorkerPhrases, &TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                        connect(trainingWorkerPhrases, &TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                        connect(trainingWorkerPhrases, &TrainingWorker::finished, this, [this, lamb, workerThread, chordsModel, trainingWorkerPhrases, melodiesFolderInfo, trainingIters](Model phrasesModel) {
                            _phrasesModel = phrasesModel;
                            auto XY = build_times_dataset(melodiesFolderInfo.filePath().toStdString());
                            auto trainingWorkerTimes = new TrainingWorker(this);
                            connect(trainingWorkerTimes, &TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                            connect(trainingWorkerTimes, &TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                            connect(trainingWorkerTimes, &TrainingWorker::finished, this, [this, lamb, workerThread, chordsModel, trainingWorkerPhrases, melodiesFolderInfo, phrasesModel](Model timesModel) {
                                _durationsModel = timesModel;
                                lamb(chordsModel, phrasesModel, timesModel);
                            });

                            ui->totalProgressBar->setValue(28);
                            trainingWorkerTimes->moveToThread(workerThread);
                            ui->statusbar->showMessage("Training note durations...");
                            trainingWorkerTimes->doTrain(torch::tensor({721}), torch::tensor(ui->durationsModelWidget->layerSizes()), trainingIters, XY.first, XY.second);
                        });
                        ui->totalProgressBar->setValue(14);
                        trainingWorkerPhrases->moveToThread(workerThread);
                        ui->statusbar->showMessage("Training phrases...");
                        trainingWorkerPhrases->doTrain(torch::tensor({3, 8, 961, 88, 3}), torch::tensor(ui->phrasesModelWidget->layerSizes()), trainingIters, XY.first, XY.second);
                    }
                });

                trainingWorkerChords->moveToThread(workerThread);
                ui->statusbar->showMessage("Training chords...");
                trainingWorkerChords->doTrain(torch::tensor({12, 2, 4, 1921}), torch::tensor(ui->chordsModelWidget->layerSizes()), trainingIters, XY.first, XY.second);
            } else {
                lamb(_chordsModel, _phrasesModel, _durationsModel);
            }

        }
    }
}

void TrainingWorker::doTrain(const torch::Tensor& classes, const torch::Tensor& ll_sizes, const int iters, const torch::Tensor &X, const torch::Tensor &Y) {
    _result = train(classes, ll_sizes, iters, X, Y);
    emit finished(_result);
}

Model TrainingWorker::train(const torch::Tensor classes, const torch::Tensor& ll_sizes, const int iters, const torch::Tensor &x, const at::Tensor &y)
{
    torch::Tensor Classes = _dialog->moved_tensor(classes);
    torch::Tensor LlSizes = _dialog->moved_tensor(ll_sizes);
    torch::Tensor X = _dialog->moved_tensor(x);
    torch::Tensor Y = _dialog->moved_tensor(y);

    std::vector<std::vector<torch::Tensor>> W1;
    std::vector<std::vector<torch::Tensor>> b1;
    std::vector<std::vector<torch::Tensor>> W;
    std::vector<std::vector<torch::Tensor>> b;
    std::vector<std::vector<torch::Tensor>> W2;
    std::vector<std::vector<torch::Tensor>> b2;
    W2.push_back({});
    b2.push_back({});

    std::vector<torch::Tensor> probs = {};
    for (int i = 0; i < classes.size(0); i++) {
        probs.push_back({});
    }

    std::vector<torch::Tensor> loss;

    for (int i = 0; i < iters; i++) {
        std::vector<torch::Tensor> h;
        torch::Tensor h1;
        torch::Tensor h11;
        for (int j = 0; j < ll_sizes.size(0); j++) {
            if (j == 0) {
                if (W1.size() == j) {
                    W1.push_back({});
                }
                if (b1.size() == j) {
                    b1.push_back({});
                }
            } else {
                if (W.size() == j-1) {
                    W.push_back({});
                }
                if (b.size() == j-1) {
                    b.push_back({});
                }
            }
            torch::Tensor xview = X.view({x.size(0), -1});

            for (int k = 0; k < classes.size(0); k++) {
                // Forward pass
                if (j == 0) {
                    if (W1[j].size() == k) {
                        torch::Tensor l_size = torch::tensor(x.sizes()[1] * x.sizes()[2]);
                        l_size = _dialog->moved_tensor(l_size);
                        torch::Tensor mat = torch::randn({l_size.item().toInt(), ll_sizes[j].item().toInt()});
                        W1[j].push_back(_dialog->moved_tensor(mat));
                        W1[j][k].set_requires_grad(true);
                    }
                } else {
                    if (W[j-1].size() == k) {
                        torch::Tensor l_size = ll_sizes[j-1];
                        torch::Tensor mat = torch::randn({l_size.item().toInt(), ll_sizes[j].item().toInt()});
                        W[j-1].push_back(mat);
                        W[j-1][k].set_requires_grad(true);
                    }
                }

                if (j == 0) {
                    if (b1[j].size() == k) {
                        torch::Tensor vec = torch::randn({ll_sizes[j].item().toInt()});
                        b1[j].push_back(_dialog->moved_tensor(vec));
                        b1[j][k].set_requires_grad(true);
                    }
                } else {
                    if (b[j-1].size() == k) {
                        torch::Tensor vec = torch::randn({ll_sizes[j].item().toInt()});
                        b[j-1].push_back(_dialog->moved_tensor(vec));
                        b[j-1][k].set_requires_grad(true);
                    }
                }

                if (j == 0) {
                    h1 = xview.to(torch::kFloat32).matmul(W1[j][k]) + (b1[j][k]);
                    h.push_back(h1);
                } else if (j > 0) {
                    h1 = h[k].matmul(W[j-1][k]) + (b[j-1][k]);
                    h[k] = h1;
                }

                if (j == ll_sizes.size(0)-1) {
                    if (W2[0].size() == k) {
                        torch::Tensor mat = torch::randn({ll_sizes[-1].item().toInt(), classes[k].item().toInt()});
                        W2[0].push_back(mat);
                        W2[0][k].set_requires_grad(true);
                    }
                    if (b2[0].size() == k) {
                        torch::Tensor vec = torch::randn({classes[k].item().toInt()});
                        b2[0].push_back(_dialog->moved_tensor(vec));
                        b2[0][k].set_requires_grad(true);
                    }

                    torch::Tensor h2 = tanh(h[k].matmul(W2[0][k]) + b2[0][k]);

                    torch::Tensor counts = h2.exp();
                    probs[k] = (counts / counts.sum(1, true)).view({-1, Classes[k].item().toInt()});
                }
            }
        }

        for (int k = 0; k < Classes.size(0); k++) {
            torch::Tensor y_int = Y.to(torch::kInt64);
            torch::Tensor y_param = y_int.matmul(torch::nn::functional::one_hot(torch::tensor(k), classes.size(0)));
            std::vector<std::pair<int, int>> pairs;
            for (int l = 0; l < x.size(0); l++) {
                pairs.push_back(std::make_pair(l, y_param[l].item().toInt()));
            }
            std::vector<torch::Tensor> _losses;
            for (int l = 0; l < x.size(0); l++) {
                torch::Tensor __loss = -probs[k][pairs[l].first][pairs[l].second].log();
                _losses.push_back(_dialog->moved_tensor(__loss));
            }
            torch::Tensor _loss = torch::stack(_losses).mean();

            if (loss.size() == k) {
                loss.push_back(_loss);
            } else {
                loss[k] = _loss;
            }
        }
        emit lossUpdate(loss);

        // Backward pass
        for (auto& v : {&(W1[0]), &(b1[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].grad().zero_();
                }
            }
        }
        for (auto& v : {&W, &b}) {
            for (int lyr = 0; lyr < ll_sizes.size(0) - 1; lyr++) {
                for (int prm = 0; prm < classes.size(0); prm++) {
                    if ((*v)[lyr][prm].grad().defined()) {
                        (*v)[lyr][prm].grad().zero_();
                    }
                }
            }
        }
        for (auto& v : {&(W2[0]), &(b2[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].grad().zero_();
                }
            }
        }

        for (int prm = 0; prm < classes.size(0); prm++) {
            loss[prm].backward({}, true);
        }

        for (auto& v : {&(W1[0]), &(b1[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].data() += -0.25 * (*v)[prm].grad();
                }
            }
        }
        for (auto& v : {&W, &b}) {
            for (int lyr = 0; lyr < ll_sizes.size(0) - 1; lyr++) {
                for (int prm = 0; prm < classes.size(0); prm++) {
                    if ((*v)[lyr][prm].grad().defined()) {
                        (*v)[lyr][prm].data() += -0.25 * (*v)[lyr][prm].grad();
                    }
                }
            }
        }
        for (auto& v : {&(W2[0]), &(b2[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].data() += -0.25 * (*v)[prm].grad();
                }
            }
        }
        updateProgress((int)(((float)i/(float)iters) * 100));
    }
    updateProgress(100);
    emit lossUpdate({torch::zeros(5).unbind()});

    torch::Tensor layer_models;
    Tensors parameter_models_W1;
    for (int m = 0; m < classes.size(0); m++) {
        if (layer_models.size(0) == 0) {
            layer_models = torch::unsqueeze(W1[0][m], 0);
        } else {
            layer_models = torch::cat({layer_models, torch::unsqueeze(W1[0][m], 0)});
        }
    }
    parameter_models_W1 = {layer_models};

    layer_models = torch::tensor({});

    Tensors parameter_models_b1;
    for (int m = 0; m < classes.size(0); m++) {
        if (layer_models.size(0) == 0) {
            layer_models = torch::unsqueeze(b1[0][m], 0);
        } else {
            layer_models = torch::cat({layer_models, torch::unsqueeze(b1[0][m], 0)});
        }
    }
    parameter_models_b1 = { layer_models };

    Tensors parameter_models_W;
    for (int l = 0; l < ll_sizes.size(0) - 1; l++) {
        layer_models = torch::tensor({});
        for (int m = 0; m < classes.size(0); m++) {
            if (layer_models.size(0) == 0) {
                layer_models = torch::unsqueeze(W[l][m], 0);
            } else {
                layer_models = torch::cat({layer_models, torch::unsqueeze(W[l][m], 0)});
            }
        }
        if (parameter_models_W.size() == 0) {
            parameter_models_W = { layer_models };
        } else {
            parameter_models_W.push_back(layer_models);
        }
    }

    Tensors parameter_models_b;
    for (int l = 0; l < ll_sizes.size(0) - 1; l++) {
        layer_models = torch::tensor({});
        for (int m = 0; m < classes.size(0); m++) {
            if (layer_models.size(0) == 0) {
                layer_models = torch::unsqueeze(b[l][m], 0);
            } else {
                layer_models = torch::cat({layer_models, torch::unsqueeze(b[l][m], 0)});
            }
        }

        if (parameter_models_b.size() == 0) {
            parameter_models_b = { layer_models };
        } else {
            parameter_models_b.push_back(layer_models);
        }
    }

    layer_models = torch::tensor({});

    Tensors parameter_models_W2;
    for (int m = 0; m < classes.size(0); m++) {
        parameter_models_W2.push_back(W2[0][m]);
    }

    layer_models = torch::tensor({});

    Tensors parameter_models_b2;
    for (int m = 0; m < classes.size(0); m++) {
        parameter_models_b2.push_back(b2[0][m]);
    }

    std::vector<torch::Tensor> classes_vec = { Classes };
    std::vector<torch::Tensor> ll_sizes_vec = { LlSizes };

    Model m = std::make_tuple(parameter_models_W1, parameter_models_b1, parameter_models_W, parameter_models_b, parameter_models_W2, parameter_models_b2, classes_vec, ll_sizes_vec);

    return m;
}

std::vector<torch::Tensor> GenerationWorker::forward(const Model &model, const at::Tensor &x)
{
    torch::Tensor _x = x;
    const torch::Tensor X = _dialog->moved_tensor(_x);
    const torch::Tensor& classes = std::get<6>(model)[0].cpu();
    int layers = std::get<2>(model).size() + 1;

    torch::Tensor xview = X.view({X.size(0), -1});
    std::vector<torch::Tensor> probs;
    for (int i = 0; i < classes.size(0); i++) {
        torch::Tensor h1;
        torch::Tensor _h1;
        for (int j = 0; j < layers; j++) {
            // Forward pass
            torch::Tensor logits = (j == 0) ? xview.to(torch::kFloat32) : h1;
            if (j == 0) {
                _h1 = torch::tanh(logits.matmul(std::get<0>(model)[j][i]) + std::get<1>(model)[j][i]);
            } else {
                _h1 = torch::tanh(logits.matmul(std::get<2>(model)[j-1][i]) + std::get<3>(model)[j-1][i]);
            }
            h1 = _dialog->moved_tensor(_h1);

            if (j == layers-1) {
                torch::Tensor h2 = tanh(h1.matmul(std::get<4>(model)[i]) + std::get<5>(model)[i]);
                h2 = _dialog->moved_tensor(h2);

                torch::Tensor counts = h2.exp();
                if (probs.size() == i) {
                    probs.push_back({});
                }
                probs[i] = (counts / counts.sum(1, true)).view({-1, classes[i].item().toInt()});
                probs[i] = _dialog->moved_tensor(probs[i]);
            }
        }
    }

    return probs;
}

std::pair<torch::Tensor, torch::Tensor> SongMakerDialog::build_chords_dataset(const std::string &path) {
    std::vector<torch::Tensor> new_chords;

    std::vector<torch::Tensor> chords;
    std::vector<int> chord = std::vector<int>(4+1, 0);
    std::vector<int> last_chord;
    std::vector<int> notes;
    int note_idx = 0;

    for (const auto& entry : QDirListing(QString::fromStdString(path), QDirListing::IteratorFlag::FilesOnly)) {
        QFile midFile = QFile(entry.absoluteFilePath());
        MIDIFile mid(midFile);

        auto trackIt = std::find_if(mid.chunks().begin(), mid.chunks().end(), [](const MIDIChunk* chunk) {
            return dynamic_cast<const MIDITrack*>(chunk);
        });

        if (trackIt == mid.chunks().end()) {
            return {};
        }

        const MIDITrack* track = dynamic_cast<const MIDITrack*>(*trackIt);

        int e = 0;
        int duration = 0;
        while (e < track->events()) {
            const MIDITrackEvent& mte = track->event(e++);
            const Event& evt = mte.event();
            const MIDIEvent* msg;
            try {
                msg = &dynamic_cast<const MIDIEvent&>(evt);
                if (mte.deltaTime() != 0 || note_idx == 4) {
                    chord[4] = duration;
                    chords.push_back(torch::cat({std::get<0>(torch::tensor(chord).slice(0, 0, 4).sort()), torch::unsqueeze(torch::tensor(0), 0)}));
                    last_chord = chord;
                    chord = std::vector<int>(4+1, 0);
                    note_idx = 0;
                    duration = 0;
                }
                if (msg->isKeyOn()) {
                    chord[note_idx++] = msg->data1() % 12;
                }
                if (msg->isKeyOff()) {
                    duration += mte.deltaTime();
                    // chord[note_idx++] = msg->data1() % 12;
                }
            } catch (std::bad_cast& ex) {

            }
        }

    }

    std::map<char, std::vector<int>> letter_rems = {
        {'A', {8, 9}},
        {'B', {10, 11}},
        {'C', {0, 1}},
        {'D', {2}},
        {'E', {3, 4}},
        {'F', {5, 6}},
        {'G', {7}}
    };

    for (const auto& chord : chords) {
        torch::Tensor chord_notes = chord.slice(0, 0, 4);
        std::map<int, char> chord_notes_names;
        for (int i = 0; i < chord_notes.size(0); i++) {
            for (auto& pair : letter_rems) {
                if (std::find_if(pair.second.begin(), pair.second.end(), [&chord_notes, &i, &chord_notes_names](const int val) {
                        return val == chord_notes[i].item().toInt();
                    }) != pair.second.end()) {
                    chord_notes_names.insert(std::make_pair(chord_notes[i].item().toInt(), pair.first));
                    break;
                }
            }
        }
        std::vector<std::pair<int, char>> vec(chord_notes_names.begin(), chord_notes_names.end());

        std::sort(vec.begin(), vec.end(), [](auto a, auto b) {
            return a.second < b.second;
        });

        int inversion = 0;
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i].first == chord[0].item().toInt()) {
                inversion = i;
                break;
            }
        }

        torch::Tensor beginning = chord.slice(0, inversion, 4);
        torch::Tensor fixed_chord = torch::cat({beginning, chord.slice(0, 0, inversion)});
        fixed_chord = torch::cat({fixed_chord, torch::unsqueeze(chord[4], 0)});

        for (int i = beginning.size(0); i < 4; i++) {
            fixed_chord[i] += 12;
        }
        fixed_chord[3] = fixed_chord[0] + 12;

        bool major = (fixed_chord[1] - fixed_chord[0]).item().toInt() == 4;
        torch::Tensor new_chord = torch::tensor({fixed_chord[0].item().toInt(), (int)major, inversion, chord[Chord::ChordDuration].item().toInt()});
        new_chords.push_back(new_chord);
    }

    std::vector<torch::Tensor> elems;
    for (int i = 0; i < CONTEXT_LENGTH; i++) {
        elems.push_back(torch::zeros(Chord::ChordDuration+1));
    }
    torch::Tensor context = torch::stack(elems);
    std::vector<torch::Tensor> X, Y;

    X.push_back(context);
    Y.push_back(new_chords[0]);
    for (std::tuple<torch::Tensor, torch::Tensor> tuple : std::views::zip(new_chords, std::vector<torch::Tensor>(new_chords.begin() + 1, new_chords.end()))) {
        context = torch::cat({context.slice(0, 1), torch::stack(std::get<0>(tuple))}, 0);
        X.push_back(context);
        Y.push_back(std::get<1>(tuple));
    }

    return std::make_pair<torch::Tensor, torch::Tensor>(torch::stack(X, 0), torch::stack(Y, 0));
}

std::pair<torch::Tensor, torch::Tensor> SongMakerDialog::build_phrases_dataset(const std::string &path)
{
    std::vector<torch::Tensor> out;

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::zeros(Phrase::HistDir+1));
        }
        context = torch::stack(elems);
    };

    for (const auto& entry : QDirListing(QString::fromStdString(path), QDirListing::IteratorFlag::FilesOnly)) {
        QFile midFile = QFile(entry.absoluteFilePath());
        MIDIFile mid(midFile);

        auto trackIt = std::find_if(mid.chunks().begin(), mid.chunks().end(), [](const MIDIChunk* chunk) {
            return dynamic_cast<const MIDITrack*>(chunk);
        });

        if (trackIt == mid.chunks().end()) {
            return {};
        }

        const MIDITrack* track = dynamic_cast<const MIDITrack*>(*trackIt);

        torch::Tensor phrase = torch::zeros(Phrase::HistDir+1);
        torch::Tensor melody;
        const MIDIEvent* msg;
        const MIDIEvent* last_msg = nullptr;
        int diff = -1;
        int direction = -2;
        int _direction = -1;
        int first_note = -1;
        int count = -1;
        int duration = -1;

        int e = 0;
        while (e < track->events()) {
            const MIDITrackEvent& mte = track->event(e++);
            const Event& evt = mte.event();
            try {
                msg = &dynamic_cast<const MIDIEvent&>(evt);

                if (msg->isKeyOn()) {

                    if (last_msg != nullptr) {
                        if (first_note < 0) {
                            first_note = msg->data1();
                        }
                        diff = msg->data1() - last_msg->data1();
                        _direction = (diff == 0) ? 0 : (diff / abs(diff));

                        if (direction > -2 && _direction != direction) {
                            phrase[Phrase::Direction] = direction;
                            phrase[Phrase::Length] = count;
                            phrase[Phrase::PhraseDuration] = duration;
                            phrase[Phrase::FirstNote] = first_note;

                            diff = (melody.size(0) < 2) ? 0 : (first_note - melody[-PHRASE_HISTORY][Phrase::FirstNote].item().toInt());
                            phrase[Phrase::HistDir] = (diff == 0) ? 0 : (diff / abs(diff));

                            if (melody.size(0) == 0) {
                                melody = torch::unsqueeze(phrase, 0);
                            } else {
                                melody = torch::cat({melody, torch::unsqueeze(phrase, 0)});
                            }

                            count = -1;
                            duration = -1;
                            direction = -2;
                            first_note = -1;
                            phrase = torch::zeros(Phrase::HistDir+1);
                        } else if (direction < 0) {
                            direction = _direction;
                        }
                    }
                    last_msg = msg;
                } else if (msg->isKeyOff()) {
                    if (mte.deltaTime() != 0) {
                        count++;
                        duration += mte.deltaTime();
                    }
                }
            } catch (const std::bad_cast& e) {

            }
        }

        out.push_back(melody);

    }

    clear_context();
    std::vector<torch::Tensor> X, Y;

    for (const torch::Tensor& mel : out) {
        clear_context();
        std::vector<torch::Tensor> mel_unbound = torch::unbind(mel);
        for (std::tuple<torch::Tensor, torch::Tensor> tuple : std::views::zip(torch::unbind(mel), std::vector<torch::Tensor>(mel_unbound.begin() + 1, mel_unbound.end()))) {
            context = torch::cat({context.slice(0, 1), torch::unsqueeze(std::get<0>(tuple), 0)}, 0);
            X.push_back(context);
            Y.push_back(std::get<1>(tuple));
        }
    }

    return std::make_pair<torch::Tensor, torch::Tensor>(torch::stack(X, 0), torch::stack(Y, 0));
}

std::pair<torch::Tensor, torch::Tensor> SongMakerDialog::build_times_dataset(const std::string &path)
{
    std::vector<torch::Tensor> out;

    for (const auto& entry : QDirListing(QString::fromStdString(path), QDirListing::IteratorFlag::FilesOnly)) {
        torch::Tensor time_sequence;

        QFile midFile = QFile(entry.absoluteFilePath());
        MIDIFile mid(midFile);

        auto trackIt = std::find_if(mid.chunks().begin(), mid.chunks().end(), [](const MIDIChunk* chunk) {
            return dynamic_cast<const MIDITrack*>(chunk);
        });

        if (trackIt == mid.chunks().end()) {
            return {};
        }

        const MIDITrack* track = dynamic_cast<const MIDITrack*>(*trackIt);
        const MIDIEvent* msg;
        const MIDIEvent* last_msg = nullptr;

        bool skip = true;

        int e = 0;
        while (e < track->events()) {
            const MIDITrackEvent& mte = track->event(e++);
            const Event& evt = mte.event();
            try {
                msg = &dynamic_cast<const MIDIEvent&>(evt);

                if (msg->isKeyOn()) {
                    if (skip) {
                        skip = false;
                        continue;
                    }
                    if (mte.deltaTime() != 0) {
                        torch::Tensor elem = torch::unsqueeze(torch::tensor((int)mte.deltaTime()), 0);
                        if (time_sequence.size(0) == 0) {
                            time_sequence = elem;
                        } else {
                            time_sequence = torch::cat({time_sequence, elem});
                        }
                    }
                } else if (msg->isKeyOff()) {
                    if (mte.deltaTime() != 0) {
                        torch::Tensor elem = torch::unsqueeze(torch::tensor((int)mte.deltaTime()), 0);
                        if (time_sequence.size(0) == 0) {
                            time_sequence = elem;
                        } else {
                            time_sequence = torch::cat({time_sequence, elem});
                        }
                        skip = true;
                    }
                }
            } catch (const std::bad_cast& e) {

            }
        }
        out.push_back(time_sequence);
    }

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::unsqueeze(torch::tensor(0), 0));
        }
        context = torch::stack(elems, 0);
    };

    clear_context();
    std::vector<torch::Tensor> X, Y;

    for (const torch::Tensor& ts : out) {
        clear_context();
        std::vector<torch::Tensor> ts_unbound = torch::unbind(ts);
        for (std::tuple<torch::Tensor, torch::Tensor> tuple : std::views::zip(ts_unbound, std::vector<torch::Tensor>(ts_unbound.begin() + 1, ts_unbound.end()))) {
            context = torch::cat({context.slice(0, 1), torch::unsqueeze(torch::unsqueeze(std::get<0>(tuple), 0), 0)});
            X.push_back(context);
            Y.push_back(torch::unsqueeze(std::get<1>(tuple), 0));

        }

    }

    return std::make_pair<torch::Tensor, torch::Tensor>(torch::stack(X, 0), torch::stack(Y, 0));
}

std::vector<std::pair<torch::Tensor, torch::Tensor>> SongMakerDialog::phrases_by_chord(const std::vector<torch::Tensor> &chords, const at::Tensor &phrases)
{
    torch::Tensor phrases_copy = phrases.clone();

    std::vector<std::pair<torch::Tensor, torch::Tensor>> chords_phrases;

    for (const torch::Tensor& chord : chords) {
        int phrase_i = 0;
        torch::Tensor chord_phrases;
        int chord_time = 0;
        int rem = 0;
        while (true) {
            torch::Tensor ph = phrases_copy[phrase_i];
            ph[Phrase::PhraseDuration] += rem;
            if (chord_phrases.size(0) == 0) {
                chord_phrases = torch::unsqueeze(ph, 0);
            } else {
                chord_phrases = torch::cat({chord_phrases, torch::unsqueeze(ph, 0)});
            }
            chord_time += ph[Phrase::PhraseDuration].item().toInt();

            if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                rem = chord_time - chord[Chord::ChordDuration].item().toInt();
                chord_phrases[-1][Phrase::PhraseDuration] -= rem;

                break;
            }

            phrase_i++;
        }

        chords_phrases.push_back(std::make_pair<torch::Tensor, torch::Tensor>(chord.clone(), chord_phrases.clone()));
    }

    return chords_phrases;
}

void SongMakerDialog::load(const QString& path) {
    bson_reader_t* reader;
    const bson_t* b;
    bson_error_t error;

    if (!(reader = bson_reader_new_from_file(path.toStdString().c_str(), &error))) {
        throw  "error decoding file";
    }

    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    models_from_bson(*b);

    ui->statusbar->showMessage(QString("Opened %1.").arg(QFileInfo(path).fileName()));
}

void SongMakerDialog::disable_fields()
{
    ui->actionOpen->setDisabled(true);
    ui->actionSave->setDisabled(true);
    ui->actionRandom->setDisabled(true);
    ui->actionClose->setDisabled(true);

    ui->octavesComboBox->setDisabled(true);
    ui->progressionSetComboBox->setDisabled(true);
    ui->majorRadioButton->setDisabled(true);
    ui->minorRadioButton->setDisabled(true);
    ui->melodySetComboBox->setDisabled(true);
    ui->drumTrackComboBox->setDisabled(true);
    ui->barsSpinBox->setDisabled(true);
    ui->trainingLevelSlider->setDisabled(true);
    ui->retrainButton->setDisabled(true);
    ui->okPushButton->setDisabled(true);
}

void SongMakerDialog::enable_fields()
{
    ui->actionOpen->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionRandom->setEnabled(true);
    ui->actionClose->setEnabled(true);

    ui->octavesComboBox->setEnabled(true);
    ui->progressionSetComboBox->setEnabled(true);
    ui->majorRadioButton->setEnabled(true);
    ui->minorRadioButton->setEnabled(true);
    ui->melodySetComboBox->setEnabled(true);
    ui->drumTrackComboBox->setEnabled(true);
    ui->barsSpinBox->setEnabled(true);
    ui->trainingLevelSlider->setEnabled(true);
    ui->retrainButton->setEnabled(true);
    ui->retrainButton->setChecked(false);
    ui->okPushButton->setEnabled(true);
}

void SongMakerDialog::models_to_bson(bson_t* dst)
{
    bson_t b_fields;

    bson_t b_chords;
    bson_init(&b_chords);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "chords", &b_chords);
    BSON::fromModel(&b_chords, _chordsModel);
    bson_append_document_end(dst, &b_chords);

    bson_t b_phrases;
    bson_init(&b_phrases);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "phrases", &b_phrases);
    BSON::fromModel(&b_phrases, _phrasesModel);
    bson_append_document_end(dst, &b_phrases);

    bson_t b_durations;
    bson_init(&b_durations);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "durations", &b_durations);
    BSON::fromModel(&b_durations, _durationsModel);
    bson_append_document_end(dst, &b_durations);

    BSON_APPEND_DOCUMENT_BEGIN(dst, "fields", &b_fields);
    BSON_APPEND_UTF8(&b_fields, "octaves", ui->octavesComboBox->currentText().toStdString().c_str());
    BSON_APPEND_UTF8(&b_fields, "progressionSet", ui->progressionSetComboBox->currentText().toStdString().c_str());
    BSON_APPEND_BOOL(&b_fields, "major", ui->majorRadioButton->isChecked());
    BSON_APPEND_UTF8(&b_fields, "melodySet", ui->melodySetComboBox->currentText().toStdString().c_str());
    BSON_APPEND_UTF8(&b_fields, "drumTrack", ui->drumTrackComboBox->currentText().toStdString().c_str());
    BSON_APPEND_INT32(&b_fields, "trainingLevel", ui->trainingLevelSlider->value());
    bson_append_document_end(dst, &b_fields);
}

void SongMakerDialog::models_from_bson(bson_t merged_bson)
{
    Model chordsModel;
    Model phrasesModel;
    Model durationsModel;

    bson_iter_t merged_bson_inner;
    bson_iter_t child;
    bson_iter_t child_inner;
    bson_iter_init(&merged_bson_inner, &merged_bson);

    if (bson_iter_find_descendant(&merged_bson_inner, "chords", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _chordsModel = BSON::toModel(child_inner);
        ui->chordsModelWidget->setLayerSizes(std::get<7>(_chordsModel)[0].unbind(0));
    }

    if (bson_iter_find_descendant(&merged_bson_inner, "phrases", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _phrasesModel = BSON::toModel(child_inner);
        ui->phrasesModelWidget->setLayerSizes(std::get<7>(_phrasesModel)[0].unbind(0));
    }

    if (bson_iter_find_descendant(&merged_bson_inner, "durations", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _durationsModel = BSON::toModel(child_inner);
        ui->durationsModelWidget->setLayerSizes(std::get<7>(_durationsModel)[0].unbind(0));
    }

    bson_iter_t fields;
    bson_iter_t fieldsChild;
    if (bson_iter_find_descendant(&merged_bson_inner, "fields", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &fields)) {
        if (bson_iter_find_descendant(&fields, "octaves", &fieldsChild) && BSON_ITER_HOLDS_UTF8(&fieldsChild)) {
            ui->octavesComboBox->setCurrentText(bson_iter_utf8(&fieldsChild, nullptr));
        }
        if (bson_iter_find_descendant(&fields, "progressionSet", &fieldsChild) && BSON_ITER_HOLDS_UTF8(&fieldsChild)) {
            ui->progressionSetComboBox->setCurrentText(bson_iter_utf8(&fieldsChild, nullptr));
        }
        if (bson_iter_find_descendant(&fields, "major", &fieldsChild) && BSON_ITER_HOLDS_BOOL(&fieldsChild)) {
            if (bson_iter_bool(&fieldsChild)) {
                ui->majorRadioButton->setChecked(true);
            } else {
                ui->minorRadioButton->setChecked(true);
            }
        }
        if (bson_iter_find_descendant(&fields, "melodySet", &fieldsChild) && BSON_ITER_HOLDS_UTF8(&fieldsChild)) {
            ui->melodySetComboBox->setCurrentText(bson_iter_utf8(&fieldsChild, nullptr));
        }
        if (bson_iter_find_descendant(&fields, "drumTrack", &fieldsChild) && BSON_ITER_HOLDS_UTF8(&fieldsChild)) {
            ui->drumTrackComboBox->setCurrentText(bson_iter_utf8(&fieldsChild, nullptr));
        }
        if (bson_iter_find_descendant(&fields, "trainingLevel", &fieldsChild) && BSON_ITER_HOLDS_INT32(&fieldsChild)) {
            ui->trainingLevelSlider->setValue(bson_iter_int32(&fieldsChild));
        }
    }
}

torch::Tensor SongMakerDialog::moved_tensor(const torch::Tensor& t)
{
    switch (_backend)
    {
    case CPU:
        return t.cpu();
    case Vulkan:
        return t.vulkan();
    case CUDA:
        return t.cuda();
    default:
        return t;
    }
}

void SongMakerDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void SongMakerDialog::dropEvent(QDropEvent *event) {
    QByteArray data = event->mimeData()->data("text/uri-list");
    QStringList paths = QString(data).split("\r\n");
    QString path = paths.first().mid(QString("file://").length()).replace("%20", " ");
#ifdef WIN32
    path = path.startsWith('/') ? path.mid(1) : path.insert(1, ':');
#endif

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "mdl") {
        load(path);
    }
}

void SongMakerDialog::random()
{
    ui->octavesComboBox->setCurrentText(ui->octavesComboBox->itemText((torch::rand(1).item().toFloat() * ui->octavesComboBox->count())));
    ui->progressionSetComboBox->setCurrentText(ui->progressionSetComboBox->itemText((torch::rand(1).item().toFloat() * ui->progressionSetComboBox->count())));
    int major = torch::rand(1).item().toFloat() *  2;
    if (major) {
        ui->majorRadioButton->setChecked(true);
    } else {
        ui->minorRadioButton->setChecked(true);
    }
    ui->drumTrackComboBox->setCurrentText(ui->drumTrackComboBox->itemText((torch::rand(1).item().toFloat() * ui->drumTrackComboBox->count())));

    ui->retrainButton->setChecked(true);
    ui->okPushButton->click();
}

void SongMakerDialog::openModels()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound Studio Project Models (*.mdl)", nullptr, QFileDialog::DontUseNativeDialog);

    load(path);
}

void SongMakerDialog::saveModels()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Chromasound Studio Project Models (*.mdl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        bson_writer_t* writer;
        uint8_t* buf = NULL;
        size_t buflen = 0;
        bson_t* doc;

        writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

        bson_writer_begin(writer, &doc);
        models_to_bson(doc);
        bson_writer_end(writer);
        bson_writer_destroy(writer);

        QByteArray ret(reinterpret_cast<const char*>(buf), buflen);
        bson_free(buf);

        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(ret);
        file.close();

        ui->statusbar->showMessage(QString("Saved %1.").arg(QFileInfo(path).fileName()));
    }
}

void SongMakerDialog::clearModels()
{
    _chordsModel = {};
    _phrasesModel = {};
    _durationsModel = {};
}

void SongMakerDialog::backendButtonClicked()
{
    int index = (_backends.indexOf(_backend) + 1) % _backends.size();

    _backend = _backends.at(index);
    ui->backendButton->setIcon(_backendIcons.at(index));
}

void SongMakerDialog::lossUpdated(std::vector<at::Tensor> losses)
{
    std::vector<QLCDNumber*> lossWidgets = {
        ui->loss1LcdNumber,
        ui->loss2LcdNumber,
        ui->loss3LcdNumber,
        ui->loss4LcdNumber,
        ui->loss5LcdNumber
    };
    if (torch::stack(losses, 0).sum(0).item().toInt() == 0) {
        for(int i = 0; i < 5; i++) {
            lossWidgets[i]->display("-----");
        }
    } else {
        for (int i = 0; i < qMin(5, (int)losses.size()); i++) {
            lossWidgets[i]->display(losses[i].item().toFloat());
        }
        for(int i = losses.size(); i < 5; i++) {
            lossWidgets[i]->display("-----");
        }
    }
}

std::vector<torch::Tensor> ChordGenerationWorker::generate_chords(const Model &model, const int bars)
{
    int lowest_root = 100;
    int highest_root = -1;
    std::vector<torch::Tensor> out;
    torch::Tensor context = torch::zeros(Chord::ChordDuration+1).unsqueeze(0);
    for (int i = 0; i < CONTEXT_LENGTH-1; i++) {
        context = torch::cat({context, torch::zeros(Chord::ChordDuration+1).unsqueeze(0)}, 0);
    }
    auto sum = [&out]() {
        return (out.empty()
         ? 0
         : ((out.size() == 1)
                ? out[0][Chord::ChordDuration].item().toInt()
                : torch::stack(out).sum(0)[Chord::ChordDuration].item().toInt()));
    };
    int _sum;
    while ((_sum = sum()) < bars * 1920) {
        updateProgress((int)(((float)_sum/((float)bars * 1920)) * 100));
        // Generate a message
        torch::Tensor msg = torch::zeros(Chord::ChordDuration+1);
        auto probs = forward(model, context.unsqueeze(0));
        for (int i = 0; i < Chord::ChordDuration+1; i++) {
            msg[i] = torch::multinomial(probs[i], 1).item().toInt();
            if (i == Chord::Root) {
                qDebug() << msg[i].item().toInt();

                if (msg[i].item().toInt() < lowest_root) {
                    lowest_root = msg[i].item().toInt();
                } else if (msg[i].item().toInt() > highest_root) {
                    highest_root = msg[i].item().toInt();
                }
            }
        }


        msg[Chord::ChordDuration] = ((torch::rand({1}) * 2).item().toInt() * 2 + 2) * 480;
        context = torch::cat({context.slice(0, 1), torch::unsqueeze(msg, 0)});
        msg[Chord::Root] += 36;
        out.push_back(msg);
    }

    std::vector<int> deltas;
    for (torch::Tensor& msg : out) {
        deltas.push_back(msg[Chord::ChordDuration].item().toInt());
    }
    int length = torch::tensor(deltas).sum().item().toInt();
    int rem = length % 1920;

    (*(--out.end()))[Chord::ChordDuration] -= rem;

    updateProgress(100);

    return out;
}

torch::Tensor PhraseGenerationWorker::generate_phrases(const Model &model, const int bars)
{
    torch::Tensor out;
    torch::Tensor msg;

    int diff;

    auto sum = [&out]() {
        return ((out.size(0) == 0)
                ? 0
                : ((out.size(0) == 1)
                       ? out[0][Phrase::PhraseDuration].item().toInt()
                       : out.sum(0)[Phrase::PhraseDuration].item().toInt()));
    };

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::zeros(Phrase::HistDir+1));
        }
        context = torch::stack(elems, 0);
    };

    clear_context();

    while (sum() < bars * 1920) {
        msg = torch::zeros(Phrase::HistDir+1);
        std::vector<torch::Tensor> probs = forward(model, context.unsqueeze(0));
        for (int i = 0; i < 4; i++) {
            int ix = torch::multinomial(probs[i], 1).item().toInt();
            if (i == Phrase::Direction || i == Phrase::HistDir) {
                ix -= 1;
            }

            msg[i] = ix;
        }
        diff = (out.size(0) < PHRASE_HISTORY) ? 0 : (msg[Phrase::FirstNote]  - out[-PHRASE_HISTORY][Phrase::FirstNote]).item().toInt();
        msg[Phrase::HistDir] = (diff == 0) ? 0 : (diff / abs(diff));
        context = torch::cat({context.slice(0, 1), torch::unsqueeze(msg, 0)}, 0);

        if (out.size(0) == 0) {
            out = torch::unsqueeze(msg, 0);
        } else {
            out = torch::cat({out, torch::unsqueeze(msg, 0)});
        }
    }

    int rem = sum() % 1920;
    out[-1][Phrase::PhraseDuration] -= rem;

    return out;
}

torch::Tensor TimeGenerationWorker::generate_times(const Model &model, const int bars)
{
    torch::Tensor out;

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::unsqueeze(torch::tensor(0), 0));
        }
        context = torch::stack(elems);
    };
    clear_context();


    auto sum = [&out]() {
        return ((out.size(0) == 0)
                ? 0
                : ((out.size(0) == 1)
                       ? out[0].item().toInt()
                       : out.sum(0).item().toInt()));
    };

    while (sum() < bars * 1920) {
        std::vector<torch::Tensor> probs = forward(model, context.unsqueeze(0));
        torch::Tensor pprobs = probs[0];
        int ix = qMax(120, ((torch::multinomial(pprobs, 1) / 120).item().toInt() * 120));

        context = torch::cat({context.slice(0, 1), torch::unsqueeze(torch::unsqueeze(torch::tensor(ix), 0), 0)}, 0);

        if (out.size(0) == 0) {
            out = torch::unsqueeze(torch::tensor(ix), 0);
        } else {
            out = torch::cat({out, torch::unsqueeze(torch::tensor(ix), 0)});
        }
    }

    int rem = sum() % 1920;
    out[-1] -= rem;

    return out;
}

at::Tensor NoteGenerationWorker::generate_notes(const std::vector<std::pair<torch::Tensor, torch::Tensor>> &chords_phrases, const at::Tensor &times)
{
    torch::Tensor times_copy = times.clone();
    torch::Tensor notes;
    torch::Tensor melody_notes;

    int chord_time = 0;

    std::vector<int> last_names = {-1, -1, -1};
    int name = chords_phrases[0].first[Chord::ChordDuration].item().toInt();
    for (const auto& [chord, phrases] : chords_phrases) {
        chord_time = 0;

        std::vector<torch::Tensor> chord_notes;
        chord_notes.push_back(chord[Chord::Root]);
        int add = (chord[Chord::MajorMinor].item().toBool() ? 4 : 3);
        chord_notes.push_back(chord[Chord::Root] + add);
        chord_notes.push_back(chord[Chord::Root] + add + (7 - add));
        std::vector<torch::Tensor> chord_notes_inverted;
        for (int i = chord[Chord::Inversion].item().toInt(); i < 3; i++) {
            chord_notes_inverted.push_back(chord_notes[i]);
        }
        for (int i = 0; i < chord[Chord::Inversion].item().toInt(); i++) {
            torch::Tensor note = chord_notes[i] + 12;

            chord_notes_inverted.push_back(note);
        }
        chord_notes_inverted.push_back(chord_notes_inverted[0] + 12);

        while (chord_time < chord[Chord::ChordDuration].item().toInt()) {
            // qDebug() << "chord_time = " << chord_time << ", chord_duration = " << chord[Chord::ChordDuration].item().toInt();
            int p = 0;
            for (p = 0; p < phrases.size(0); p++) {
                torch::Tensor phrase = phrases[p];
                int phrase_count = 0;
                int direction = phrase[Phrase::Direction].item().toInt();
                // qDebug() << "phrase_count = " << phrase_count << ", phrase[Phrase::Length] = " << phrase[Phrase::Length].item().toInt();
                while (phrase_count < phrase[Phrase::Length].item().toInt()) {
                    if (direction == 0) {
                        direction = std::vector<int>{-1, 1}[(torch::rand(1).item().toInt() * 2)];
                    }
                    name += direction;
                    torch::Tensor _notes = torch::stack(chord_notes_inverted, 0);
                    while (!torch::eq(_notes, name).any().item().toBool() || name == 0 || std::find(last_names.begin(), last_names.end(), name) != last_names.end()) {
                        if (direction == 0) {
                            direction = std::vector<int>{-1, 1}[(torch::rand(1).item().toInt() * 2)];
                        }
                        if (name > _notes.max().item().toInt()) {
                            direction = -1;
                        }
                        if (name < _notes.min().item().toInt()) {
                            direction = 1;
                        }
                        name += direction;
                    }
                    if ((chord_time + times_copy[0].item().toInt()) > chord[Chord::ChordDuration].item().toInt()) {
                        int rem = ((chord_time + times_copy[0]) - chord[Chord::ChordDuration]).item().toInt();
                        if (times_copy.size(0) == 0) {
                            times_copy = torch::unsqueeze(torch::tensor(rem), 0);
                        } else {
                            if (times_copy[0].item().toInt() == rem) {
                                // exactly twice the length of the distance to the end of the chord
                                // keep times_copy[0] as is
                            } else {
                                times_copy[0] -= rem;
                                if (times_copy.size(0) > 1) {
                                    times_copy[1] += rem;
                                }
                            }
                        }
                    }
                    last_names = std::vector<int>(last_names.begin() + 1, last_names.end());
                    last_names.push_back(name);
                    int _name = name;
                    if (melody_notes.size(0) > 0) {
                        while (torch::abs(name - melody_notes[melody_notes.size(0)-1][0]).item().toInt() > 6) {
                            if (_name > melody_notes[melody_notes.size(0)-1][0].item().toInt()) {
                                name -= 12;
                            } else {
                                name += 12;
                            }
                        }
                    }
                    torch::Tensor note = torch::tensor({name, times_copy[0].item().toInt()});
                    chord_time += times_copy[0].item().toInt();
                    // qDebug() << "!!" << chord[Chord::ChordDuration].item().toInt() << ", times_copy[0] = " << times_copy[0].item().toInt();

                    times_copy = times_copy.slice(0, 1);

                    if (melody_notes.size(0) == 0) {
                        melody_notes = torch::unsqueeze(note, 0);
                    } else {
                        melody_notes = torch::cat({melody_notes, torch::unsqueeze(note, 0)});
                    }

                    phrase_count++;

                    if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                        break;
                    }
                }

                if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                    break;
                }
            }

            if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                // qDebug() << "breaking because chord_time = " << chord_time;
                break;
            }
        }
    }

    return melody_notes;
}

#include "songmakerdialog.h"
#include "tools/songmaker/ui_songmakerdialog.h"
#include "ui_songmakerdialog.h"

#include "mainwindow.h"

SongMakerDialog::SongMakerDialog(QWidget *parent)
    : QMainWindow(parent)
    , _mainWindow(dynamic_cast<MainWindow*>(parent))
    , _backend(MusicBrain::Backend::CPU)
    , ui(new Ui::SongMakerDialog)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    connect(ui->actionOpen, &QAction::triggered, this, &SongMakerDialog::openModels);
    connect(ui->actionSave, &QAction::triggered, this, &SongMakerDialog::saveModels);
    connect(ui->actionRandom, &QAction::triggered, this,  &SongMakerDialog::random);
    ui->actionRandom->setShortcut({QKeySequence("Alt+R")});
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
    ui->menubar->setNativeMenuBar(false);

    connect(ui->octavesComboBox, &QComboBox::currentTextChanged, this, &SongMakerDialog::chordsSelectionChanged);

    connect(ui->actionCPU, &QAction::triggered, this, &SongMakerDialog::cpuSelected);
    connect(ui->actionCUDA, &QAction::triggered, this, &SongMakerDialog::cudaSelected);
    connect(ui->actionMetal, &QAction::triggered, this, &SongMakerDialog::metalSelected);

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

    if (!torch::cuda::is_available()) {
        ui->menuBackend->removeAction(ui->actionCUDA);
    }
    if (!torch::mps::is_available())
    {
        ui->menuBackend->removeAction(ui->actionMetal);
    }
    ui->actionCPU->trigger();

    connect(ui->octavesComboBox, &QComboBox::currentTextChanged, this, &SongMakerDialog::requireRetrain);
    connect(ui->progressionSetComboBox, &QComboBox::currentTextChanged, this, &SongMakerDialog::requireRetrain);
    connect(ui->majorRadioButton, &QRadioButton::toggled, this, &SongMakerDialog::requireRetrain);
    connect(ui->minorRadioButton, &QRadioButton::toggled, this, &SongMakerDialog::requireRetrain);
    connect(ui->melodySetComboBox, &QComboBox::currentTextChanged, this, &SongMakerDialog::requireRetrain);
    connect(ui->trainingLevelSlider, &QSlider::valueChanged, this, &SongMakerDialog::requireRetrain);
}

SongMakerDialog::~SongMakerDialog()
{
    delete ui;
}

void SongMakerDialog::chordsSelectionChanged(const QString& chords)
{
    ui->progressionSetComboBox->clear();

    QDirListing octavesDirList(":/unison/progressions/", {QString("*%1").arg(chords)});

    QString progressionsDirPath = octavesDirList.begin()->filePath();

    QDirListing progressionsDirList(progressionsDirPath);
    for (const auto &entry : progressionsDirList) {
        QString name = entry.fileName().at(0).toUpper() + entry.fileName().mid(1);
        ui->progressionSetComboBox->addItem(name);
    }
}

void SongMakerDialog::requireRetrain()
{
    ui->retrainCheckBox->setChecked(true);
    ui->retrainCheckBox->setDisabled(true);
}

void SongMakerDialog::okButtonClicked()
{
    ui->taskProgressBar->setValue(0);
    ui->totalProgressBar->setValue(0);

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

    auto lamb = [this](MusicBrain::Model chordsModel, MusicBrain::Model phrasesModel, MusicBrain::Model timesModel) {
        auto generatingWorkerChords = new MusicBrain::ChordGenerationWorker(_backend);
        connect(generatingWorkerChords, &MusicBrain::ChordGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
        connect(generatingWorkerChords, &MusicBrain::ChordGenerationWorker::finished, this, [this, generatingWorkerChords, phrasesModel, timesModel](std::vector<torch::Tensor> chords){
            delete generatingWorkerChords;

            QList<Track::Item*> items;
            int time = 0;
            torch::Tensor negOne = torch::tensor({-1});
            std::vector<torch::Tensor> last_cni = torch::unbind(negOne, 0);
            for (const torch::Tensor chord : chords) {
                std::vector<torch::Tensor> chord_notes;
                chord_notes.push_back(chord[MusicBrain::Chord::Root]);
                int add = (chord[MusicBrain::Chord::MajorMinor].item().toBool() ? 4 : 3);
                chord_notes.push_back(chord[MusicBrain::Chord::Root] + add);
                chord_notes.push_back(chord[MusicBrain::Chord::Root] + add + (7 - add));
                std::vector<torch::Tensor> chord_notes_inverted;
                for (int i = chord[MusicBrain::Chord::Inversion].item().toInt(); i < 3; i++) {
                    chord_notes_inverted.push_back(chord_notes[i]);
                }
                for (int i = 0; i < chord[MusicBrain::Chord::Inversion].item().toInt(); i++) {
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
                    Track::Item* item = new Track::Item(time, Note(chord_notes_inverted[i].item().toInt(), chord[MusicBrain::Chord::ChordDuration].item().toFloat() / 480));
                    items.append(item);
                }
                time += chord[MusicBrain::Chord::ChordDuration].item().toFloat() / 480;

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

            auto generatingWorkerPhrases = new MusicBrain::PhraseGenerationWorker(_backend);
            connect(generatingWorkerPhrases, &MusicBrain::PhraseGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
            connect(generatingWorkerPhrases, &MusicBrain::PhraseGenerationWorker::finished, this, [this, generatingWorkerPhrases, chords, phrasesModel, timesModel](torch::Tensor phrases){
                delete generatingWorkerPhrases;
                auto generatingWorkerTimes = new MusicBrain::TimeGenerationWorker(_backend);

                connect(generatingWorkerTimes, &MusicBrain::TimeGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                connect(generatingWorkerTimes, &MusicBrain::TimeGenerationWorker::finished, this, [this, generatingWorkerTimes, chords, phrasesModel, phrases](torch::Tensor times) {
                    delete generatingWorkerTimes;
                    auto chords_phrases = phrases_by_chord(chords, phrases);
                    auto generatingWorkerNotes = new MusicBrain::NoteGenerationWorker(_backend);
                    connect(generatingWorkerNotes, &MusicBrain::NoteGenerationWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                    connect(generatingWorkerNotes, &MusicBrain::NoteGenerationWorker::finished, this, [this, generatingWorkerNotes, chords, phrasesModel](torch::Tensor melody) {
                        delete generatingWorkerNotes;
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
                    ui->statusbar->showMessage("Generating melody...");
                    _generatingNotesArgs = {
                        chords_phrases,
                        times
                    };
                    generatingWorkerNotes->doGenerateNotes(_generatingNotesArgs);
                });
                ui->totalProgressBar->setValue(70);
                ui->statusbar->showMessage("Generating durations...");
                _generatingArgs = {
                    &_durationsModel,
                    ui->barsSpinBox->value()
                };
                generatingWorkerTimes->doGenerateTimes(_generatingArgs);
            });
            ui->totalProgressBar->setValue(56);
            ui->statusbar->showMessage("Generating phrases...");
            _generatingArgs = {
                &_phrasesModel,
                ui->barsSpinBox->value()
            };
            generatingWorkerPhrases->doGeneratePhrases(_generatingArgs);
        });
        ui->totalProgressBar->setValue(42);
        ui->statusbar->showMessage("Generating chords...");
        _generatingArgs = {
            &_chordsModel,
            ui->barsSpinBox->value()
        };
        generatingWorkerChords->doGenerateChords(_generatingArgs);
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
            if (ui->retrainCheckBox->isChecked()) {
                clearModels();
            }

            if (std::get<0>(_chordsModel).empty()) {
                auto XY = build_chords_dataset(folderInfo.filePath().toStdString());

                auto trainingWorkerChords = new MusicBrain::TrainingWorker(_backend);
                connect(trainingWorkerChords, &MusicBrain::TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                connect(trainingWorkerChords, &MusicBrain::TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                connect(trainingWorkerChords, &MusicBrain::TrainingWorker::finished, this, [this, lamb, trainingWorkerChords, trainingIters](MusicBrain::Model chordsModel){
                    _chordsModel = chordsModel;
                    delete trainingWorkerChords;
                    QString melodiesPath = QFile(ui->melodySetComboBox->currentText()).exists()
                                               ? ui->melodySetComboBox->currentText()
                                               : _melodySetsPaths[ui->melodySetComboBox->currentText()];
                    QFile melodiesFolder = QFile(melodiesPath);
                    QFileInfo melodiesFolderInfo = QFileInfo(melodiesFolder);
                    if (melodiesFolderInfo.isDir()) {
                        auto XY = build_phrases_dataset(melodiesFolderInfo.filePath().toStdString());
                        auto trainingWorkerPhrases = new MusicBrain::TrainingWorker(_backend);
                        connect(trainingWorkerPhrases, &MusicBrain::TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                        connect(trainingWorkerPhrases, &MusicBrain::TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                        connect(trainingWorkerPhrases, &MusicBrain::TrainingWorker::finished, this, [this, lamb, chordsModel, trainingWorkerPhrases, melodiesFolderInfo, trainingIters](MusicBrain::Model phrasesModel) {
                            _phrasesModel = phrasesModel;
                            auto XY = build_times_dataset(melodiesFolderInfo.filePath().toStdString());
                            auto trainingWorkerTimes = new MusicBrain::TrainingWorker(_backend);
                            connect(trainingWorkerTimes, &MusicBrain::TrainingWorker::progressUpdated, ui->taskProgressBar, &QProgressBar::setValue);
                            connect(trainingWorkerTimes, &MusicBrain::TrainingWorker::lossUpdate, this, &SongMakerDialog::lossUpdated);
                            connect(trainingWorkerTimes, &MusicBrain::TrainingWorker::finished, this, [this, lamb, chordsModel, trainingWorkerPhrases, melodiesFolderInfo, phrasesModel](MusicBrain::Model timesModel) {
                                _durationsModel = timesModel;
                                lamb(chordsModel, phrasesModel, timesModel);
                            });

                            ui->totalProgressBar->setValue(28);
                            ui->statusbar->showMessage("Training note durations...");
                            _trainingArgs = {
                                torch::tensor({721}),
                                torch::tensor(ui->durationsModelWidget->layerSizes()),
                                trainingIters,
                                XY.first,
                                XY.second
                            };
                            trainingWorkerTimes->doTrain(_trainingArgs);
                        });
                        ui->totalProgressBar->setValue(14);
                        ui->statusbar->showMessage("Training phrases...");
                        _trainingArgs = {
                            torch::tensor({3, 8, 961, 88, 3}),
                            torch::tensor(ui->phrasesModelWidget->layerSizes()),
                            trainingIters,
                            XY.first,
                            XY.second
                        };
                        trainingWorkerPhrases->doTrain(_trainingArgs);
                    }
                });

                ui->statusbar->showMessage("Training chords...");
                _trainingArgs = {
                    torch::tensor({12, 2, 4, 1921}),
                    torch::tensor(ui->chordsModelWidget->layerSizes()),
                    trainingIters,
                    XY.first,
                    XY.second
                };
                trainingWorkerChords->doTrain(_trainingArgs);
            } else {
                lamb(_chordsModel, _phrasesModel, _durationsModel);
            }

        }
    }
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
        torch::Tensor new_chord = torch::tensor({fixed_chord[0].item().toInt(), (int)major, inversion, chord[MusicBrain::Chord::ChordDuration].item().toInt()});
        new_chords.push_back(new_chord);
    }

    std::vector<torch::Tensor> elems;
    for (int i = 0; i < CONTEXT_LENGTH; i++) {
        elems.push_back(torch::zeros(MusicBrain::Chord::ChordDuration+1));
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
            elems.push_back(torch::zeros(MusicBrain::Phrase::HistDir+1));
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

        torch::Tensor phrase = torch::zeros(MusicBrain::Phrase::HistDir+1);
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
                            phrase[MusicBrain::Phrase::Direction] = direction;
                            phrase[MusicBrain::Phrase::Length] = count;
                            phrase[MusicBrain::Phrase::PhraseDuration] = duration;
                            phrase[MusicBrain::Phrase::FirstNote] = first_note;

                            diff = (melody.size(0) < 2) ? 0 : (first_note - melody[-PHRASE_HISTORY][MusicBrain::Phrase::FirstNote].item().toInt());
                            phrase[MusicBrain::Phrase::HistDir] = (diff == 0) ? 0 : (diff / abs(diff));

                            if (melody.size(0) == 0) {
                                melody = torch::unsqueeze(phrase, 0);
                            } else {
                                melody = torch::cat({melody, torch::unsqueeze(phrase, 0)});
                            }

                            count = -1;
                            duration = -1;
                            direction = -2;
                            first_note = -1;
                            phrase = torch::zeros(MusicBrain::Phrase::HistDir+1);
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
            ph[MusicBrain::Phrase::PhraseDuration] += rem;
            if (chord_phrases.size(0) == 0) {
                chord_phrases = torch::unsqueeze(ph, 0);
            } else {
                chord_phrases = torch::cat({chord_phrases, torch::unsqueeze(ph, 0)});
            }
            chord_time += ph[MusicBrain::Phrase::PhraseDuration].item().toInt();

            if (chord_time >= chord[MusicBrain::Chord::ChordDuration].item().toInt()) {
                rem = chord_time - chord[MusicBrain::Chord::ChordDuration].item().toInt();
                chord_phrases[-1][MusicBrain::Phrase::PhraseDuration] -= rem;

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

    ui->actionCPU->setDisabled(true);
    ui->actionCUDA->setDisabled(true);
    ui->actionMetal->setDisabled(true);

    ui->octavesComboBox->setDisabled(true);
    ui->progressionSetComboBox->setDisabled(true);
    ui->majorRadioButton->setDisabled(true);
    ui->minorRadioButton->setDisabled(true);
    ui->melodySetComboBox->setDisabled(true);
    ui->drumTrackComboBox->setDisabled(true);
    ui->barsSpinBox->setDisabled(true);
    ui->trainingLevelSlider->setDisabled(true);
    ui->retrainCheckBox->setDisabled(true);
    ui->okPushButton->setDisabled(true);
}

void SongMakerDialog::enable_fields()
{
    ui->actionOpen->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionRandom->setEnabled(true);
    ui->actionClose->setEnabled(true);

    ui->actionCPU->setEnabled(true);
    ui->actionCUDA->setEnabled(true);
    ui->actionMetal->setEnabled(true);

    ui->octavesComboBox->setEnabled(true);
    ui->progressionSetComboBox->setEnabled(true);
    ui->majorRadioButton->setEnabled(true);
    ui->minorRadioButton->setEnabled(true);
    ui->melodySetComboBox->setEnabled(true);
    ui->drumTrackComboBox->setEnabled(true);
    ui->barsSpinBox->setEnabled(true);
    ui->trainingLevelSlider->setEnabled(true);
    ui->retrainCheckBox->setEnabled(true);
    ui->retrainCheckBox->setChecked(false);
    ui->okPushButton->setEnabled(true);
}

void SongMakerDialog::models_to_bson(bson_t* dst)
{
    bson_t b_fields;

    bson_t b_chords;
    bson_init(&b_chords);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "chords", &b_chords);
    BSON::fromModel(&b_chords, _chordsModel.cpu());
    bson_append_document_end(dst, &b_chords);

    bson_t b_phrases;
    bson_init(&b_phrases);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "phrases", &b_phrases);
    BSON::fromModel(&b_phrases, _phrasesModel.cpu());
    bson_append_document_end(dst, &b_phrases);

    bson_t b_durations;
    bson_init(&b_durations);
    BSON_APPEND_DOCUMENT_BEGIN(dst, "durations", &b_durations);
    BSON::fromModel(&b_durations, _durationsModel.cpu());
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

void SongMakerDialog::models_from_bson(const bson_t& merged_bson)
{
    bson_iter_t merged_bson_inner;
    bson_iter_t child;
    bson_iter_t child_inner;
    bson_iter_init(&merged_bson_inner, &merged_bson);

    if (bson_iter_find_descendant(&merged_bson_inner, "chords", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _chordsModel = BSON::toModel(child_inner);

        if (_backend == MusicBrain::Backend::CUDA)
        {
            _chordsModel = _chordsModel.cuda();
        } else if (_backend == MusicBrain::Backend::MPS)
        {
            _chordsModel = _chordsModel.mps();
        }

        ui->chordsModelWidget->setLayerSizes(std::get<7>(_chordsModel)[0].unbind(0));
    }

    if (bson_iter_find_descendant(&merged_bson_inner, "phrases", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _phrasesModel = BSON::toModel(child_inner);
        if (_backend == MusicBrain::Backend::CUDA)
        {
            _phrasesModel = _phrasesModel.cuda();
        } else if (_backend == MusicBrain::Backend::MPS)
        {
            _phrasesModel = _phrasesModel.mps();
        }

        ui->phrasesModelWidget->setLayerSizes(std::get<7>(_phrasesModel)[0].unbind(0));
    }

    if (bson_iter_find_descendant(&merged_bson_inner, "durations", &child) && BSON_ITER_HOLDS_DOCUMENT(&child) && bson_iter_recurse(&child, &child_inner)) {
        _durationsModel = BSON::toModel(child_inner);
        if (_backend == MusicBrain::Backend::CUDA)
        {
            _durationsModel = _durationsModel.cuda();
        } else if (_backend == MusicBrain::Backend::MPS)
        {
            _durationsModel = _durationsModel.mps();
        }

        ui->durationsModelWidget->setLayerSizes(std::get<7>(_durationsModel)[0].unbind(0));
    }

    const QSignalBlocker blocker1(ui->octavesComboBox);
    const QSignalBlocker blocker2(ui->progressionSetComboBox);
    const QSignalBlocker blocker3(ui->majorRadioButton);
    const QSignalBlocker blocker4(ui->minorRadioButton);
    const QSignalBlocker blocker5(ui->melodySetComboBox);
    const QSignalBlocker blocker6(ui->trainingLevelSlider);

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

void SongMakerDialog::cpuSelected()
{
    _backend = MusicBrain::Backend::CPU;

    _chordsModel = _chordsModel.cpu();
    _phrasesModel = _phrasesModel.cpu();
    _durationsModel = _durationsModel.cpu();

    ui->actionCUDA->setChecked(false);
    ui->actionMetal->setChecked(false);
}

void SongMakerDialog::cudaSelected()
{
    _backend = MusicBrain::Backend::CUDA;

    _chordsModel = _chordsModel.cuda();
    _phrasesModel = _phrasesModel.cuda();
    _durationsModel = _durationsModel.cuda();

    ui->actionCPU->setChecked(false);
    ui->actionMetal->setChecked(false);
}

void SongMakerDialog::metalSelected()
{
    _backend = MusicBrain::Backend::MPS;

    _chordsModel = _chordsModel.mps();
    _phrasesModel = _phrasesModel.mps();
    _durationsModel = _durationsModel.mps();

    ui->actionCPU->setChecked(false);
    ui->actionCUDA->setChecked(false);
}

void SongMakerDialog::lossUpdated(std::vector<at::Tensor> losses)
{
    std::vector<torch::Tensor> _losses;
    for (const torch::Tensor& t : losses) {
        _losses.push_back(t.cpu());
    }

    std::vector<QLCDNumber*> lossWidgets = {
        ui->loss1LcdNumber,
        ui->loss2LcdNumber,
        ui->loss3LcdNumber,
        ui->loss4LcdNumber,
        ui->loss5LcdNumber
    };
    if (torch::stack(_losses, 0).sum(0).item().toInt() == 0) {
        for(int i = 0; i < 5; i++) {
            lossWidgets[i]->display("-----");
        }
    } else {
        for (int i = 0; i < qMin(5, (int)_losses.size()); i++) {
            lossWidgets[i]->display(_losses[i].item().toFloat());
        }
        for(int i = _losses.size(); i < 5; i++) {
            lossWidgets[i]->display("-----");
        }
    }
}

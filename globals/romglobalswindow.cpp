#include "romglobalswindow.h"
#include "ui_romglobalswindow.h"

ROMGlobalsWindow::ROMGlobalsWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMGlobalsWindow)
    , _app(app)
{
    ui->setupUi(this);

    ui->romGlobalsWidget->setApplication(app);

    ui->romGlobalsWidget->setROMFile(app->project().pcmFile());

    connect(ui->romGlobalsWidget, &ROMGlobalsWidget::updated, this, &ROMGlobalsWindow::romUpdated);

    connect(ui->actionOpen, &QAction::triggered, this, &ROMGlobalsWindow::open);
    connect(ui->actionReset, &QAction::triggered, this, &ROMGlobalsWindow::reset);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    connect(ui->optimizePCMButton, &QPushButton::clicked, this, &ROMGlobalsWindow::optimizePCM);

    ui->menubar->setNativeMenuBar(false);
}

ROMGlobalsWindow::~ROMGlobalsWindow()
{
    delete ui;
}

void ROMGlobalsWindow::doUpdate()
{
    ui->romGlobalsWidget->blockSignals(true);

    QString pcmFilePath = _app->project().pcmFile();

    ui->romGlobalsWidget->setROMFile(pcmFilePath);

    ui->optimizePCMButton->setVisible(!pcmFilePath.isNull());

    ui->romGlobalsWidget->blockSignals(false);
}

void ROMGlobalsWindow::romUpdated()
{
    _app->undoStack().push(new SetProjectPCMFileCommand(_app->window(), _app->project(), ui->romGlobalsWidget->romFile()));
}

void ROMGlobalsWindow::open()
{
    ui->romGlobalsWidget->open();
}

void ROMGlobalsWindow::reset()
{
    ui->romGlobalsWidget->reset();
}

void ROMGlobalsWindow::optimizePCM()
{
    QMessageBox::information(this, "Save the ROM", "In the next dialog, please choose a save location for the optimized ROM.");

    QString newPath = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Chromasound ROM (*.rom)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!newPath.isNull()) {
        QSet<int> usedSamples;
        for (int i = 0; i < _app->project().channelCount(); i++) {
            const Channel* channel = &_app->project().channels()[i];
            if (channel->type() == Channel::Type::PCM) {
                const PCMChannelSettings* settings = dynamic_cast<const PCMChannelSettings*>(&channel->settings());
                for (const Pattern* pattern : _app->project().patterns()) {
                    if (pattern->hasTrack(i)) {
                        Track* track = pattern->tracks()[i];
                        for (const Track::Item* item : track->items()) {
                            auto it = std::find_if(track->settingsChanges().rbegin(), track->settingsChanges().rend(), [&](const Track::SettingsChange* change){ return change->time() <= item->time();});
                            if (it == track->settingsChanges().rend()) {
                                if (settings->keySampleMappings().contains(item->note().key())) {
                                    usedSamples.insert(settings->keySampleMappings()[item->note().key()]);
                                }
                            } else {
                                const ROMChannelSettings* changeSettings = dynamic_cast<const ROMChannelSettings*>(*it);
                                if (changeSettings->keySampleMappings().contains(item->note().key())) {
                                    usedSamples.insert(changeSettings->keySampleMappings()[item->note().key()]);
                                }
                            }

                        }
                    }
                }
            }
        }

        ROM pcm(_app->project().pcmFile());

        QList<int> samplesToRemove;
        for (int i = 0; i < pcm.names().size(); i++) {
            samplesToRemove.append(i);
        }
        samplesToRemove.removeIf([&](int a){ return usedSamples.contains(a); });

        std::sort(samplesToRemove.begin(), samplesToRemove.end(), [](int a, int b){ return b < a; });

        for (int i = 0; i < samplesToRemove.size(); i++) {
            pcm.remove(samplesToRemove[i]);
        }

        pcm.save(newPath);
        _app->undoStack().push(new SetProjectPCMFileCommand(_app->window(), _app->project(), newPath));

        for (int i = 0; i < _app->project().channelCount(); i++) {
            Channel* channel = &_app->project().getChannel(i);
            if (channel->type() == Channel::Type::PCM) {
                PCMChannelSettings* origSettings = dynamic_cast<PCMChannelSettings*>(&channel->settings());
                PCMChannelSettings settings = *origSettings;

                for (auto it = origSettings->keySampleMappings().begin(); it != origSettings->keySampleMappings().end(); ++it) {
                    if (samplesToRemove.contains(it.value())) {
                        settings.keySampleMappings().remove(it.key());
                    }
                }

                for (const int& sample : samplesToRemove) {
                    for (auto it = settings.keySampleMappings().begin(); it != settings.keySampleMappings().end(); ++it) {
                        if (it.value() > sample) {
                            it.value()--;
                        }
                    }
                }

                _app->undoStack().push(new SetPCMChannelSettingsCommand(_app->window(), dynamic_cast<PCMChannelSettings&>(channel->settings()), settings, true));

                for (Pattern* pattern : _app->project().patterns()) {
                    if (pattern->hasTrack(i)) {
                        Track* track = pattern->tracks()[i];
                        for (Track::SettingsChange* settingsChange : track->settingsChanges()) {
                            ROMChannelSettings* origSettings = dynamic_cast<ROMChannelSettings*>(&settingsChange->settings());
                            ROMChannelSettings settings = *origSettings;

                            for (auto it = origSettings->keySampleMappings().begin(); it != origSettings->keySampleMappings().end(); ++it) {
                                if (samplesToRemove.contains(it.value())) {
                                    settings.keySampleMappings().remove(it.key());
                                }
                            }

                            for (const int& sample : samplesToRemove) {
                                for (auto it = settings.keySampleMappings().begin(); it != settings.keySampleMappings().end(); ++it) {
                                    if (it.value() > sample) {
                                        it.value()--;
                                    }
                                }
                            }

                            _app->undoStack().push(new SetPCMChannelSettingsCommand(_app->window(), dynamic_cast<ROMChannelSettings&>(settingsChange->settings()), settings, true));
                        }
                    }
                }
            }
        }
    }
}


void ROMGlobalsWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

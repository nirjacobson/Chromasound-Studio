#include "pcmglobalswindow.h"
#include "ui_pcmglobalswindow.h"

PCMGlobalsWindow::PCMGlobalsWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::PCMGlobalsWindow)
    , _app(app)
{
    ui->setupUi(this);

    ui->pcmGlobalsWidget->setApplication(app);

    ui->pcmGlobalsWidget->setROMFile(app->project().pcmFile());

    connect(ui->pcmGlobalsWidget, &PCMGlobalsWidget::updated, this, &PCMGlobalsWindow::romUpdated);

    connect(ui->actionOpen, &QAction::triggered, this, &PCMGlobalsWindow::open);
    connect(ui->actionReset, &QAction::triggered, this, &PCMGlobalsWindow::reset);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    connect(ui->optimizePCMButton, &QPushButton::clicked, this, &PCMGlobalsWindow::optimizePCM);

    ui->menubar->setNativeMenuBar(false);
}

PCMGlobalsWindow::~PCMGlobalsWindow()
{
    delete ui;
}

void PCMGlobalsWindow::doUpdate()
{
    ui->pcmGlobalsWidget->blockSignals(true);

    QString pcmFilePath = _app->project().pcmFile();

    ui->pcmGlobalsWidget->setROMFile(pcmFilePath);

    ui->optimizePCMButton->setVisible(!pcmFilePath.isNull());

    ui->pcmGlobalsWidget->blockSignals(false);
}

void PCMGlobalsWindow::romUpdated()
{
    _app->undoStack().push(new SetProjectPCMFileCommand(_app->window(), _app->project(), ui->pcmGlobalsWidget->romFile()));
}

void PCMGlobalsWindow::open()
{
    ui->pcmGlobalsWidget->open();
}

void PCMGlobalsWindow::reset()
{
    ui->pcmGlobalsWidget->reset();
}

void PCMGlobalsWindow::optimizePCM()
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
                                const PCMChannelSettings* changeSettings = dynamic_cast<const PCMChannelSettings*>(*it);
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
                            PCMChannelSettings* origSettings = dynamic_cast<PCMChannelSettings*>(&settingsChange->settings());
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

                            _app->undoStack().push(new SetPCMChannelSettingsCommand(_app->window(), dynamic_cast<PCMChannelSettings&>(settingsChange->settings()), settings, true));
                        }
                    }
                }
            }
        }
    }
}


void PCMGlobalsWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

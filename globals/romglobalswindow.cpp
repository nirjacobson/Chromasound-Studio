#include "romglobalswindow.h"
#include "ui_romglobalswindow.h"

ROMGlobalsWindow::ROMGlobalsWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMGlobalsWindow)
    , _app(app)
{
    ui->setupUi(this);

    ui->romGlobalsWidgetStatic->setApplication(app);
    ui->romGlobalsWidgetDynamic->setApplication(app);

    ui->romGlobalsWidgetStatic->setROMFile(app->project().resolve(app->project().spcmFile()));
    ui->romGlobalsWidgetDynamic->setROMFile(app->project().resolve(app->project().dpcmFile()));

    connect(ui->romGlobalsWidgetStatic, &ROMGlobalsWidget::updated, this, &ROMGlobalsWindow::staticUpdated);
    connect(ui->romGlobalsWidgetDynamic, &ROMGlobalsWidget::updated, this, &ROMGlobalsWindow::dynamicUpdated);

    connect(ui->actionOpen, &QAction::triggered, this, &ROMGlobalsWindow::open);
    connect(ui->actionReset, &QAction::triggered, this, &ROMGlobalsWindow::reset);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    connect(ui->optimizeSPCMButton, &QPushButton::clicked, this, &ROMGlobalsWindow::optimizeSPCM);
    connect(ui->optimizeDPCMButton, &QPushButton::clicked, this, &ROMGlobalsWindow::optimizeDPCM);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &ROMGlobalsWindow::tabIndexChanged);
    tabIndexChanged(ui->tabWidget->currentIndex());

    ui->menubar->setNativeMenuBar(false);
}

ROMGlobalsWindow::~ROMGlobalsWindow()
{
    delete ui;
}

void ROMGlobalsWindow::doUpdate()
{
    ui->romGlobalsWidgetStatic->blockSignals(true);
    ui->romGlobalsWidgetDynamic->blockSignals(true);

    QString spcmFilePath = _app->project().resolve(_app->project().spcmFile());
    QString dpcmFilePath = _app->project().resolve(_app->project().dpcmFile());

    ui->romGlobalsWidgetStatic->setROMFile(spcmFilePath);
    ui->romGlobalsWidgetDynamic->setROMFile(dpcmFilePath);

    ui->optimizeSPCMButton->setVisible(!spcmFilePath.isNull());
    ui->optimizeDPCMButton->setVisible(!dpcmFilePath.isNull());

    ui->romGlobalsWidgetStatic->blockSignals(false);
    ui->romGlobalsWidgetDynamic->blockSignals(false);
}

void ROMGlobalsWindow::staticUpdated()
{
    _app->undoStack().push(new SetProjectSPCMFileCommand(_app->window(), _app->project(), ui->romGlobalsWidgetStatic->romFile()));
}

void ROMGlobalsWindow::dynamicUpdated()
{
    _app->undoStack().push(new SetProjectDPCMFileCommand(_app->window(), _app->project(), ui->romGlobalsWidgetDynamic->romFile()));
}

void ROMGlobalsWindow::open()
{
    if (ui->tabWidget->currentIndex() == 0) {
        ui->romGlobalsWidgetStatic->open();
    } else {
        ui->romGlobalsWidgetDynamic->open();
    }
}

void ROMGlobalsWindow::reset()
{
    if (ui->tabWidget->currentIndex() == 0) {
        ui->romGlobalsWidgetStatic->reset();
    } else {
        ui->romGlobalsWidgetDynamic->reset();
    }
}

void ROMGlobalsWindow::optimizeDPCM()
{
    QMessageBox::information(this, "Save the ROM", "In the next dialog, please choose a save location for the optimized ROM.");

    QString newPath = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Chromasound ROM (*.rom)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!newPath.isNull()) {
        QSet<int> usedSamples;
        for (int i = 0; i < _app->project().channelCount(); i++) {
            const Channel* channel = &_app->project().channels()[i];
            if (channel->type() == Channel::Type::DPCM) {
                const ROMChannelSettings* settings = dynamic_cast<const ROMChannelSettings*>(&channel->settings());
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

        ROM dpcm(_app->project().dpcmFile());

        QList<int> samplesToRemove;
        for (int i = 0; i < dpcm.names().size(); i++) {
            samplesToRemove.append(i);
        }
        samplesToRemove.removeIf([&](int a){ return usedSamples.contains(a); });

        std::sort(samplesToRemove.begin(), samplesToRemove.end(), [](int a, int b){ return b < a; });

        for (int i = 0; i < samplesToRemove.size(); i++) {
            dpcm.remove(samplesToRemove[i]);
        }

        dpcm.save(newPath);
        _app->undoStack().push(new SetProjectDPCMFileCommand(_app->window(), _app->project(), newPath));

        for (int i = 0; i < _app->project().channelCount(); i++) {
            Channel* channel = &_app->project().getChannel(i);
            if (channel->type() == Channel::Type::DPCM) {
                ROMChannelSettings* origSettings = dynamic_cast<ROMChannelSettings*>(&channel->settings());
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

                _app->undoStack().push(new SetROMChannelSettingsCommand(_app->window(), dynamic_cast<ROMChannelSettings&>(channel->settings()), settings, true));

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

                            _app->undoStack().push(new SetROMChannelSettingsCommand(_app->window(), dynamic_cast<ROMChannelSettings&>(settingsChange->settings()), settings, true));
                        }
                    }
                }
            }
        }
    }
}

void ROMGlobalsWindow::optimizeSPCM()
{
    QMessageBox::information(this, "Save the ROM", "In the next dialog, please choose a save location for the optimized ROM.");

    QString newPath = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Chromasound ROM (*.rom)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!newPath.isNull()) {
        QSet<int> usedSamples;
        for (int i = 0; i < _app->project().channelCount(); i++) {
            const Channel* channel = &_app->project().channels()[i];
            if (channel->type() == Channel::Type::SPCM) {
                const ROMChannelSettings* settings = dynamic_cast<const ROMChannelSettings*>(&channel->settings());
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

        ROM spcm(_app->project().spcmFile());

        QList<int> samplesToRemove;
        for (int i = 0; i < spcm.names().size(); i++) {
            samplesToRemove.append(i);
        }
        samplesToRemove.removeIf([&](int a){ return usedSamples.contains(a); });

        std::sort(samplesToRemove.begin(), samplesToRemove.end(), [](int a, int b){ return b < a; });

        for (int i = 0; i < samplesToRemove.size(); i++) {
            spcm.remove(samplesToRemove[i]);
        }

        spcm.save(newPath);
        _app->undoStack().push(new SetProjectSPCMFileCommand(_app->window(), _app->project(), newPath));

        for (int i = 0; i < _app->project().channelCount(); i++) {
            Channel* channel = &_app->project().getChannel(i);
            if (channel->type() == Channel::Type::SPCM) {
                ROMChannelSettings* origSettings = dynamic_cast<ROMChannelSettings*>(&channel->settings());
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

                _app->undoStack().push(new SetROMChannelSettingsCommand(_app->window(), dynamic_cast<ROMChannelSettings&>(channel->settings()), settings, true));

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

                            _app->undoStack().push(new SetROMChannelSettingsCommand(_app->window(), dynamic_cast<ROMChannelSettings&>(settingsChange->settings()), settings, true));
                        }
                    }
                }
            }
        }
    }
}

void ROMGlobalsWindow::tabIndexChanged(const int index)
{
    ui->optimizeSPCMButton->setVisible(!_app->project().spcmFile().isEmpty());
    ui->optimizeDPCMButton->setVisible(!_app->project().dpcmFile().isEmpty());
}

void ROMGlobalsWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

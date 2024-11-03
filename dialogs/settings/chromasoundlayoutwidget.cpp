#include "chromasoundlayoutwidget.h"
#include "ui_chromasoundlayoutwidget.h"

ChromasoundLayoutWidget::ChromasoundLayoutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChromasoundLayoutWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

#ifdef Q_OS_LINUX
    if (Application::isRaspberryPi()) {
        ui->implementationComboBox->addItem("Chromasound Direct");
        ui->playbackComboBox->addItem("Chromasound Direct");
        ui->interactiveComboBox->addItem("Chromasound Direct");
    }
#endif

    int quantity = settings.value(Chromasound_Studio::NumberOfChromasoundsKey, 1).toInt();

    ui->numberOfComboBox->setCurrentIndex(quantity - 1);
    ui->implementationComboBox->setCurrentIndex(settings.value(Chromasound_Studio::Chromasound1Key, Chromasound_Studio::Emulator) == Chromasound_Studio::Emulator ? 0 : 1);
    ui->playbackComboBox->setCurrentIndex(settings.value(Chromasound_Studio::Chromasound1Key, Chromasound_Studio::Emulator) == Chromasound_Studio::Emulator ? 0 : 1);
    ui->interactiveComboBox->setCurrentIndex(settings.value(Chromasound_Studio::Chromasound2Key, Chromasound_Studio::Emulator) == Chromasound_Studio::Emulator ? 0 : 1);

    connect(ui->numberOfComboBox, &QComboBox::currentIndexChanged, this, &ChromasoundLayoutWidget::quantityChanged);
    connect(ui->playbackComboBox, &QComboBox::currentIndexChanged, this, &ChromasoundLayoutWidget::playbackChanged);
    connect(ui->interactiveComboBox, &QComboBox::currentIndexChanged, this, &ChromasoundLayoutWidget::interactiveChanged);
    connect(ui->implementationComboBox, &QComboBox::currentIndexChanged, this, &ChromasoundLayoutWidget::implementationChanged);

    quantityChanged(quantity - 1);
}

ChromasoundLayoutWidget::~ChromasoundLayoutWidget()
{
    delete ui;
}

int ChromasoundLayoutWidget::quantity() const
{
    return ui->numberOfComboBox->currentIndex() + 1;
}

QString ChromasoundLayoutWidget::chromasound1() const
{
    if (quantity() == 1) {
        return ui->implementationComboBox->currentIndex() == 0
               ? Chromasound_Studio::Emulator
               : Chromasound_Studio::ChromasoundDirect;
    }

    return ui->playbackComboBox->currentIndex() == 0
           ? Chromasound_Studio::Emulator
           : Chromasound_Studio::ChromasoundDirect;
}

QString ChromasoundLayoutWidget::chromasound2() const
{
    return ui->interactiveComboBox->currentIndex() == 0
           ? Chromasound_Studio::Emulator
           : Chromasound_Studio::ChromasoundDirect;
}

void ChromasoundLayoutWidget::quantityChanged(int index)
{
    int quantity = index + 1;

    ui->playbackComboBox->setVisible(quantity == 2);
    ui->playbackLabel->setVisible(quantity == 2);
    ui->interactiveComboBox->setVisible(quantity == 2);
    ui->interactiveLabel->setVisible(quantity == 2);
    ui->implementationComboBox->setVisible(quantity == 1);
    ui->implementationLabel->setVisible(quantity == 1);

    if (quantity == 1) {
        ui->infoLabel->setText("<html><body><p>One Chromasound cannot play the song and MIDI at the same time.</p><p>MIDI is active when the Chromasound is stopped.</p></body></html>");
    } else {
        ui->infoLabel->setText("<html><body><p>Choose which implementation to use for playback and for MIDI. No more than one Chromasound Direct is supported.</p><p>MIDI is active even when the song is playing.</p></body></html>");
    }

    emit changed();
}

void ChromasoundLayoutWidget::playbackChanged(int index)
{
    if (index == 1) {
        if (ui->interactiveComboBox->count() > 1) {
            ui->interactiveComboBox->removeItem(1);
        }
    } else {
        if (ui->interactiveComboBox->count() == 1) {
            ui->interactiveComboBox->addItem("Chromasound Direct");
        }
    }

    emit changed();
}

void ChromasoundLayoutWidget::interactiveChanged(int index)
{
    if (index == 1) {
        if (ui->playbackComboBox->count() > 1) {
            ui->playbackComboBox->removeItem(1);
        }
    } else {
        if (ui->playbackComboBox->count() == 1) {
            ui->playbackComboBox->addItem("Chromasound Direct");
        }
    }

    emit changed();
}

void ChromasoundLayoutWidget::implementationChanged(int index)
{
    emit changed();
}

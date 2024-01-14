#include "melodywidget.h"
#include "ui_melodywidget.h"

MelodyWidget::MelodyWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::MelodyWidget),
    _app(app)
{
    ui->setupUi(this);

    connect(ui->instrumentComboBox, &QComboBox::currentIndexChanged, this, &MelodyWidget::selectionChanged);
}

MelodyWidget::~MelodyWidget()
{
    delete ui;
}

void MelodyWidget::setApplication(Application* app)
{
    _app = app;
}

void MelodyWidget::setSettings(MelodyChannelSettings* settings)
{
    _settings = settings;

    ui->instrumentComboBox->blockSignals(true);
    ui->instrumentComboBox->setCurrentIndex(settings->instrument());
    ui->instrumentComboBox->blockSignals(false);
}

void MelodyWidget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void MelodyWidget::selectionChanged(int index)
{
    MelodyChannelSettings newSettings(*_settings);
    newSettings.setInstrument(static_cast<MelodyChannelSettings::Instrument>(index));

    _app->undoStack().push(new EditMelodyChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

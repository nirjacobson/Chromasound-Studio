#include "rhythmwidget.h"
#include "ui_rhythmwidget.h"

RhythmWidget::RhythmWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::RhythmWidget),
    _app(app),
    _settings(nullptr)
{
    ui->setupUi(this);

    connect(ui->instrumentComboBox, &QComboBox::currentIndexChanged, this, &RhythmWidget::selectionChanged);
}

RhythmWidget::~RhythmWidget()
{
    delete ui;
}

void RhythmWidget::setApplication(Application* app)
{
    _app = app;
}

void RhythmWidget::setSettings(RhythmChannelSettings* settings)
{
    _settings = settings;

    ui->instrumentComboBox->blockSignals(true);
    ui->instrumentComboBox->setCurrentIndex(settings->instrument());
    ui->instrumentComboBox->blockSignals(false);
}

void RhythmWidget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void RhythmWidget::selectionChanged(int index)
{
    RhythmChannelSettings newSettings(*_settings);
    newSettings.setInstrument(static_cast<RhythmChannelSettings::Instrument>(index));

    _app->undoStack().push(new EditRhythmChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

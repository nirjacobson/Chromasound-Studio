#include "romwidget.h"
#include "ui_romwidget.h"

ROMWidget::ROMWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::ROMWidget)
    , _app(app)
{
    ui->setupUi(this);

    if (app) {
        ui->sampleComboBox->addItems(ROM(app->project().romFile()).names());
    }

    connect(ui->sampleComboBox, &QComboBox::currentIndexChanged, this, &ROMWidget::selectionChanged);
}

ROMWidget::~ROMWidget()
{
    delete ui;
}

void ROMWidget::setApplication(Application* app)
{
    _app = app;

    ui->sampleComboBox->blockSignals(true);
    ui->sampleComboBox->addItems(ROM(app->project().romFile()).names());
    ui->sampleComboBox->blockSignals(false);
}

void ROMWidget::setSettings(ROMChannelSettings* settings)
{
    _settings = settings;

    ui->sampleComboBox->blockSignals(true);
    ui->sampleComboBox->setCurrentIndex(settings->patch());
    ui->sampleComboBox->blockSignals(false);
}

void ROMWidget::doUpdate()
{
    ui->sampleComboBox->blockSignals(true);
    ui->sampleComboBox->clear();
    ui->sampleComboBox->addItems(ROM(_app->project().romFile()).names());
    ui->sampleComboBox->blockSignals(false);

    if (_settings) setSettings(_settings);
}

void ROMWidget::selectionChanged(int index)
{
    ROMChannelSettings newSettings(*_settings);
    newSettings.setPatch(index);

    _app->undoStack().push(new EditROMChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

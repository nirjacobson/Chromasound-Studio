#include "melodywidget.h"
#include "ui_melodywidget.h"

MelodyWidget::MelodyWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::MelodyWidget),
    _app(app)
{
    ui->setupUi(this);

    if (app) {
        ui->instrumentComboBox->addItems(OPLL::patches(_app->project().opllType()));
    }

    connect(ui->instrumentComboBox, &QComboBox::currentIndexChanged, this, &MelodyWidget::selectionChanged);
}

MelodyWidget::~MelodyWidget()
{
    delete ui;
}

void MelodyWidget::setApplication(Application* app)
{
    _app = app;

    ui->instrumentComboBox->blockSignals(true);
    ui->instrumentComboBox->addItems(OPLL::patches(_app->project().opllType()));
    ui->instrumentComboBox->blockSignals(false);
}

void MelodyWidget::setSettings(MelodyChannelSettings* settings)
{
    _settings = settings;

    ui->instrumentComboBox->blockSignals(true);
    ui->instrumentComboBox->setCurrentIndex(settings->patch());
    ui->instrumentComboBox->blockSignals(false);
}

void MelodyWidget::doUpdate()
{
    ui->instrumentComboBox->blockSignals(true);
    ui->instrumentComboBox->clear();
    ui->instrumentComboBox->addItems(OPLL::patches(_app->project().opllType()));
    ui->instrumentComboBox->blockSignals(false);

    if (_settings) setSettings(_settings);
}

void MelodyWidget::selectionChanged(int index)
{
    MelodyChannelSettings newSettings(*_settings);
    newSettings.setPatch(index);

    _app->undoStack().push(new EditMelodyChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

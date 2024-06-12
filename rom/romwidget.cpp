#include "romwidget.h"
#include "ui_romwidget.h"

ROMWidget::ROMWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::ROMWidget)
    , _app(app)
{
    ui->setupUi(this);

    QStringList items;
    if (app) {
        items = ROM(app->project().resolve(app->project().romFile())).names();
        ui->sampleComboBox->addItems(items);
    }

    ui->stackedWidget->setCurrentIndex(!items.empty());

    connect(ui->sampleComboBox, &QComboBox::currentIndexChanged, this, &ROMWidget::selectionChanged);
}

ROMWidget::~ROMWidget()
{
    delete ui;
}

void ROMWidget::setApplication(Application* app)
{
    _app = app;

    QStringList items = ROM(app->project().resolve(app->project().romFile())).names();
    ui->sampleComboBox->blockSignals(true);
    ui->sampleComboBox->addItems(items);
    ui->sampleComboBox->blockSignals(false);

    ui->stackedWidget->setCurrentIndex(!items.empty());
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
    QStringList items = ROM(_app->project().resolve(_app->project().romFile())).names();
    ui->sampleComboBox->blockSignals(true);
    ui->sampleComboBox->clear();
    ui->sampleComboBox->addItems(items);
    ui->sampleComboBox->blockSignals(false);

    if (_settings) setSettings(_settings);

    ui->stackedWidget->setCurrentIndex(!items.empty());
}

void ROMWidget::selectionChanged(int index)
{
    ROMChannelSettings newSettings(*_settings);
    newSettings.setPatch(index);

    _app->undoStack().push(new EditROMChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

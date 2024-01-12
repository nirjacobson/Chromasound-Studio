#include "ssgwidget.h"
#include "ui_ssgwidget.h"

SSGWidget::SSGWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , _app(app)
    , ui(new Ui::SSGWidget)
    , _settings(nullptr)
{
    ui->setupUi(this);

    connect(ui->envLed, &LED::clicked, this, &SSGWidget::envLEDClicked);
    connect(ui->noiseLed, &LED::clicked, this, &SSGWidget::noiseLEDClicked);
    connect(ui->toneLed, &LED::clicked, this, &SSGWidget::toneLEDClicked);
}

SSGWidget::~SSGWidget()
{
    delete ui;
}

void SSGWidget::setApplication(Application* app)
{
    _app = app;
}

void SSGWidget::setSettings(SSGChannelSettings* settings)
{
    _settings = settings;

    ui->toneLed->setOn(_settings->tone());
    ui->noiseLed->setOn(_settings->noise());
    ui->envLed->setOn(_settings->envelope());
}

void SSGWidget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void SSGWidget::toneLEDClicked(bool shift)
{
    ui->toneLed->setOn(!ui->toneLed->on());

    SSGChannelSettings newSettings(*_settings);
    newSettings.setTone(ui->toneLed->on());

    _app->undoStack().push(new EditSSGChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

void SSGWidget::noiseLEDClicked(bool shift)
{
    ui->noiseLed->setOn(!ui->noiseLed->on());

    SSGChannelSettings newSettings(*_settings);
    newSettings.setNoise(ui->noiseLed->on());

    _app->undoStack().push(new EditSSGChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

void SSGWidget::envLEDClicked(bool shift)
{
    ui->envLed->setOn(!ui->envLed->on());

    SSGChannelSettings newSettings(*_settings);
    newSettings.setEnvelope(ui->envLed->on());

    _app->undoStack().push(new EditSSGChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

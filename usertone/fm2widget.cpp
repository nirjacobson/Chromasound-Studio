#include "fm2widget.h"
#include "ui_fm2widget.h"

FM2Widget::FM2Widget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::FM2Widget),
    _app(app),
    _settings(nullptr)
{
    ui->setupUi(this);

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    connect(ui->tlDial, &QDial::valueChanged, this, &FM2Widget::tlDialChanged);
    connect(ui->fbDial, &QDial::valueChanged, this, &FM2Widget::fbDialChanged);
}

FM2Widget::~FM2Widget()
{
    delete ui;
}

void FM2Widget::setApplication(Application* app)
{
    _app = app;

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
}

void FM2Widget::setSettings(FM2Settings* settings)
{
    _settings = settings;

    ui->operatorWidget1->blockSignals(true);
    ui->operatorWidget1->setSettings(&settings->operators()[0]);
    ui->operatorWidget1->blockSignals(false);

    ui->operatorWidget2->blockSignals(true);
    ui->operatorWidget2->setSettings(&settings->operators()[1]);
    ui->operatorWidget2->blockSignals(false);

    ui->tlDial->blockSignals(true);
    ui->tlDial->setValue(settings->tl());
    ui->tlDial->blockSignals(false);

    ui->tlValueLabel->setText(QString::number(_settings->tl()));
    ui->fbDial->setValue(settings->fb());
    ui->fbValueLabel->setText(QString::number(_settings->fb()));
}

void FM2Widget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void FM2Widget::tlDialChanged(const int value)
{
    FM2Settings newSettings(*_settings);

    newSettings.setTl(value);

    _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), *_settings, newSettings));
}

void FM2Widget::fbDialChanged(const int value)
{
    FM2Settings newSettings(*_settings);

    newSettings.setFb(value);

    _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), *_settings, newSettings));
}

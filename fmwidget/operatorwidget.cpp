#include "operatorwidget.h"
#include "ui_operatorwidget.h"

OperatorWidget::OperatorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OperatorWidget)
    , _app(nullptr)
{
    ui->setupUi(this);

    connect(ui->arDial, &QDial::valueChanged, this, &OperatorWidget::arDialChanged);
    connect(ui->t1lDial, &QDial::valueChanged, this, &OperatorWidget::t1lDialChanged);
    connect(ui->d1rDial, &QDial::valueChanged, this, &OperatorWidget::d1rDialChanged);
    connect(ui->t2lDial, &QDial::valueChanged, this, &OperatorWidget::t2lDialChanged);
    connect(ui->d2rDial, &QDial::valueChanged, this, &OperatorWidget::d2rDialChanged);
    connect(ui->rrDial, &QDial::valueChanged, this, &OperatorWidget::rrDialChanged);
    connect(ui->mulComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &OperatorWidget::mulChanged);
    connect(ui->dtComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &OperatorWidget::dtChanged);
    connect(ui->rsDial, &QDial::valueChanged, this, &OperatorWidget::rsDialChanged);
    connect(ui->amLed, &LED::clicked, this, &OperatorWidget::amLedClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &OperatorWidget::resetEnvelopeSettings);
}

OperatorWidget::~OperatorWidget()
{
    delete ui;
}

void OperatorWidget::setApplication(Application* app)
{
    _app = app;
}

void OperatorWidget::setSettings(OperatorSettings* settings)
{
    _settings = settings;

    ui->arDial->blockSignals(true);
    ui->arDial->setValue(_settings->envelopeSettings().ar());
    ui->arDial->blockSignals(false);
    ui->arValueLabel->setText(QString::number(_settings->envelopeSettings().ar()));

    ui->t1lDial->blockSignals(true);
    ui->t1lDial->setValue(_settings->envelopeSettings().t1l());
    ui->t1lDial->blockSignals(false);
    ui->t1lValueLabel->setText(QString::number(_settings->envelopeSettings().t1l()));

    ui->d1rDial->blockSignals(true);
    ui->d1rDial->setValue(_settings->envelopeSettings().d1r());
    ui->d1rDial->blockSignals(false);
    ui->d1rValueLabel->setText(QString::number(_settings->envelopeSettings().d1r()));

    ui->t2lDial->blockSignals(true);
    ui->t2lDial->setValue(_settings->envelopeSettings().t2l());
    ui->t2lDial->blockSignals(false);
    ui->t2lValueLabel->setText(QString::number(_settings->envelopeSettings().t2l()));

    ui->d2rDial->blockSignals(true);
    ui->d2rDial->setValue(_settings->envelopeSettings().d2r());
    ui->d2rDial->blockSignals(false);
    ui->d2rValueLabel->setText(QString::number(_settings->envelopeSettings().d2r()));

    ui->rrDial->blockSignals(true);
    ui->rrDial->setValue(_settings->envelopeSettings().rr());
    ui->rrDial->blockSignals(false);
    ui->rrValueLabel->setText(QString::number(_settings->envelopeSettings().rr()));

    ui->mulComboBox->blockSignals(true);
    ui->mulComboBox->setCurrentIndex(_settings->mul());
    ui->mulComboBox->blockSignals(false);

    ui->dtComboBox->blockSignals(true);
    ui->dtComboBox->setCurrentIndex(dtToIndex(_settings->dt()));
    ui->dtComboBox->blockSignals(false);

    ui->rsDial->blockSignals(true);
    ui->rsDial->setValue(_settings->rs());
    ui->rsDial->blockSignals(false);
    ui->rsValueLabel->setText(QString::number(_settings->rs()));

    ui->amLed->blockSignals(true);
    ui->amLed->setOn(_settings->am());
    ui->amLed->blockSignals(false);

    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

int OperatorWidget::dtToIndex(const int dt) const
{
    if (dt >= 4) {
        return 7 - dt;
    } else {
        return 3 + dt;
    }
}

void OperatorWidget::arDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setAr(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::t1lDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setT1l(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::d1rDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setD1r(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::t2lDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setT2l(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::d2rDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setD2r(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::rrDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.envelopeSettings().setRr(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::mulChanged(const int idx)
{
    OperatorSettings newSettings(*_settings);

    newSettings.setMul(idx);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::dtChanged(const int idx)
{
    OperatorSettings newSettings(*_settings);

    if (idx < 3) {
        newSettings.setDt(7 - idx);
    } else {
        newSettings.setDt(idx - 3);
    }

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::rsDialChanged(const int value)
{
    OperatorSettings newSettings(*_settings);

    newSettings.setRs(value);

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::amLedClicked(bool)
{
    OperatorSettings newSettings(*_settings);

    newSettings.setAm(!_settings->am());

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

void OperatorWidget::resetEnvelopeSettings()
{
    OperatorSettings newSettings(*_settings);
    newSettings.envelopeSettings() = EnvelopeSettings();

    _app->undoStack().push(new EditOperatorSettingsCommand(_app->window(), *_settings, newSettings));
}

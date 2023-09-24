#include "operatorwidget.h"
#include "ui_operatorwidget.h"

OperatorWidget::OperatorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OperatorWidget)
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

void OperatorWidget::setSettings(OperatorSettings* settings)
{
    _settings = settings;

    ui->arDial->setValue(_settings->envelopeSettings().ar());
    ui->t1lDial->setValue(_settings->envelopeSettings().t1l());
    ui->d1rDial->setValue(_settings->envelopeSettings().d1r());
    ui->t2lDial->setValue(_settings->envelopeSettings().t2l());
    ui->d2rDial->setValue(_settings->envelopeSettings().d2r());
    ui->rrDial->setValue(_settings->envelopeSettings().rr());
    ui->mulComboBox->setCurrentIndex(_settings->mul());
    ui->dtComboBox->setCurrentIndex(dtToIndex(_settings->dt()));
    ui->rsDial->setValue(_settings->rs());
    ui->amLed->setOn(_settings->am());
}

int OperatorWidget::dtToIndex(const int dt) const
{
    if (dt > 4) {
        return 7 - dt;
    } else {
        return 3 + dt;
    }
}

void OperatorWidget::arDialChanged(const int value)
{
    ui->arValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setAr(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::t1lDialChanged(const int value)
{
    ui->t1lValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setT1l(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::d1rDialChanged(const int value)
{
    ui->d1rValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setD1r(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::t2lDialChanged(const int value)
{
    ui->t2lValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setT2l(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::d2rDialChanged(const int value)
{
    ui->d2rValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setD2r(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::rrDialChanged(const int value)
{
    ui->rrValueLabel->setText(QString::number(value));
    _settings->envelopeSettings().setRr(value);
    ui->envelopeDisplayWidget->setSettings(&_settings->envelopeSettings());
}

void OperatorWidget::mulChanged(const int idx)
{
    _settings->setMul(idx);
}

void OperatorWidget::dtChanged(const int idx)
{
    if (idx < 3) {
        _settings->setDt(7 - idx);
    } else {
        _settings->setDt(idx - 3);
    }
}

void OperatorWidget::rsDialChanged(const int value)
{
    ui->rsValueLabel->setText(QString::number(value));
    _settings->setRs(value);
}

void OperatorWidget::amLedClicked(bool)
{
    _settings->setAm(!_settings->am());

    ui->amLed->setOn(_settings->am());
}

void OperatorWidget::resetEnvelopeSettings()
{
    _settings->envelopeSettings() = EnvelopeSettings();

    ui->arDial->setValue(_settings->envelopeSettings().ar());
    ui->t1lDial->setValue(_settings->envelopeSettings().t1l());
    ui->d1rDial->setValue(_settings->envelopeSettings().d1r());
    ui->t2lDial->setValue(_settings->envelopeSettings().t2l());
    ui->d2rDial->setValue(_settings->envelopeSettings().d2r());
    ui->rrDial->setValue(_settings->envelopeSettings().rr());
}

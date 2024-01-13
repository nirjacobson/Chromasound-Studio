#include "fm2widget.h"
#include "ui_fm2widget.h"

FM2Widget::FM2Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FM2Widget)
{
    ui->setupUi(this);

    connect(ui->operatorWidget1, &FM2OperatorWidget::changed, this, &FM2Widget::operator1Changed);
    connect(ui->operatorWidget2, &FM2OperatorWidget::changed, this, &FM2Widget::operator2Changed);
    connect(ui->tlDial, &QDial::valueChanged, this, &FM2Widget::tlDialChanged);
    connect(ui->fbDial, &QDial::valueChanged, this, &FM2Widget::fbDialChanged);
}

FM2Widget::~FM2Widget()
{
    delete ui;
}

const FM2Settings& FM2Widget::settings() const
{
    return _settings;
}

void FM2Widget::set(const FM2Settings& settings)
{
    _settings = settings;

    ui->operatorWidget1->blockSignals(true);
    ui->operatorWidget1->set(_settings.operators()[0]);
    ui->operatorWidget1->blockSignals(false);

    ui->operatorWidget2->blockSignals(true);
    ui->operatorWidget2->set(_settings.operators()[1]);
    ui->operatorWidget2->blockSignals(false);

    ui->tlDial->blockSignals(true);
    ui->tlDial->setValue(_settings.tl());
    ui->tlDial->blockSignals(false);

    ui->fbDial->blockSignals(true);
    ui->fbDial->setValue(_settings.fb());
    ui->fbDial->blockSignals(false);

    ui->tlValueLabel->setText(QString::number(_settings.tl()));
    ui->fbDial->setValue(_settings.fb());
    ui->fbValueLabel->setText(QString::number(_settings.fb()));
}

void FM2Widget::operator1Changed()
{
    _settings.operators()[0] = ui->operatorWidget1->settings();
    emit changed();
}

void FM2Widget::operator2Changed()
{
    _settings.operators()[1] = ui->operatorWidget2->settings();
    emit changed();
}

void FM2Widget::tlDialChanged(const int value)
{
    _settings.setTl(value);

    emit changed();
}

void FM2Widget::fbDialChanged(const int value)
{
    _settings.setFb(value);

    emit changed();
}

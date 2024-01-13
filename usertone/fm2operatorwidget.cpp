#include "fm2operatorwidget.h"
#include "ui_fm2operatorwidget.h"

FM2OperatorWidget::FM2OperatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FM2OperatorWidget)
{
    ui->setupUi(this);

    connect(ui->envTypeComboBox, &QComboBox::currentIndexChanged, this, &FM2OperatorWidget::envTypeChanged);
    connect(ui->arDial, &QDial::valueChanged, this, &FM2OperatorWidget::arDialChanged);
    connect(ui->drDial, &QDial::valueChanged, this, &FM2OperatorWidget::drDialChanged);
    connect(ui->slDial, &QDial::valueChanged, this, &FM2OperatorWidget::slDialChanged);
    connect(ui->rrDial, &QDial::valueChanged, this, &FM2OperatorWidget::rrDialChanged);
    connect(ui->multiDial, &QDial::valueChanged, this, &FM2OperatorWidget::multiDialChanged);
    connect(ui->kslDial, &QDial::valueChanged, this, &FM2OperatorWidget::kslDialChanged);
    connect(ui->amLed, &LED::clicked, this, &FM2OperatorWidget::amChanged);
    connect(ui->vibLed, &LED::clicked, this, &FM2OperatorWidget::vibChanged);
    connect(ui->ksrLed, &LED::clicked, this, &FM2OperatorWidget::ksrChanged);
    connect(ui->dLed, &LED::clicked, this, &FM2OperatorWidget::dChanged);
}

FM2OperatorWidget::~FM2OperatorWidget()
{
    delete ui;
}

const FM2OperatorSettings& FM2OperatorWidget::settings() const
{
    return _settings;
}

void FM2OperatorWidget::set(const FM2OperatorSettings& settings)
{
    _settings = settings;

    ui->envTypeComboBox->blockSignals(true);
    ui->envTypeComboBox->setCurrentIndex(_settings.envelopeSettings().type() == FM2EnvelopeSettings::EGType::Percussive ? 0 : 1);
    ui->envTypeComboBox->blockSignals(false);

    ui->arDial->blockSignals(true);
    ui->arDial->setValue(_settings.envelopeSettings().ar());
    ui->arDial->blockSignals(false);
    ui->arValueLabel->setText(QString::number(_settings.envelopeSettings().ar()));

    ui->drDial->blockSignals(true);
    ui->drDial->setValue(_settings.envelopeSettings().dr());
    ui->drDial->blockSignals(false);
    ui->drValueLabel->setText(QString::number(_settings.envelopeSettings().dr()));

    ui->slDial->blockSignals(true);
    ui->slDial->setValue(_settings.envelopeSettings().sl());
    ui->slDial->blockSignals(false);
    ui->slValueLabel->setText(QString::number(_settings.envelopeSettings().sl()));

    ui->rrDial->blockSignals(true);
    ui->rrDial->setValue(_settings.envelopeSettings().rr());
    ui->rrDial->blockSignals(false);
    ui->rrValueLabel->setText(QString::number(_settings.envelopeSettings().rr()));

    ui->multiDial->blockSignals(true);
    ui->multiDial->setValue(_settings.multi());
    ui->multiDial->blockSignals(false);
    ui->multiValueLabel->setText(QString::number(_settings.multi()));

    ui->kslDial->blockSignals(true);
    ui->kslDial->setValue(_settings.ksl());
    ui->kslDial->blockSignals(false);
    ui->kslValueLabel->setText(QString::number(_settings.ksl()));

    ui->amLed->setOn(_settings.am());
    ui->vibLed->setOn(_settings.vib());
    ui->ksrLed->setOn(_settings.ksr());
    ui->dLed->setOn(_settings.d());

    ui->envelopeDisplayWidget->setSettings(&_settings.envelopeSettings());
}

void FM2OperatorWidget::envTypeChanged(const int index)
{
    _settings.envelopeSettings().setType((FM2EnvelopeSettings::EGType)index);

    emit changed();
}

void FM2OperatorWidget::arDialChanged(const int value)
{
    _settings.envelopeSettings().setAr(value);

    emit changed();
}

void FM2OperatorWidget::drDialChanged(const int value)
{
    _settings.envelopeSettings().setDr(value);

    emit changed();
}

void FM2OperatorWidget::slDialChanged(const int value)
{
    _settings.envelopeSettings().setSl(value);

    emit changed();
}

void FM2OperatorWidget::rrDialChanged(const int value)
{
    _settings.envelopeSettings().setRr(value);

    emit changed();
}

void FM2OperatorWidget::multiDialChanged(const int value)
{
    _settings.setMulti(value);

    emit changed();
}

void FM2OperatorWidget::kslDialChanged(const int value)
{
    _settings.setKsl(value);

    emit changed();
}

void FM2OperatorWidget::amChanged()
{
    ui->amLed->setOn(!ui->amLed->on());

    _settings.setAm(ui->amLed->on());

    emit changed();
}

void FM2OperatorWidget::vibChanged()
{
    ui->vibLed->setOn(!ui->vibLed->on());

    _settings.setVib(ui->vibLed->on());

    emit changed();
}

void FM2OperatorWidget::ksrChanged()
{
    ui->ksrLed->setOn(!ui->ksrLed->on());

    _settings.setKsr(ui->ksrLed->on());

    emit changed();
}

void FM2OperatorWidget::dChanged()
{
    ui->dLed->setOn(!ui->dLed->on());

    _settings.setD(ui->dLed->on());

    emit changed();
}

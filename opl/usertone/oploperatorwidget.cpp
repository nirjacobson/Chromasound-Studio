#include "oploperatorwidget.h"
#include "ui_oploperatorwidget.h"

OPLOperatorWidget::OPLOperatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OPLOperatorWidget)
{
    ui->setupUi(this);

    connect(ui->envTypeComboBox, &QComboBox::currentIndexChanged, this, &OPLOperatorWidget::envTypeChanged);
    connect(ui->arDial, &QDial::valueChanged, this, &OPLOperatorWidget::arDialChanged);
    connect(ui->drDial, &QDial::valueChanged, this, &OPLOperatorWidget::drDialChanged);
    connect(ui->slDial, &QDial::valueChanged, this, &OPLOperatorWidget::slDialChanged);
    connect(ui->rrDial, &QDial::valueChanged, this, &OPLOperatorWidget::rrDialChanged);
    connect(ui->multiDial, &QDial::valueChanged, this, &OPLOperatorWidget::multiDialChanged);
    connect(ui->kslDial, &QDial::valueChanged, this, &OPLOperatorWidget::kslDialChanged);
    connect(ui->amLed, &LED::clicked, this, &OPLOperatorWidget::amChanged);
    connect(ui->vibLed, &LED::clicked, this, &OPLOperatorWidget::vibChanged);
    connect(ui->ksrLed, &LED::clicked, this, &OPLOperatorWidget::ksrChanged);
    connect(ui->dLed, &LED::clicked, this, &OPLOperatorWidget::dChanged);
}

OPLOperatorWidget::~OPLOperatorWidget()
{
    delete ui;
}

const OPLOperatorSettings& OPLOperatorWidget::settings() const
{
    return _settings;
}

void OPLOperatorWidget::set(const OPLOperatorSettings& settings)
{
    _settings = settings;

    ui->envTypeComboBox->blockSignals(true);
    ui->envTypeComboBox->setCurrentIndex(_settings.envelopeSettings().type() == OPLEnvelopeSettings::EGType::Percussive ? 0 : 1);
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

void OPLOperatorWidget::envTypeChanged(const int index)
{
    _settings.envelopeSettings().setType((OPLEnvelopeSettings::EGType)index);

    emit changed();
}

void OPLOperatorWidget::arDialChanged(const int value)
{
    _settings.envelopeSettings().setAr(value);

    emit changed();
}

void OPLOperatorWidget::drDialChanged(const int value)
{
    _settings.envelopeSettings().setDr(value);

    emit changed();
}

void OPLOperatorWidget::slDialChanged(const int value)
{
    _settings.envelopeSettings().setSl(value);

    emit changed();
}

void OPLOperatorWidget::rrDialChanged(const int value)
{
    _settings.envelopeSettings().setRr(value);

    emit changed();
}

void OPLOperatorWidget::multiDialChanged(const int value)
{
    _settings.setMulti(value);

    emit changed();
}

void OPLOperatorWidget::kslDialChanged(const int value)
{
    _settings.setKsl(value);

    emit changed();
}

void OPLOperatorWidget::amChanged()
{
    ui->amLed->setOn(!ui->amLed->on());

    _settings.setAm(ui->amLed->on());

    emit changed();
}

void OPLOperatorWidget::vibChanged()
{
    ui->vibLed->setOn(!ui->vibLed->on());

    _settings.setVib(ui->vibLed->on());

    emit changed();
}

void OPLOperatorWidget::ksrChanged()
{
    ui->ksrLed->setOn(!ui->ksrLed->on());

    _settings.setKsr(ui->ksrLed->on());

    emit changed();
}

void OPLOperatorWidget::dChanged()
{
    ui->dLed->setOn(!ui->dLed->on());

    _settings.setD(ui->dLed->on());

    emit changed();
}

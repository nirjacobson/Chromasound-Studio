#include "ssgenvelopeshapewidget.h"
#include "ui_ssgenvelopeshapewidget.h"

SSGEnvelopeShapeWidget::SSGEnvelopeShapeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SSGEnvelopeShapeWidget)
{
    ui->setupUi(this);
    ui->displayWidget->setSettings(&_settings);

    connect(ui->contButton, &QPushButton::toggled, this, &SSGEnvelopeShapeWidget::contToggled);
    connect(ui->attButton, &QPushButton::toggled, this, &SSGEnvelopeShapeWidget::attToggled);
    connect(ui->altButton, &QPushButton::toggled, this, &SSGEnvelopeShapeWidget::altToggled);
    connect(ui->holdButton, &QPushButton::toggled, this, &SSGEnvelopeShapeWidget::holdToggled);
}

SSGEnvelopeShapeWidget::~SSGEnvelopeShapeWidget()
{
    delete ui;
}

const SSGEnvelopeSettings& SSGEnvelopeShapeWidget::settings() const
{
    return _settings;
}

void SSGEnvelopeShapeWidget::set(const SSGEnvelopeSettings& settings)
{
    _settings = settings;

    ui->contButton->setChecked(settings.cont());
    ui->attButton->setChecked(settings.att());
    ui->altButton->setChecked(settings.alt());
    ui->holdButton->setChecked(settings.hold());

    ui->displayWidget->update();
}

void SSGEnvelopeShapeWidget::contToggled(const bool enabled)
{
    _settings.setCont(enabled);
    ui->displayWidget->update();
    emit changed();
}

void SSGEnvelopeShapeWidget::attToggled(const bool enabled)
{
    _settings.setAtt(enabled);
    ui->displayWidget->update();
    emit changed();
}

void SSGEnvelopeShapeWidget::altToggled(const bool enabled)
{
    _settings.setAlt(enabled);
    ui->displayWidget->update();
    emit changed();
}

void SSGEnvelopeShapeWidget::holdToggled(const bool enabled)
{
    _settings.setHold(enabled);
    ui->displayWidget->update();
    emit changed();
}

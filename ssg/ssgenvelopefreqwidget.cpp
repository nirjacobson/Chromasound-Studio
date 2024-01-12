#include "ssgenvelopefreqwidget.h"
#include "ui_ssgenvelopefreqwidget.h"

SSGEnvelopeFreqWidget::SSGEnvelopeFreqWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SSGEnvelopeFreqWidget)
{
    ui->setupUi(this);

    connect(ui->freqSpinBox, &QSpinBox::valueChanged, this, &SSGEnvelopeFreqWidget::freqChanged);
}

SSGEnvelopeFreqWidget::~SSGEnvelopeFreqWidget()
{
    delete ui;
}

int SSGEnvelopeFreqWidget::setting() const
{
    return ui->freqSpinBox->value();
}

void SSGEnvelopeFreqWidget::set(const int freq)
{
    ui->freqSpinBox->setValue(freq);
}

void SSGEnvelopeFreqWidget::freqChanged(int value)
{
    float envFreq = value == 0 ? 0 : (3579545.0f / (512 * value));
    ui->freqLabel->setText(QString("%1 Hz").arg(envFreq));
    emit changed();
}

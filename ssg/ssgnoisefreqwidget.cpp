#include "ssgnoisefreqwidget.h"
#include "ui_ssgnoisefreqwidget.h"

SSGNoiseFreqWidget::SSGNoiseFreqWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SSGNoiseFreqWidget)
{
    ui->setupUi(this);

    connect(ui->freqSpinBox, &QSpinBox::valueChanged, this, &SSGNoiseFreqWidget::freqChanged);
}

SSGNoiseFreqWidget::~SSGNoiseFreqWidget()
{
    delete ui;
}

int SSGNoiseFreqWidget::setting() const
{
    return ui->freqSpinBox->value();
}

void SSGNoiseFreqWidget::set(const int freq)
{
    ui->freqSpinBox->setValue(freq);
}

void SSGNoiseFreqWidget::freqChanged(int value)
{
    float envFreq = value == 0 ? 0 : (3579545.0f / (32 * value));
    ui->freqLabel->setText(QString("%1 Hz").arg(envFreq));
    emit changed();
}

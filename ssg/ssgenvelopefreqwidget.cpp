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

void SSGEnvelopeFreqWidget::freqChanged(int value)
{
    float envFreq = value == 0 ? 0 : (3579545.0f / (512 * value));
    ui->freqLabel->setText(QString("%1 Hz").arg(envFreq));
}

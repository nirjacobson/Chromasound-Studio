#ifndef SSGENVELOPEFREQWIDGET_H
#define SSGENVELOPEFREQWIDGET_H

#include <QWidget>

namespace Ui {
class SSGEnvelopeFreqWidget;
}

class SSGEnvelopeFreqWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SSGEnvelopeFreqWidget(QWidget *parent = nullptr);
    ~SSGEnvelopeFreqWidget();

private:
    Ui::SSGEnvelopeFreqWidget *ui;

private slots:
    void freqChanged(int value);
};

#endif // SSGENVELOPEFREQWIDGET_H

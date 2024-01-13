#ifndef SSGNOISEFREQWIDGET_H
#define SSGNOISEFREQWIDGET_H

#include <QWidget>

namespace Ui {
    class SSGNoiseFreqWidget;
}

class SSGNoiseFreqWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit SSGNoiseFreqWidget(QWidget *parent = nullptr);
        ~SSGNoiseFreqWidget();

        int setting() const;
        void set(const int freq);
    signals:
        void changed();

    private:
        Ui::SSGNoiseFreqWidget *ui;

    private slots:
        void freqChanged(int value);
};

#endif // SSGNOISEFREQWIDGET_H

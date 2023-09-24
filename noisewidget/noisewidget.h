#ifndef NOISEWIDGET_H
#define NOISEWIDGET_H

#include <QWidget>
#include <QComboBox>

#include "../project/channel/noisechannelsettings.h"

namespace Ui {
    class NoiseWidget;
}

class NoiseWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit NoiseWidget(QWidget *parent = nullptr);
        ~NoiseWidget();

        void setSettings(NoiseChannelSettings* settings);

    private:
        Ui::NoiseWidget *ui;
        NoiseChannelSettings* _settings;

    private slots:
        void typeChanged(int idx);
        void shiftRateChanged(int idx);
};

#endif // NOISEWIDGET_H

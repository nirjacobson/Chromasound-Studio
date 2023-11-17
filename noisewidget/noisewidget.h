#ifndef NOISEWIDGET_H
#define NOISEWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QStyleOption>
#include <QPainter>

#include "application.h"
#include "project/channel/noisechannelsettings.h"
#include "commands/editnoisechannelsettingscommand.h"

namespace Ui {
    class NoiseWidget;
}

class NoiseWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit NoiseWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~NoiseWidget();

        void setSettings(NoiseChannelSettings* settings);
        void doUpdate();

        void setApplication(Application* app);

    private:
        Ui::NoiseWidget *ui;
        Application* _app;
        NoiseChannelSettings* _settings;

    private slots:
        void typeChanged(int idx);
        void shiftRateChanged(int idx);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // NOISEWIDGET_H

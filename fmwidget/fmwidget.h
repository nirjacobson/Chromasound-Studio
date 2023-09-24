#ifndef FMWIDGET_H
#define FMWIDGET_H

#include <QWidget>
#include "project/channel/fmchannelsettings.h"

namespace Ui {
    class FMWidget;
}

class FMWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit FMWidget(QWidget *parent = nullptr);
        ~FMWidget();

        void setSettings(FMChannelSettings* settings);

    private:
        Ui::FMWidget *ui;
        FMChannelSettings* _settings;

    private slots:
        void algorithmChanged(const int a);
        void feedbackChanged(const int fb);
};

#endif // FMWIDGET_H

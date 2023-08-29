#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>
#include <QStyle>

#include "application.h"

namespace Ui {
    class TopWidget;
}

class TopWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit TopWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~TopWidget();

        void setApplication(Application* app);

    signals:
        void play();
        void pause();
        void stop();
        void patternChanged(int);
        void tempoChanged(int);
        void beatsPerBarChanged(int);

    private:
        Ui::TopWidget *ui;
        Application* _app;

        bool _isPlaying;

    private slots:
        void playPauseClicked();
        void stopClicked();

        void tempoDidChange(int bpm);
        void beatsPerBarDidChange(int beats);
        void patModeSelected();
        void songModeSelected();
};

#endif // TOPWIDGET_H

#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>
#include <QStyle>

#include "application.h"
#include "midi/midiinput.h"

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
        void updateFromProject(const Project& project);

    signals:
        void play();
        void pause();
        void stop();
        void patternChanged(int);
        void tempoChanged(int);
        void beatsPerBarChanged(int);
        void midiDeviceSet(int);

    private:
        Ui::TopWidget *ui;
        Application* _app;

        MIDIInput* _midiInput;

        bool _isPlaying;

    private slots:
        void playPauseClicked();
        void stopClicked();

        void tempoDidChange(int bpm);
        void beatsPerBarDidChange(int beats);
        void patModeSelected();
        void songModeSelected();
        void lfoModeChanged(int mode);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // TOPWIDGET_H

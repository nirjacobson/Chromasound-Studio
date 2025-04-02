#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>
#include <QStyle>

#include "application.h"
#include "formats/midi/midiinput.h"
#include "common/recordiconengine.h"

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
        void setStatusMessage(const QString& message);

        void doUpdate(const float position);

        void showUploadButton(bool show);

    signals:
        void play(bool record);
        void pause();
        void stop();
        void patternChanged(int);
        void tempoChanged(int);
        void beatsPerBarChanged(int);
        void midiDeviceSet(int);
        void userToneTriggered();
        void seekClicked(const float pos);
        void uploadClicked();

    private:
        Ui::TopWidget *ui;
        Application* _app;
        float _appPosition;

        MIDIInput* _midiInput;

        bool _isPlaying;

    private slots:
        void playPauseClicked();
        void stopClicked();

        void tempoDidChange(int bpm);
        void beatsPerBarDidChange(int beats);
        void patModeSelected();
        void songModeSelected();
        void chromasoundStopped();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // TOPWIDGET_H

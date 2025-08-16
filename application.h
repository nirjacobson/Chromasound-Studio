#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>
#include <QUndoStack>
#include <QPair>

#include "project/project.h"
#include "project/vgmstream.h"
#include "chromasound/chromasound.h"
#include "chromasound/emu/audio_output.h"

#include "chromasound/chromasound_standin.h"
#include "chromasound/chromasound_emu.h"
#include "chromasound/chromasound_dual.h"
#include "Chromasound_Studio.h"

#ifdef Q_OS_LINUX
#include "chromasound/chromasound_direct.h"
#endif

class MainWindow;

class Application : public QApplication
{
        Q_OBJECT

    public:
        Application(int &argc, char **argv, int flags = ApplicationFlags);
        ~Application();

        static bool isRaspberryPi();

        void pause();
        bool paused() const;
        void play();
        void play(const Pattern& pattern, const float loopStart, const float loopEnd);
        void play(const float loopStart, const float loopEnd);
        void stop();

        void uploadPCM(const QByteArray& pcm);

        void record();

        float position() const;
        void setPosition(const float pos);
        bool isPlaying() const;

        void setWindow(MainWindow* window);
        void showWindow();
        MainWindow* window();

        Project& project();

        void keyOn(const Channel& channel, const int key, const int velocity);
        void keyOn(const Channel& channel, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(const Channel &channel, int key);
        void pitchChange(const int index, const float pitch, const int pitchRange);

        QUndoStack& undoStack();

        Chromasound& chromasound();

        void ignoreCSTime(const bool ignore);

        void setupChromasound();

        void midiSync();

        const Track& recording() const;
        void clearRecording();

    signals:
        void pcmUploadStarted();
        void pcmUploadFinished();
        void compileStarted();
        void compileFinished();

    private:
        MainWindow* _mainWindow;
        QUndoStack _undoStack;

        AudioOutput<int16_t>* _output;
        Chromasound* _chromasound;

        Project _project;

        bool _paused;
        bool _ignoreCSTime;

        bool _recording;

        QElapsedTimer _recordingTimer;

        QMap<int, QPair<float, int>> _recordingMap;
        Track _recordingTrack;
};

#endif // APPLICATION_H

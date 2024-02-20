#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>
#include <QUndoStack>

#include "project/project.h"
#include "project/vgmstream.h"
#include "chromasound/chromasound.h"
#include "chromasound/chromasound_standin.h"
#include "chromasound/chromasound_direct.h"
#include "chromasound/chromasound_emu.h"
#include "chromasound/chromasound_proxy.h"
#include "Chromasound_Studio.h"

class MainWindow;

class Application : public QApplication
{
        Q_OBJECT

    public:
        Application(int &argc, char **argv, int flags = ApplicationFlags);
        ~Application();

        void pause();
        bool paused() const;
        void play();
        void play(const Pattern& pattern, const float loopStart, const float loopEnd);
        void play(const float loopStart, const float loopEnd);
        void stop();

        float position() const;
        void setPosition(const float pos);
        bool isPlaying() const;

        void setWindow(MainWindow* window);
        void showWindow();
        MainWindow* window();

        Project& project();

        void keyOn(const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);

        QUndoStack& undoStack();

        Chromasound& chromasound();

        void ignoreCSTime(const bool ignore);

        void setupChromasound();

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
};

#endif // APPLICATION_H

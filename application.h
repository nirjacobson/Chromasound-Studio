#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>
#include <QUndoStack>

#include "project/project.h"
#include "project/vgmstream.h"
#include "fm-psg/fm-psg.h"
#include "fm-psg/fm-psg_dummy.h"
#include "fm-psg/fm-psg_impl.h"
#include "fm-psg/fm-psg_soft.h"

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

    signals:
        void pcmUploadStarted();
        void pcmUploadFinished();

    private:
        MainWindow* _mainWindow;
        QUndoStack _undoStack;

        FM_PSG* _fmPSG;

        Project _project;

        bool _paused;
};

#endif // APPLICATION_H

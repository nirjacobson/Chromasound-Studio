#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>

#include "project/project.h"

class Application : public QApplication
{
    Q_OBJECT

    public:
        enum PlayMode {
            Pattern,
            Song
        };

        Application(int &argc, char **argv, int flags = ApplicationFlags);

        static constexpr int SampleRate = 44100;

        float position() const;
        bool isPlaying() const;

        void pause();
        void play();
        void stop();

        Project& project();

        PlayMode playMode() const;
        void setPlayMode(const PlayMode mode);

    private:
        static Application* _instance;

        Project _project;
        PlayMode _playMode;

        QElapsedTimer _timer;
        qint64 _ref;
        bool _playing;

        qint64 nanosecondsPerBeat() const;
};

#endif // APPLICATION_H

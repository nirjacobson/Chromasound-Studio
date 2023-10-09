#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>

#include "project/project.h"
#include "fm-psg/fm-psg.h"
#include "fm-psg/fm-psg_dummy.h"
#include "fm-psg/fm-psg_impl.h"

class Application : public QApplication
{
    Q_OBJECT

    public:
        Application(int &argc, char **argv, int flags = ApplicationFlags);

        void pause();
        void play();
        void stop();

        float position() const;
        bool isPlaying() const;

        Project& project();

    private:
        static Application* _instance;
        FM_PSG* _fmPSG;

        Project _project;

        QElapsedTimer _timer;

        qint64 nanosecondsPerBeat() const;
};

#endif // APPLICATION_H

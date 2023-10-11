#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QElapsedTimer>

#include "project/project.h"
#include "project/vgmstream.h"
#include "fm-psg/fm-psg.h"
#include "fm-psg/fm-psg_dummy.h"
#include "fm-psg/fm-psg_impl.h"

class Application : public QApplication
{
    Q_OBJECT

    public:
        Application(int &argc, char **argv, int flags = ApplicationFlags);
        ~Application();

        void pause();
        void play();
        void stop();

        float position() const;
        bool isPlaying() const;

        Project& project();

        void keyOn(const Channel::Type channelType, const Settings& settings, const int key, const int velocity);
        void keyOff(int key);

    private:
        static Application* _instance;
        FM_PSG* _fmPSG;

        Project _project;

        bool _paused;
};

#endif // APPLICATION_H

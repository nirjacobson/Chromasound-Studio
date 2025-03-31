#ifndef MIDIPOLLER_H
#define MIDIPOLLER_H

#include <QThread>
#include <QMutex>

#include "midiinput.h"

class MIDIPoller : public QThread
{
        Q_OBJECT

    signals:
        void receivedMessage(const long msg);
        void sync();

    public:
        MIDIPoller(QObject *parent = nullptr);
        ~MIDIPoller();

        void stop();

    public slots:
        void start(Priority p = InheritPriority);

        // QThread interface
    protected:
        void run();

    private:
        MIDIInput* _midiInput;

        QMutex _stopLock;
        bool _stop;
        bool _synced;
};

#endif // MIDIPOLLER_H

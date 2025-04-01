#include "midipoller.h"

MIDIPoller::MIDIPoller(QObject *parent)
    : QThread{parent}
    , _midiInput(MIDIInput::instance())
    , _stop(false)
    , _synced(true)
{

}

MIDIPoller::~MIDIPoller()
{

}

void MIDIPoller::stop()
{
    _stopLock.lock();
    _stop = true;
    _stopLock.unlock();
}

void MIDIPoller::start(Priority p)
{
    _stopLock.lock();
    _stop = false;
    _stopLock.unlock();

    QThread::start(p);
}

void MIDIPoller::run()
{
    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        _stopLock.unlock();
        if (stop) return;

        QList<long> messages;
        do {
            messages = _midiInput->read();
            if (!messages.empty()) {
                _synced = false;
                for (const long message : messages) {
                    emit receivedMessage(message);
                }
                QThread::msleep(5);
            }
        } while (!messages.empty());

        if (!_synced) {
            emit sync();
            _synced = true;
        }
    }
}

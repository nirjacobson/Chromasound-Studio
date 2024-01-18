#ifndef MIDIINPUT_H
#define MIDIINPUT_H

#include <QList>
#include <QString>

#include <portmidi.h>

class MIDIInput
{
    public:
        MIDIInput();

        static MIDIInput* instance();

        void init();
        void destroy();

        QList<QString> devices() const;

        void setDevice(const int index);
        int device() const;

        QList<long> read() const;

    private:
        static MIDIInput* _instance;

        static PmError _pmError;
        static PmDeviceID _pmDeviceId;
        static PortMidiStream* _pmStream;

        int _deviceIndex;
};

#endif // MIDIINPUT_H

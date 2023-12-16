#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <QDataStream>
#include <QIODevice>

#include "event.h"
#include "qtypes.h"

class MIDIEvent : public Event
{
        friend class MIDI;

    public:
        MIDIEvent(const quint8 status);
        MIDIEvent(const quint8 status, const quint8 data1);

        MIDIEvent& operator<<(QDataStream& stream);

        quint8 status() const;
        quint8 data1() const;
        quint8 data2() const;

        bool isKeyOn() const;
        bool isKeyOff() const;

    private:
        quint8 _status;
        quint8 _data1;
        quint8 _data2;

        bool _haveData1;

        // Event interface
    public:
        QByteArray encode() const;
};

#endif // MIDIEVENT_H

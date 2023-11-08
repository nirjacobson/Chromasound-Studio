#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <QDataStream>

#include "event.h"
#include "qtypes.h"

class MIDIEvent : public Event
{
public:
    MIDIEvent(const quint8 status);
    MIDIEvent(const quint8 status, const quint8 data1);

    MIDIEvent& operator<<(QDataStream& stream);

    quint8 status() const;

private:
    quint8 _status;
    quint8 _data1;
    quint8 _data2;

    bool _haveData1;
};

#endif // MIDIEVENT_H

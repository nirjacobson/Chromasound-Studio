#include "midievent.h"

MIDIEvent::MIDIEvent(const quint8 status)
    : _status(status)
    , _haveData1(false)
{

}

MIDIEvent::MIDIEvent(const quint8 status, const quint8 datum)
    : _status(status)
    , _data1(datum)
    , _haveData1(true)
{

}

MIDIEvent& MIDIEvent::operator<<(QDataStream& stream)
{
    if (!_haveData1) {
        stream >> _data1;
    }

    stream >> _data2;

    return *this;
}

quint8 MIDIEvent::status() const
{
    return _status;
}

quint8 MIDIEvent::data1() const
{
    return _data1;
}

quint8 MIDIEvent::data2() const
{
    return _data2;
}

bool MIDIEvent::isKeyOn() const
{
    return (_status >> 4) == 0x9;
}

bool MIDIEvent::isKeyOff() const
{
    return (_status >> 4) == 0x8;
}

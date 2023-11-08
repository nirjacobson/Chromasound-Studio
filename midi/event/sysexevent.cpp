#include "sysexevent.h"

SysexEvent::SysexEvent(quint8 event)
    : _event(event)
{

}

SysexEvent& SysexEvent::operator<<(QDataStream& stream)
{
    quint8 datum;

    _length = Util::getVariableLengthQuantity(stream);

    _data.resize(_length);
    for (quint32 i = 0; i < _length; i++) {
        stream >> _data[i];
    }

    return *this;
}

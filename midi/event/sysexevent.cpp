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

QByteArray SysexEvent::encode() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << _event;

    QByteArray length = Util::toVariableLengthQuantity(_data.size());
    stream.writeRawData(length.constData(), length.size());

    stream.writeRawData(_data.constData(), _data.size());

    return data;
}

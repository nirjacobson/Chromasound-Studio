#include "metaevent.h"

MetaEvent::MetaEvent(quint8 type)
    : _type(type)
{

}

MetaEvent& MetaEvent::operator<<(QDataStream& stream)
{
    _length = Util::getVariableLengthQuantity(stream);

    _data.resize(_length);
    for (quint32 i = 0; i < _length; i++) {
        stream >> _data[i];
    }

    return *this;
}

quint8 MetaEvent::type() const
{
    return _type;
}

quint32 MetaEvent::length() const
{
    return _length;
}

const QByteArray& MetaEvent::data() const
{
    return _data;
}

QByteArray MetaEvent::encode() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (quint8)0xFF;
    stream << _type;

    QByteArray length = Util::toVariableLengthQuantity(_data.size());
    stream.writeRawData(length.constData(), length.size());
    stream.writeRawData(_data.constData(), _data.size());

    return data;
}

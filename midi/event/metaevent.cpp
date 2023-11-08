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

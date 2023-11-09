#include "util.h"

Util::Util()
{

}

quint32 Util::getVariableLengthQuantity(QDataStream& stream)
{
    quint8 datum;
    quint32 value = 0;
    while (true) {
        stream >> datum;

        value = (value << 7) | (datum & 0x7F);

        if (!(datum & 0x80)) {
            break;
        }
    }

    return value;
}

QByteArray Util::toVariableLengthQuantity(quint32 value)
{
    QByteArray result;

    while (true) {
        result.prepend((value & 0x7F) | (result.isEmpty() ? 0x00 : 0x80));
        value >>= 7;

        if (value == 0) break;
    }

    return result;
}

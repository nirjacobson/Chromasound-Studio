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

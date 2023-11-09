#ifndef UTIL_H
#define UTIL_H

#include <QDataStream>
#include <QByteArray>
#include "qtypes.h"

class Util
{
public:
    Util();

    static quint32 getVariableLengthQuantity(QDataStream& stream);
    static QByteArray toVariableLengthQuantity(quint32 value);
};

#endif // UTIL_H

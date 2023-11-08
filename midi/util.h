#ifndef UTIL_H
#define UTIL_H

#include <QDataStream>
#include "qtypes.h"

class Util
{
public:
    Util();

    static quint32 getVariableLengthQuantity(QDataStream& stream);
};

#endif // UTIL_H

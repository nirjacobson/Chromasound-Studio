#ifndef METAEVENT_H
#define METAEVENT_H

#include <QDataStream>
#include <QByteArray>

#include "event.h"
#include "qtypes.h"
#include "../util.h"

class MetaEvent : public Event
{
public:
    MetaEvent(quint8 type);

    MetaEvent& operator<<(QDataStream& stream);

private:
    quint8 _type;
    quint32 _length;
    QByteArray _data;
};

#endif // METAEVENT_H

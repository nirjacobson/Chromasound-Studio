#ifndef SYSEXEVENT_H
#define SYSEXEVENT_H

#include <QByteArray>
#include <QDataStream>

#include "event.h"
#include "qtypes.h"
#include "../util.h"

class SysexEvent : public Event
{
public:
    SysexEvent(quint8 event);

    SysexEvent& operator<<(QDataStream& stream);

private:
    quint8 _event;
    quint32 _length;
    QByteArray _data;
};

#endif // SYSEXEVENT_H

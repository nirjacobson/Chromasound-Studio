#ifndef SYSEXEVENT_H
#define SYSEXEVENT_H

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

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

    // Event interface
public:
    QByteArray encode() const;
};

#endif // SYSEXEVENT_H

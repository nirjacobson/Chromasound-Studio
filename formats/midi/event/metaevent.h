#ifndef METAEVENT_H
#define METAEVENT_H

#include <QDataStream>
#include <QByteArray>
#include <QIODevice>

#include "event.h"
#include "qtypes.h"
#include "../util.h"

class MetaEvent : public Event
{
        friend class MIDI;

    public:
        MetaEvent(quint8 type);

        MetaEvent& operator<<(QDataStream& stream);

    private:
        quint8 _type;
        quint32 _length;
        QByteArray _data;

        // Event interface
    public:
        QByteArray encode() const;
};

#endif // METAEVENT_H

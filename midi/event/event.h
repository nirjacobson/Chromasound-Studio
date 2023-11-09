#ifndef EVENT_H
#define EVENT_H

#include <QByteArray>

class Event
{
public:
    Event();
    virtual ~Event();

    virtual QByteArray encode() const = 0;
};

#endif // EVENT_H

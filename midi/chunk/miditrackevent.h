#ifndef MIDITRACKEVENT_H
#define MIDITRACKEVENT_H

#include <QDataStream>

#include "qtypes.h"

#include "../event/event.h"
#include "../event/sysexevent.h"
#include "../event/metaevent.h"
#include "../event/midievent.h"
#include "../util.h"

class MIDITrackEvent
{
public:
    MIDITrackEvent(const quint8 lastMIDIStatus);
    MIDITrackEvent();
    ~MIDITrackEvent();

    MIDITrackEvent& operator<<(QDataStream& stream);

    const Event* event() const;

private:
    quint32 _deltaTime;
    Event* _event;

    quint8 _lastMIDIStatus;

    bool isMIDIStatus(const quint8 datum);
};

#endif // MIDITRACKEVENT_H

#ifndef GANTTMARKER_H
#define GANTTMARKER_H

#include <QString>

class GanttMarker
{
public:
    virtual ~GanttMarker() = 0;

    virtual float time() const = 0;
    virtual const QString& name() const = 0;
};

#endif // GANTTMARKER_H

#ifndef GANTTMARKER_H
#define GANTTMARKER_H

#include <QString>
#include <QColor>

class GanttMarker
{
    public:
        virtual ~GanttMarker() = 0;

        virtual float time() const = 0;
        virtual QString name() const = 0;
        virtual const QColor& color() const = 0;
};

#endif // GANTTMARKER_H

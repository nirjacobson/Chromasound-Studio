#ifndef GANTTITEM_H
#define GANTTITEM_H


class GanttItem
{
    public:
        virtual ~GanttItem() = 0;
        virtual long row() const = 0;
        virtual float time() const = 0;
        virtual float duration() const = 0;
};

#endif // GANTTITEM_H

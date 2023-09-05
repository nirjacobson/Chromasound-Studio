#ifndef GANTTITEM_H
#define GANTTITEM_H


class GanttItem
{
    public:
        virtual ~GanttItem() = 0;
        virtual long row() const = 0;
        virtual float time() const = 0;
        virtual float duration() const = 0;
        virtual int velocity() const = 0;

        virtual void setRow(const long row) = 0;
        virtual void setTime(const float time) = 0;
        virtual void setDuration(const float duration) = 0;
        virtual void setVelocity(const int velocity) = 0;
};

#endif // GANTTITEM_H

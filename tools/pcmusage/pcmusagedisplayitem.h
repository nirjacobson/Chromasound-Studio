#ifndef PCMUSAGEDISPLAYITEM_H
#define PCMUSAGEDISPLAYITEM_H

#include <QString>
#include <QColor>

class PCMUsageDisplayItem
{
    public:
        PCMUsageDisplayItem(const QString& name, const quint32 size, const QColor& color);

        const QString& name() const;
        const quint32 size() const;
        QString sizeString() const;
        const QColor& color() const;

    private:
        QString _name;
        quint32 _size;
        QColor _color;
};

#endif // PCMUSAGEDISPLAYITEM_H

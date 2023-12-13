#include "pcmusagedisplayitem.h"

PCMUsageDisplayItem::PCMUsageDisplayItem(const QString& name, const quint32 size, const QColor& color)
    : _name(name)
    , _size(size)
    , _color(color)
{

}

const QString& PCMUsageDisplayItem::name() const
{
    return _name;
}

const quint32 PCMUsageDisplayItem::size() const
{
    return _size;
}

QString PCMUsageDisplayItem::sizeString() const
{
    if (_size >= 1024) {
        return QString("%1 %2").arg(QString::number((float)_size / 1024, 'f', 2)).arg("KB");
    } else {
        return QString("%1 %2").arg(_size).arg("B");
    }
}

const QColor& PCMUsageDisplayItem::color() const
{
    return _color;
}

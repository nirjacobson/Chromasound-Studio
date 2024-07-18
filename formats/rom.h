#ifndef ROM_H
#define ROM_H

#include <QString>
#include <QList>
#include <QFile>
#include <QFileInfo>

class ROM
{
public:
    ROM(const QString& path);

    const QList<quint32>& offsets() const;
    const QList<QString>& names() const;

    quint32 size(const int index) const;

private:
    QList<quint32> _offsets;
    QList<QString> _names;
    quint32 _size;
};

#endif // ROM_H

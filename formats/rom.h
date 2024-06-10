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

    const QList<quint16>& offsets() const;
    const QList<QString>& names() const;

    long size(const int index) const;

private:
    QList<quint16> _offsets;
    QList<QString> _names;
    quint16 _size;
};

#endif // ROM_H

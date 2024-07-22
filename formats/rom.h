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

    quint16 size(const int index) const;
    void remove(const int index);

    void save(const QString& path);

private:
    QString _path;
    QList<quint32> _offsets;
    QList<QString> _names;
    quint32 _size;

    QByteArray header() const;
};

#endif // ROM_H

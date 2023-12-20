#ifndef GD3_H
#define GD3_H

#include <QString>

#include "project/project.h"

class GD3
{
public:

    static QByteArray generateGd3(const Project& project);
    static GD3 parseGd3(const QString& path);

    const QString& title() const;
    const QString& game() const;
    const QString& author() const;
    const QDate& releaseDate() const;

private:
    QString _title;
    QString _game;
    QString _author;
    QDate _releaseDate;
};

#endif // GD3_H

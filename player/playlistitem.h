#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QString>

class PlaylistItem
{
public:
    PlaylistItem(const QString& path, const QString& title, const QString& artist = "", const QString& album = "", const int length = 0);

    const QString& path() const;
    const QString& title() const;
    const QString& artist() const;
    const QString& album() const;
    const int length() const;

private:
    QString _path;
    QString _title;
    QString _artist;
    QString _album;
    int _length;
};

#endif // PLAYLISTITEM_H

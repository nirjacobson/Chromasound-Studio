#include "playlistitem.h"

PlaylistItem::PlaylistItem(const QString& path, const QString& title, const QString& artist, const QString& album, const int length)
    : _path(path)
    , _title(title)
    , _artist(artist)
    , _album(album)
    , _length(length)
{

}

const QString& PlaylistItem::path() const
{
    return _path;
}

const QString& PlaylistItem::title() const
{
    return _title;
}

const QString& PlaylistItem::artist() const
{
    return _artist;
}

const QString& PlaylistItem::album() const
{
    return _album;
}

const int PlaylistItem::length() const
{
    return _length;
}

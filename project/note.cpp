#include "note.h"

Note::Note(const int key, const float duration)
    : _key(key)
    , _duration(duration)
{

}

int Note::key() const
{
    return _key;
}

float Note::duration() const
{
    return _duration;
}

void Note::setKey(const int key)
{
    _key = key;
}

void Note::setDuration(const float duration)
{
    _duration = duration;
}

bool Note::operator==(const Note& note)
{
    return _key == note.key() && _duration == note.duration();
}

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

bool Note::operator==(const Note& note)
{
    return _key == note.key() && _duration == note.duration();
}

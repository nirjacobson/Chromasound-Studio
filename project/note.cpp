#include "note.h"

Note::Note(const int key, const float duration, const int velocity)
    : _key(key)
    , _duration(duration)
    , _velocity(velocity)
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

int Note::velocity() const
{
    return _velocity;
}

void Note::setKey(const int key)
{
    _key = key;
}

void Note::setDuration(const float duration)
{
    _duration = duration;
}

void Note::setVelocity(const int velocity)
{
    _velocity = velocity;
}

bool Note::operator==(const Note& note) const
{
    return _key == note.key() && _duration == note.duration() && _velocity == note.velocity();
}

Note::Note()
{

}

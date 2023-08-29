#ifndef NOTE_H
#define NOTE_H


class Note
{
    public:
        Note(const int key, const float duration);

        int key() const;
        float duration() const;

        bool operator==(const Note& note);

    private:
        int _key;
        float _duration;
};

#endif // NOTE_H

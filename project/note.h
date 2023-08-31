#ifndef NOTE_H
#define NOTE_H


class Note
{
    public:
        Note(const int key, const float duration);

        int key() const;
        float duration() const;

        void setKey(const int key);
        void setDuration(const float duration);

        bool operator==(const Note& note);

    private:
        int _key;
        float _duration;
};

#endif // NOTE_H

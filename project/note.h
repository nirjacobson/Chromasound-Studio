#ifndef NOTE_H
#define NOTE_H


class Note
{
        friend class BSON;

    public:
        Note(const int key, const float duration, const int velocity = 100);

        int key() const;
        float duration() const;
        int velocity() const;

        void setKey(const int key);
        void setDuration(const float duration);
        void setVelocity(const int velocity);

        bool operator==(const Note& note) const;

    private:
        Note();

        int _key;
        float _duration;
        int _velocity;
};

#endif // NOTE_H

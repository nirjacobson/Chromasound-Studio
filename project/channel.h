#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>

class Channel
{
        friend class BSON;

    public:
        Channel();

        bool enabled() const;
        void setEnabled(const bool enabled);

        QString name() const;
        void setName(const QString& name);

    private:
        bool _enabled;
        QString _name;
};

#endif // CHANNEL_H

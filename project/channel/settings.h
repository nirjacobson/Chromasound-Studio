#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>

#ifdef Q_OS_WINDOWS
#include <bson.h>
#else
#include <libbson-1.0/bson/bson.h>
#endif

class Settings
{
    public:
        virtual ~Settings();

        virtual bson_t toBSON() const = 0;
        virtual void fromBSON(bson_iter_t& bson) = 0;
};

#endif // SETTINGS_H

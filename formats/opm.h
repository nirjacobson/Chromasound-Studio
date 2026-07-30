#ifndef OPM_H
#define OPM_H

#include <QFile>
#include <QRegularExpression>

#include "project/channel/fmchannelsettings.h"

class OPM
{
public:
    OPM();

    static std::map<std::string, FMChannelSettings> parseOPM(const QString& path);
};

#endif // OPM_H

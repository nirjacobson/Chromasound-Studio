#include "pcmchannelsettings.h"
#include "project/channel/channel.h"

PCMChannelSettings::PCMChannelSettings()
{

}

const QString& PCMChannelSettings::path() const
{
    return _filePath;
}

void PCMChannelSettings::setPath(const QString& path)
{
    _filePath = path;
}

bson_t PCMChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_UTF8(&bson, "path", _filePath.toStdString().c_str());

    return bson;
}

void PCMChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t path;

    if (bson_iter_find_descendant(&bson, "path", &path) && BSON_ITER_HOLDS_UTF8(&path)) {
        _filePath = bson_iter_utf8(&path, nullptr);
    }
}

QString PCMChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::PCM);
}

ChannelSettings* PCMChannelSettings::copy() const
{
    PCMChannelSettings* pcs = new PCMChannelSettings;
    *pcs = *this;

    return pcs;
}

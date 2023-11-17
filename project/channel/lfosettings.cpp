#include "lfosettings.h"

LFOSettings::LFOSettings()
    : _ams(0)
    , _fms(0)
{

}

int LFOSettings::ams() const
{
    return _ams;
}

int LFOSettings::fms() const
{
    return _fms;
}

void LFOSettings::setAMS(const int ams)
{
    _ams = ams;
}

void LFOSettings::setFMS(const int fms)
{
    _fms = fms;
}

bool LFOSettings::operator==(const LFOSettings& other) const
{
    return _ams == other._ams && _fms == other._fms;
}

bson_t LFOSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    BSON_APPEND_INT32(&bson, "ams", _ams);
    BSON_APPEND_INT32(&bson, "fms", _fms);

    return bson;
}

void LFOSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t ams;
    bson_iter_t fms;

    if (bson_iter_find_descendant(&bson, "ams", &ams) && BSON_ITER_HOLDS_INT32(&ams)) {
        _ams = bson_iter_int32(&ams);
    }
    if (bson_iter_find_descendant(&bson, "fms", &fms) && BSON_ITER_HOLDS_INT32(&fms)) {
        _fms = bson_iter_int32(&fms);
    }
}

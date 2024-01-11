#include "ssgenvelopesettings.h"

SSGEnvelopeSettings::SSGEnvelopeSettings()
    : _cont(false)
    , _att(false)
    , _alt(false)
    , _hold(false)
{

}

bool SSGEnvelopeSettings::cont() const
{
    return _cont;
}

bool SSGEnvelopeSettings::att() const
{
    return _att;
}

bool SSGEnvelopeSettings::alt() const
{
    return _alt;
}

bool SSGEnvelopeSettings::hold() const
{
    return _hold;
}

void SSGEnvelopeSettings::setCont(bool enable)
{
    _cont = enable;
}

void SSGEnvelopeSettings::setAtt(bool enable)
{
    _att = enable;
}

void SSGEnvelopeSettings::setAlt(bool enable)
{
    _alt = enable;
}

void SSGEnvelopeSettings::setHold(bool enable)
{
    _hold = enable;
}

bool SSGEnvelopeSettings::operator==(const SSGEnvelopeSettings& other) const
{
    return _cont == other._cont &&
           _att == other._att &&
           _alt == other._alt &&
           _hold == other._hold;
}

bson_t SSGEnvelopeSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);
    BSON_APPEND_BOOL(&bson, "cont", _cont);
    BSON_APPEND_BOOL(&bson, "att", _att);
    BSON_APPEND_BOOL(&bson, "alt", _alt);
    BSON_APPEND_BOOL(&bson, "hold", _hold);

    return bson;
}

void SSGEnvelopeSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t cont;
    bson_iter_t att;
    bson_iter_t alt;
    bson_iter_t hold;

    if (bson_iter_find_descendant(&bson, "cont", &cont) && BSON_ITER_HOLDS_BOOL(&cont)) {
        _cont = bson_iter_bool(&cont);
    }
    if (bson_iter_find_descendant(&bson, "att", &att) && BSON_ITER_HOLDS_BOOL(&att)) {
        _att = bson_iter_bool(&att);
    }
    if (bson_iter_find_descendant(&bson, "alt", &alt) && BSON_ITER_HOLDS_BOOL(&alt)) {
        _alt = bson_iter_bool(&alt);
    }
    if (bson_iter_find_descendant(&bson, "hold", &hold) && BSON_ITER_HOLDS_BOOL(&hold)) {
        _hold = bson_iter_bool(&hold);
    }
}

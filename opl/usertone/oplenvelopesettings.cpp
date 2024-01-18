#include "oplenvelopesettings.h"

OPLEnvelopeSettings::OPLEnvelopeSettings()
    : _type(EGType::Sustained)
    , _ar(0)
    , _dr(0)
    , _sl(0)
    , _rr(15)
    , _tl(0)
{

}

OPLEnvelopeSettings::EGType OPLEnvelopeSettings::type() const
{
    return _type;
}

int OPLEnvelopeSettings::ar() const
{
    return _ar;
}

int OPLEnvelopeSettings::dr() const
{
    return _dr;
}

int OPLEnvelopeSettings::sl() const
{
    return _sl;
}

int OPLEnvelopeSettings::rr() const
{
    return _rr;
}

int OPLEnvelopeSettings::tl() const
{
    return _tl;
}

float OPLEnvelopeSettings::arf() const
{
    return (float)_ar / 15.0f;
}

float OPLEnvelopeSettings::drf() const
{
    return (float)_dr / 15.0f;
}

float OPLEnvelopeSettings::slf() const
{
    return (float)_sl / 15.0f;
}

float OPLEnvelopeSettings::rrf() const
{
    return (float)_rr / 15.0f;
}

float OPLEnvelopeSettings::tlf() const
{
    return (float)_tl / 63.0f;
}

void OPLEnvelopeSettings::setType(const EGType type)
{
    _type = type;
}

void OPLEnvelopeSettings::setAr(const int ar)
{
    _ar = ar;
}

void OPLEnvelopeSettings::setDr(const int dr)
{
    _dr = dr;
}

void OPLEnvelopeSettings::setSl(const int sl)
{
    _sl = sl;
}

void OPLEnvelopeSettings::setRr(const int rr)
{
    _rr = rr;
}

void OPLEnvelopeSettings::setTl(const int tl)
{
    _tl = tl;
}

bool OPLEnvelopeSettings::operator==(const OPLEnvelopeSettings& other) const
{
    return _type == other._type &&
           _ar == other._ar &&
           _dr == other._dr &&
           _sl == other._sl &&
           _rr == other._rr &&
           _tl == other._tl;
}

bson_t OPLEnvelopeSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);
    BSON_APPEND_UTF8(&bson, "type", _type == EGType::Percussive ? "Percussive" : "Sustained");
    BSON_APPEND_INT32(&bson, "ar", _ar);
    BSON_APPEND_INT32(&bson, "dr", _dr);
    BSON_APPEND_INT32(&bson, "sl", _sl);
    BSON_APPEND_INT32(&bson, "rr", _rr);
    BSON_APPEND_INT32(&bson, "tl", _tl);

    return bson;
}

void OPLEnvelopeSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t type;
    bson_iter_t ar;
    bson_iter_t dr;
    bson_iter_t sl;
    bson_iter_t rr;
    bson_iter_t tl;

    if (bson_iter_find_descendant(&bson, "type", &type) && BSON_ITER_HOLDS_UTF8(&type)) {
        QString typeStr(bson_iter_utf8(&type, nullptr));
        _type = typeStr == "Percussive" ? EGType::Percussive : EGType::Sustained;
    }

    if (bson_iter_find_descendant(&bson, "ar", &ar) && BSON_ITER_HOLDS_INT(&ar)) {
        _ar = bson_iter_int32(&ar);
    }

    if (bson_iter_find_descendant(&bson, "dr", &dr) && BSON_ITER_HOLDS_INT(&dr)) {
        _dr = bson_iter_int32(&dr);
    }

    if (bson_iter_find_descendant(&bson, "sl", &sl) && BSON_ITER_HOLDS_INT(&sl)) {
        _sl = bson_iter_int32(&sl);
    }

    if (bson_iter_find_descendant(&bson, "rr", &rr) && BSON_ITER_HOLDS_INT(&rr)) {
        _rr = bson_iter_int32(&rr);
    }

    if (bson_iter_find_descendant(&bson, "tl", &tl) && BSON_ITER_HOLDS_INT(&tl)) {
        _tl = bson_iter_int32(&tl);
    }
}

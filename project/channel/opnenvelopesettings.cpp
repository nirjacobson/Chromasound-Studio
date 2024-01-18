#include "opnenvelopesettings.h"

OPNEnvelopeSettings::OPNEnvelopeSettings()
    : _ar(0)
    , _t1l(0)
    , _d1r(0)
    , _t2l(0)
    , _d2r(0)
    , _rr(15)
{

}

int OPNEnvelopeSettings::ar() const
{
    return _ar;
}

int OPNEnvelopeSettings::t1l() const
{
    return _t1l;
}

int OPNEnvelopeSettings::d1r() const
{
    return _d1r;
}

int OPNEnvelopeSettings::t2l() const
{
    return _t2l;
}

int OPNEnvelopeSettings::d2r() const
{
    return _d2r;
}

int OPNEnvelopeSettings::rr() const
{
    return _rr;
}

float OPNEnvelopeSettings::arf() const
{
    return (float)_ar / 31.0f;
}

float OPNEnvelopeSettings::t1lf() const
{
    return (float)_t1l / 127.0f;
}

float OPNEnvelopeSettings::d1rf() const
{
    return (float)_d1r / 31.0f;
}

float OPNEnvelopeSettings::t2lf() const
{
    return (float)_t2l / 15.0f;
}

float OPNEnvelopeSettings::d2rf() const
{
    return (float)_d2r / 31.0f;
}

float OPNEnvelopeSettings::rrf() const
{
    return (float)_rr / 15.0f;
}

void OPNEnvelopeSettings::setAr(const int ar)
{
    _ar = ar;
}

void OPNEnvelopeSettings::setT1l(const int t1l)
{
    _t1l = t1l;
}

void OPNEnvelopeSettings::setD1r(const int d1r)
{
    _d1r = d1r;
}

void OPNEnvelopeSettings::setT2l(const int t2l)
{
    _t2l = t2l;
}

void OPNEnvelopeSettings::setD2r(const int d2r)
{
    _d2r = d2r;
}

void OPNEnvelopeSettings::setRr(const int rr)
{
    _rr = rr;
}

bool OPNEnvelopeSettings::operator==(const OPNEnvelopeSettings& other) const
{
    return _ar == other._ar &&
           _t1l == other._t1l &&
           _d1r == other._d1r &&
           _t2l == other._t2l &&
           _d2r == other._d2r &&
           _rr == other._rr;
}

bson_t OPNEnvelopeSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);
    BSON_APPEND_INT32(&bson, "ar", _ar);
    BSON_APPEND_INT32(&bson, "t1l", _t1l);
    BSON_APPEND_INT32(&bson, "d1r", _d1r);
    BSON_APPEND_INT32(&bson, "t2l", _t2l);
    BSON_APPEND_INT32(&bson, "d2r", _d2r);
    BSON_APPEND_INT32(&bson, "rr", _rr);

    return bson;
}

void OPNEnvelopeSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t ar;
    bson_iter_t t1l;
    bson_iter_t d1r;
    bson_iter_t t2l;
    bson_iter_t d2r;
    bson_iter_t rr;

    if (bson_iter_find_descendant(&bson, "ar", &ar) && BSON_ITER_HOLDS_INT(&ar)) {
        _ar = bson_iter_int32(&ar);
    }
    if (bson_iter_find_descendant(&bson, "t1l", &t1l) && BSON_ITER_HOLDS_INT(&t1l)) {
        _t1l = bson_iter_int32(&t1l);
    }
    if (bson_iter_find_descendant(&bson, "d1r", &d1r) && BSON_ITER_HOLDS_INT(&d1r)) {
        _d1r = bson_iter_int32(&d1r);
    }
    if (bson_iter_find_descendant(&bson, "t2l", &t2l) && BSON_ITER_HOLDS_INT(&t2l)) {
        _t2l = bson_iter_int32(&t2l);
    }
    if (bson_iter_find_descendant(&bson, "d2r", &d2r) && BSON_ITER_HOLDS_INT(&d2r)) {
        _d2r = bson_iter_int32(&d2r);
    }
    if (bson_iter_find_descendant(&bson, "rr", &rr) && BSON_ITER_HOLDS_INT(&rr)) {
        _rr = bson_iter_int32(&rr);
    }
}

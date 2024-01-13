#include "fm4envelopesettings.h"

FM4EnvelopeSettings::FM4EnvelopeSettings()
    : _ar(0)
    , _t1l(0)
    , _d1r(0)
    , _t2l(0)
    , _d2r(0)
    , _rr(15)
{

}

int FM4EnvelopeSettings::ar() const
{
    return _ar;
}

int FM4EnvelopeSettings::t1l() const
{
    return _t1l;
}

int FM4EnvelopeSettings::d1r() const
{
    return _d1r;
}

int FM4EnvelopeSettings::t2l() const
{
    return _t2l;
}

int FM4EnvelopeSettings::d2r() const
{
    return _d2r;
}

int FM4EnvelopeSettings::rr() const
{
    return _rr;
}

float FM4EnvelopeSettings::arf() const
{
    return (float)_ar / 31.0f;
}

float FM4EnvelopeSettings::t1lf() const
{
    return (float)_t1l / 127.0f;
}

float FM4EnvelopeSettings::d1rf() const
{
    return (float)_d1r / 31.0f;
}

float FM4EnvelopeSettings::t2lf() const
{
    return (float)_t2l / 15.0f;
}

float FM4EnvelopeSettings::d2rf() const
{
    return (float)_d2r / 31.0f;
}

float FM4EnvelopeSettings::rrf() const
{
    return (float)_rr / 15.0f;
}

void FM4EnvelopeSettings::setAr(const int ar)
{
    _ar = ar;
}

void FM4EnvelopeSettings::setT1l(const int t1l)
{
    _t1l = t1l;
}

void FM4EnvelopeSettings::setD1r(const int d1r)
{
    _d1r = d1r;
}

void FM4EnvelopeSettings::setT2l(const int t2l)
{
    _t2l = t2l;
}

void FM4EnvelopeSettings::setD2r(const int d2r)
{
    _d2r = d2r;
}

void FM4EnvelopeSettings::setRr(const int rr)
{
    _rr = rr;
}

bool FM4EnvelopeSettings::operator==(const FM4EnvelopeSettings& other) const
{
    return _ar == other._ar &&
           _t1l == other._t1l &&
           _d1r == other._d1r &&
           _t2l == other._t2l &&
           _d2r == other._d2r &&
           _rr == other._rr;
}

bson_t FM4EnvelopeSettings::toBSON() const
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

void FM4EnvelopeSettings::fromBSON(bson_iter_t& bson)
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

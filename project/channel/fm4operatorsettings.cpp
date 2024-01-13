#include "fm4operatorsettings.h"

FM4OperatorSettings::FM4OperatorSettings()
    : _mul(1)
    , _dt(0)
    , _rs(0)
    , _am(false)
{

}

FM4EnvelopeSettings& FM4OperatorSettings::envelopeSettings()
{
    return _envelopeSettings;
}

const FM4EnvelopeSettings& FM4OperatorSettings::envelopeSettings() const
{
    return _envelopeSettings;
}

int FM4OperatorSettings::mul() const
{
    return _mul;
}

int FM4OperatorSettings::dt() const
{
    return _dt;
}

int FM4OperatorSettings::rs() const
{
    return _rs;
}

bool FM4OperatorSettings::am() const
{
    return _am;
}

void FM4OperatorSettings::setEnvelopeSettings(const FM4EnvelopeSettings& settings)
{
    _envelopeSettings = settings;
}

void FM4OperatorSettings::setMul(const int mul)
{
    _mul = mul;
}

void FM4OperatorSettings::setDt(const int dt)
{
    _dt = dt;
}

void FM4OperatorSettings::setRs(const int rs)
{
    _rs = rs;
}

void FM4OperatorSettings::setAm(const bool am)
{
    _am = am;
}

bool FM4OperatorSettings::operator==(const FM4OperatorSettings& other) const
{
    return _envelopeSettings == other._envelopeSettings &&
           _rs == other._rs &&
           _mul == other._mul &&
           _dt == other._dt &&
           _am == other._am;
}

bson_t FM4OperatorSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    bson_t envelope = _envelopeSettings.toBSON();
    BSON_APPEND_DOCUMENT(&bson, "envelope", &envelope);
    BSON_APPEND_INT32(&bson, "mul", _mul);
    BSON_APPEND_INT32(&bson, "dt", _dt);
    BSON_APPEND_INT32(&bson, "rs", _rs);
    BSON_APPEND_BOOL(&bson, "am", _am);

    return bson;
}

void FM4OperatorSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t envelope;
    bson_iter_t envelopeInner;
    bson_iter_t mul;
    bson_iter_t dt;
    bson_iter_t rs;
    bson_iter_t am;

    if (bson_iter_find_descendant(&bson, "envelope", &envelope) && BSON_ITER_HOLDS_DOCUMENT(&envelope) && bson_iter_recurse(&envelope, &envelopeInner)) {
        _envelopeSettings.fromBSON(envelopeInner);
    }
    if (bson_iter_find_descendant(&bson, "mul", &mul) && BSON_ITER_HOLDS_INT(&mul)) {
        _mul = bson_iter_int32(&mul);
    }
    if (bson_iter_find_descendant(&bson, "dt", &dt) && BSON_ITER_HOLDS_INT(&dt)) {
        _dt = bson_iter_int32(&dt);
    }
    if (bson_iter_find_descendant(&bson, "rs", &rs) && BSON_ITER_HOLDS_INT(&rs)) {
        _rs = bson_iter_int32(&rs);
    }
    if (bson_iter_find_descendant(&bson, "am", &am) && BSON_ITER_HOLDS_BOOL(&am)) {
        _am = bson_iter_bool(&am);
    }
}

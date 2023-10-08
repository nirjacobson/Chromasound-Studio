#include "operatorsettings.h"

OperatorSettings::OperatorSettings()
    : _mul(1)
    , _dt(0)
    , _rs(0)
    , _am(false)
{

}

EnvelopeSettings& OperatorSettings::envelopeSettings()
{
    return _envelopeSettings;
}

const EnvelopeSettings& OperatorSettings::envelopeSettings() const
{
    return _envelopeSettings;
}

int OperatorSettings::mul() const
{
    return _mul;
}

int OperatorSettings::dt() const
{
    return _dt;
}

int OperatorSettings::rs() const
{
    return _rs;
}

bool OperatorSettings::am() const
{
    return _am;
}

void OperatorSettings::setEnvelopeSettings(const EnvelopeSettings& settings)
{
    _envelopeSettings = settings;
}

void OperatorSettings::setMul(const int mul)
{
    _mul = mul;
}

void OperatorSettings::setDt(const int dt)
{
    _dt = dt;
}

void OperatorSettings::setRs(const int rs)
{
    _rs = rs;
}

void OperatorSettings::setAm(const bool am)
{
    _am = am;
}

bool OperatorSettings::operator==(const OperatorSettings& other) const
{
    return _envelopeSettings == other._envelopeSettings &&
            _rs == other._rs &&
            _mul == other._mul &&
            _dt == other._dt &&
            _am == other._am;
}

bson_t OperatorSettings::toBSON() const
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

void OperatorSettings::fromBSON(bson_iter_t& bson)
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
